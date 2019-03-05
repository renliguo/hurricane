/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */
/*
 * Documentation for this app is available online.
 * See https://docs.silabs.com/gecko-os/latest/sdk/examples/basic/blinky
 */

#include "gos.h"

#include "GUI.h"
#include "GUIDRV_Lin.h"

#include "hurricane_gpio.h"
#include "ili9163c.h"



#define USE_18BPP

// emWin driver configuration
#ifdef USE_18BPP
  #define COLOR_CONVERSION GUICC_888
  #define DISPLAY_DRIVER GUIDRV_LIN_24
  #define BYTES_PER_PIXEL  3              // Need 3 bytes (24-bits) to fit 18bpp
#else
  #define COLOR_CONVERSION GUICC_565     
  #define DISPLAY_DRIVER GUIDRV_LIN_16   
  #define BYTES_PER_PIXEL  2
#endif

#if LCD_GAMMA == 1
	static const uint8_t pGammaSet[15]= {0x36,0x29,0x12,0x22,0x1C,0x15,0x42,0xB7,0x2F,0x13,0x12,0x0A,0x11,0x0B,0x06};
	static const uint8_t nGammaSet[15]= {0x09,0x16,0x2D,0x0D,0x13,0x15,0x40,0x48,0x53,0x0C,0x1D,0x25,0x2E,0x34,0x39};
#elif LCD_GAMMA == 2
	static const uint8_t pGammaSet[15]= {0x3F,0x21,0x12,0x22,0x1C,0x15,0x42,0xB7,0x2F,0x13,0x02,0x0A,0x01,0x00,0x00};
	static const uint8_t nGammaSet[15]= {0x09,0x18,0x2D,0x0D,0x13,0x15,0x40,0x48,0x53,0x0C,0x1D,0x25,0x2E,0x24,0x29};
#elif LCD_GAMMA == 3
	static const uint8_t pGammaSet[15]= {0x3F,0x26,0x23,0x30,0x28,0x10,0x55,0xB7,0x40,0x19,0x10,0x1E,0x02,0x01,0x00};
	static const uint8_t nGammaSet[15]= {0x09,0x18,0x2D,0x0D,0x13,0x15,0x40,0x48,0x53,0x0C,0x1D,0x25,0x2E,0x24,0x29};
#else
	static const uint8_t pGammaSet[15]= {0x3F,0x25,0x1C,0x1E,0x20,0x12,0x2A,0x90,0x24,0x11,0x00,0x00,0x00,0x00,0x00};
	static const uint8_t nGammaSet[15]= {0x20,0x20,0x20,0x20,0x05,0x15,0x00,0xA7,0x3D,0x18,0x25,0x2A,0x2B,0x2B,0x3A};
#endif

static gos_spi_device_t spi;

// sized for 24 bits per pixel
#define FRAME_SIZE (BYTES_PER_PIXEL * LCD_WIDTH * LCD_HEIGHT)
static uint8_t vram[FRAME_SIZE * LCD_N_BUFFERS];

static int8_t pending_buffer;

static gos_thread_t display_thread;
static gos_event_flags_t display_event_flags;
#define EVENT_DISPLAY_BUFFER_SWITCH 1


// emWin configuration callback
void LCD_X_Config(void) {
  
  // initialize multiple buffering
  GUI_MULTIBUF_Config(LCD_N_BUFFERS);

  GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER, COLOR_CONVERSION, 0, 0);

  LCD_SetVRAMAddrEx(0, vram);

  //
  // Display driver configuration
  //
  if (LCD_GetSwapXY()) {
    LCD_SetSizeEx (0, LCD_HEIGHT, LCD_WIDTH);
    LCD_SetVSizeEx(0, LCD_HEIGHT, LCD_WIDTH);
  } else {
    LCD_SetSizeEx (0, LCD_WIDTH, LCD_HEIGHT);
    LCD_SetVSizeEx(0, LCD_WIDTH, LCD_HEIGHT);
  }
}

// emWin display callback
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * p) {
  GUI_USE_PARA(LayerIndex);
  GUI_USE_PARA(Cmd);
  GUI_USE_PARA(p);

  GOS_INTERRUPT_CONTEXT();

  LCD_X_SHOWBUFFER_INFO * pData;

  if(Cmd == LCD_X_SHOWBUFFER)
  {
    pData = (LCD_X_SHOWBUFFER_INFO *)p;
    
    GOS_DISABLE_INTERRUPTS();
    pending_buffer = pData->Index;
    GOS_ENABLE_INTERRUPTS();

    gos_rtos_event_flags_set(&display_event_flags, EVENT_DISPLAY_BUFFER_SWITCH);
  }

  return 0;
}

static void assert_cs(void)
{	
	// gos_spi_master_lock_bus(&spi, 5000);
	hurricane_gpio_write(HURRICANE_GPIO_LCD_CS, false);
}

static void release_cs(void)
{
	hurricane_gpio_write(HURRICANE_GPIO_LCD_CS, true);	
	// gos_spi_master_unlock_bus(&spi);
}


static void lcd_command(uint8_t command, const uint8_t *parameters, uint32_t param_len)
{
	gos_spi_message_t msg;
	msg.tx_buffer = &command;
	msg.rx_buffer = 0;
	msg.length = sizeof(command);

	spi.lcd_bit8 = false; // specify command signal

	assert_cs();
	gos_spi_transfer(&spi, &msg, 1);
	release_cs();

	// note that the CS line will be deasserted between
	// calls to gos_spi_transfer.
	// this is ok, the LCD expects this between command
	// and data bytes.

	// setup data transfer
	msg.rx_buffer = 0;
	msg.tx_buffer = (void *)parameters;
	msg.length = param_len;

	spi.lcd_bit8 = true; // specify data signal

	assert_cs();
	gos_spi_transfer(&spi, &msg, 1);
	release_cs();
}

static void set_column_address(uint16_t x0, uint16_t x1)
{
	uint8_t parameters[4];

	parameters[0] = (x0 >> 8) & 0xff;
	parameters[1] = (x0 >> 0) & 0xff;
	parameters[2] = (x1 >>  8) & 0xff;
	parameters[3] = (x1 >>  0) & 0xff;

	lcd_command(ILI9163_CMD_CLMADRS, parameters, sizeof(parameters));
}

static void set_page_address(uint16_t y0, uint16_t y1)
{
	uint8_t parameters[4];

	parameters[0] = (y0 >> 8) & 0xff;
	parameters[1] = (y0 >> 0) & 0xff;
	parameters[2] = (y1 >>  8) & 0xff;
	parameters[3] = (y1 >>  0) & 0xff;

	lcd_command(ILI9163_CMD_PGEADRS, parameters, sizeof(parameters));
}

static void set_rotation(uint8_t rotation)
{
	uint8_t mad_ctl = 0;

	rotation %= 4; // can't be higher than 3

	switch (rotation) {
		case 0:
			mad_ctl = 0b00001000;
			break;
		case 1:
			mad_ctl = 0b01101000;
			break;
		case 2:
			mad_ctl = 0b11001000;
			break;
		case 3:
			mad_ctl = 0b10101000;
			break;
	}

	lcd_command(ILI9163_CMD_MADCTL, &mad_ctl, sizeof(mad_ctl));
}

static void set_addr_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{

	set_column_address(x0, x1);
	set_page_address(y0, y1);
}


void update_display(int8_t buffer_index)
{
	if((buffer_index >= LCD_N_BUFFERS) || (buffer_index < 0))
	{
		return;
	}

	set_addr_window(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);

  	void *buffer = vram + (buffer_index * FRAME_SIZE);

	gos_spi_message_t msg;
	msg.tx_buffer = buffer;
	msg.rx_buffer = 0;
	msg.length = FRAME_SIZE;

	// start RAM write command
	lcd_command(ILI9163_CMD_RAMWR, 0, 0);

	spi.lcd_bit8 = true;

	assert_cs();
	gos_spi_transfer(&spi, &msg, 1);
	release_cs();

	// send NOP to terminate write command
	lcd_command(ILI9163_CMD_NOP, 0, 0); 
}


static void display_update_thread(void *arg)
{
	GOS_INTERRUPT_CONTEXT();

	uint32_t flags = 0;

	while(1)
	{
		gos_rtos_event_flags_wait_for_any(&display_event_flags, EVENT_DISPLAY_BUFFER_SWITCH, &flags, GOS_WAIT_FOREVER);
		
		GOS_DISABLE_INTERRUPTS();
		int8_t buffer = pending_buffer;
		GOS_ENABLE_INTERRUPTS();

		if(buffer >= 0)
		{
			GOS_DISABLE_INTERRUPTS();
			pending_buffer = -1;
			GOS_ENABLE_INTERRUPTS();
			
			GUI_MULTIBUF_Confirm(buffer);

			update_display(buffer);
		}
	}
}


void lcd_init(void)
{
	// return;
	uint8_t parameters[8];

    spi.port 			= LCD_SPI;
    spi.chip_select 	= GOS_GPIO_INVALID;
    // spi.chip_select 	= GOS_GPIO_28;
    spi.speed 			= LCD_SPI_CLK;
    spi.bits 			= 8;
    spi.flags 			= GOS_SPI_MASTER_MODE | 
    					  GOS_SPI_CLOCK_RISING_EDGE | 
    					  GOS_SPI_CLOCK_IDLE_LOW |
    					  GOS_SPI_MSB_FIRST | 
    					  GOS_SPI_LCD_9BIT_MODE |
    					  GOS_SPI_DISABLE_AUTO_CS;

    // gos_gpio_init(LCD_GPIO_RST, GOS_GPIO_OUTPUT_PUSH_PULL, true);
    // gos_gpio_init(spi.chip_select, GOS_GPIO_OUTPUT_PUSH_PULL, true);

    hurricane_gpio_write(HURRICANE_GPIO_LCD_CS, true);

    // reset
    // gos_gpio_set(LCD_GPIO_RST, false);
    hurricane_gpio_write(HURRICANE_GPIO_LCD_RESET, false);
	gos_rtos_delay_milliseconds(1);    
	// gos_gpio_set(LCD_GPIO_RST, true);    
	hurricane_gpio_write(HURRICANE_GPIO_LCD_RESET, true);
	gos_rtos_delay_milliseconds(200);    

	lcd_command(ILI9163_CMD_SWRESET, 0, 0); // software reset
	gos_rtos_delay_milliseconds(500);

	lcd_command(ILI9163_CMD_SLPOUT, 0, 0); // exit sleep
	gos_rtos_delay_milliseconds(120);

	lcd_command(ILI9163_CMD_DISPOFF, 0, 0); // display OFF
	gos_rtos_delay_milliseconds(20);

#ifdef USE_18BPP
	parameters[0] = 0x06; // Set Color Format 18 bit
#else
	parameters[0] = 0x05; // Set Color Format 16 bit
#endif	
	lcd_command(ILI9163_CMD_PIXFMT, parameters, 1);
	gos_rtos_delay_milliseconds(100);
	
	parameters[0] = 0x04;
	
	
	

	parameters[0] = 0x04;
	lcd_command(ILI9163_CMD_GAMMASET, parameters, 1); //default gamma curve 3
	gos_rtos_delay_milliseconds(1);

	parameters[0] = 0x01;
	lcd_command(ILI9163_CMD_GAMRSEL, parameters, 1);//Enable Gamma adj
	gos_rtos_delay_milliseconds(1);

	lcd_command(ILI9163_CMD_NORML, 0, 0);

	parameters[0] = 0b11111111;
	parameters[1] = 0b00000110;
	lcd_command(ILI9163_CMD_DFUNCTR, parameters, 2);
	
	lcd_command(ILI9163_CMD_PGAMMAC, pGammaSet, sizeof(pGammaSet));//Positive Gamma Correction Setting
	
	lcd_command(ILI9163_CMD_NGAMMAC, nGammaSet, sizeof(nGammaSet));//Negative Gamma Correction Setting
		
	parameters[0] = 0x08;
	parameters[1] = 0x02;
	lcd_command(ILI9163_CMD_FRMCTR1, parameters, 2);//Frame Rate Control (In normal mode/Full colors)
	gos_rtos_delay_milliseconds(1);

	parameters[0] = 0x07;
	lcd_command(ILI9163_CMD_DINVCTR, parameters, 1);//display inversion
	gos_rtos_delay_milliseconds(1);

	parameters[0] = 0x0A; // 4.3V
	parameters[1] = 0x02;
	lcd_command(ILI9163_CMD_PWCTR1, parameters, 2);//Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
	gos_rtos_delay_milliseconds(1);

	parameters[0] = 0x02;
	lcd_command(ILI9163_CMD_PWCTR2, parameters, 1);//Set BT[2:0] for AVDD & VCL & VGH & VGL
	gos_rtos_delay_milliseconds(1);

	parameters[0] = 0x50;
	parameters[1] = 99;
	lcd_command(ILI9163_CMD_VCOMCTR1, parameters, 2);//Set VMH[6:0] & VML[6:0] for VOMH & VCOML
	gos_rtos_delay_milliseconds(1);

	parameters[0] = 0x00;	
	lcd_command(ILI9163_CMD_VCOMOFFS, parameters, 1);
	gos_rtos_delay_milliseconds(1);

	set_column_address(0, LCD_WIDTH);
	gos_rtos_delay_milliseconds(1);

	set_page_address(0, LCD_HEIGHT);
	gos_rtos_delay_milliseconds(1);


	parameters[0] = 0x00;
	parameters[1] = 0x00;
	parameters[2] = 0x00;
	parameters[3] = 0x00;
	parameters[4] = LCD_HEIGHT;
	parameters[5] = 0x00;
	lcd_command(ILI9163_CMD_VSCLLDEF, parameters, 6);
	gos_rtos_delay_milliseconds(1);

	set_rotation(3);


	// clear display VRAM
	update_display(0);


	lcd_command(ILI9163_CMD_DISPON, 0, 0);//display ON


	// set up display event
	gos_rtos_event_flags_init(&display_event_flags);

	// create display update thread
	gos_thread_config_t config;
    config.function = display_update_thread;
    config.stack_size = 4096;
    config.stack_buffer = 0;
    config.arg = 0;
    config.priority = GOS_THREAD_PRIORITY_HIGHER;
    gos_rtos_thread_create(&display_thread, &config);
}

