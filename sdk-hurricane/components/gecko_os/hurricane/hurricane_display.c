/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */

/*************************************************************************************************/
#include "gos.h"
#include "GUI.h"
#include "GUIDRV_Lin.h"
#include "ili9163c.h"
#include "hurricane.h"



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


static void hurricane_display_assert_cs(void);
static void hurricane_display_release_cs(void);
static void hurricane_display_update_thread(void *arg);




// sized for 24 bits per pixel
#define FRAME_SIZE (BYTES_PER_PIXEL * LCD_WIDTH * LCD_HEIGHT)
static uint8_t vram[FRAME_SIZE * LCD_N_BUFFERS] __attribute__((aligned(4)));

static gos_thread_t display_thread;
static gos_event_flags_t display_event_flags;
#define EVENT_DISPLAY_BUFFER_SWITCH 1


static int8_t pending_buffer;

static gos_spi_device_t spi = 
{
  .port        = GOS_SPI_1,
  .chip_select = GOS_GPIO_INVALID,     //Chips select is via the I2C IO Expander
  // .chip_select  = GOS_GPIO_28,    
  .speed       = 15000000,
  .bits        = 8,
  .flags       = GOS_SPI_MASTER_MODE |          \
                 GOS_SPI_CLOCK_RISING_EDGE |    \
                 GOS_SPI_CLOCK_IDLE_LOW |       \
                 GOS_SPI_MSB_FIRST |            \
                 GOS_SPI_LCD_9BIT_MODE |        \
                 GOS_SPI_DISABLE_AUTO_CS
  
};

ili9163c_device_t ili9163c_device = 
{ .spi = &spi,
  .assert_cs = hurricane_display_assert_cs,
  .release_cs = hurricane_display_release_cs,
  .vram = vram,
  .num_buffers = LCD_N_BUFFERS,
  .lcd_height = LCD_HEIGHT,
  .lcd_width = LCD_WIDTH,
  .rotation = 3,
#ifdef USE_18BPP
  .use_18bpp = true
#else
  .use_18bpp = false
#endif
};



void hurricane_init_display(void)
{
//Set CS high
  hurricane_gpio_write(HURRICANE_GPIO_LCD_CS, true);

// Reset the Display 
  hurricane_gpio_write(HURRICANE_GPIO_LCD_RESET, false);
	gos_rtos_delay_milliseconds(1);    
	hurricane_gpio_write(HURRICANE_GPIO_LCD_RESET, true);
	gos_rtos_delay_milliseconds(200);    

// Initialize the display driver
  ili9163c_lcd_init(&ili9163c_device);

// set up display event
  gos_rtos_event_flags_init(&display_event_flags);

// create display update thread
  gos_thread_config_t config;
  config.function = hurricane_display_update_thread;
  config.stack_size = 4096;
  config.stack_buffer = 0;
  config.arg = 0;
  config.priority = GOS_THREAD_PRIORITY_HIGHER;
  gos_rtos_thread_create(&display_thread, &config);

}


static void hurricane_display_assert_cs(void)
{ 
  gos_spi_master_lock_bus(&spi, 5000);
  hurricane_gpio_write(HURRICANE_GPIO_LCD_CS, false);
}

static void hurricane_display_release_cs(void)
{
  hurricane_gpio_write(HURRICANE_GPIO_LCD_CS, true);  
  gos_spi_master_unlock_bus(&spi);
}




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


static void hurricane_display_update_thread(void *arg)
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

      ili9163c_update_display(&ili9163c_device, buffer);
    }
  }
}


