/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */

#include "gos.h"
#include "ili9163c.h"


/* If the SPI CS is from a standard GPIO or IO expander then use the callback mechanism
   othwerise CS will be handler by the GOS SPI peripheral
*/
#if ILI9163_GPIO_SPI_CS ==  true
  #define  ILI9163C_ASSERT_SPI_CS()  handle->assert_cs()
  #define  ILI9163C_RELEASE_SPI_CS()  handle->release_cs()
#else
  #define  ILI9163C_ASSERT_SPI_CS()  
  #define  ILI9163C_RELEASE_SPI_CS()  
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


static void ili9163c_set_column_address(ili9163c_device_t *handle, uint16_t x0, uint16_t x1);
static void ili9163c_set_page_address(ili9163c_device_t *handle, uint16_t y0, uint16_t y1);
static void ili9163c_set_addr_window(ili9163c_device_t *handle, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);





gos_result_t ili9163c_lcd_command(ili9163c_device_t *handle, uint8_t command, const uint8_t *parameters, uint32_t param_len)
{ gos_result_t result = GOS_SUCCESS;
  gos_spi_message_t msg;
  msg.tx_buffer = &command;
  msg.rx_buffer = 0;
  msg.length = sizeof(command);

  handle->spi->lcd_bit8 = false; // specify command signal

  ILI9163C_ASSERT_SPI_CS();
  gos_spi_transfer(handle->spi, &msg, 1);
  ILI9163C_RELEASE_SPI_CS();

  // note that the CS line will be deasserted between
  // calls to gos_spi_transfer.
  // this is ok, the LCD expects this between command
  // and data bytes.

  // setup data transfer
  msg.rx_buffer = 0;
  msg.tx_buffer = (void *)parameters;
  msg.length = param_len;

  handle->spi->lcd_bit8 = true; // specify data signal

  ILI9163C_ASSERT_SPI_CS();
  gos_spi_transfer(handle->spi, &msg, 1);
  ILI9163C_RELEASE_SPI_CS();
  return result;
}

static void ili9163c_set_column_address(ili9163c_device_t *handle, uint16_t x0, uint16_t x1)
{
  uint8_t parameters[4];

  parameters[0] = (x0 >> 8) & 0xff;
  parameters[1] = (x0 >> 0) & 0xff;
  parameters[2] = (x1 >>  8) & 0xff;
  parameters[3] = (x1 >>  0) & 0xff;

  ili9163c_lcd_command(handle, ILI9163_CMD_CLMADRS, parameters, sizeof(parameters));
}

static void ili9163c_set_page_address(ili9163c_device_t *handle, uint16_t y0, uint16_t y1)
{
  uint8_t parameters[4];

  parameters[0] = (y0 >> 8) & 0xff;
  parameters[1] = (y0 >> 0) & 0xff;
  parameters[2] = (y1 >>  8) & 0xff;
  parameters[3] = (y1 >>  0) & 0xff;

  ili9163c_lcd_command(handle, ILI9163_CMD_PGEADRS, parameters, sizeof(parameters));
}

gos_result_t ili9163c_set_rotation(ili9163c_device_t *handle, uint8_t rotation)
{ gos_result_t result = GOS_SUCCESS;
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

  result = ili9163c_lcd_command(handle, ILI9163_CMD_MADCTL, &mad_ctl, sizeof(mad_ctl));
  return result;
}

static void ili9163c_set_addr_window(ili9163c_device_t *handle, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{

  ili9163c_set_column_address(handle, x0, x1);
  ili9163c_set_page_address(handle, y0, y1);
}



gos_result_t ili9163c_update_display(ili9163c_device_t *handle, int8_t buffer_index)
{ gos_result_t result = GOS_SUCCESS;
  gos_spi_message_t msg;
  uint32_t frame_size = handle->lcd_width * handle->lcd_height * ((handle->use_18bpp == true) ? 3 : 2);
  void *buffer = handle->vram + (buffer_index * frame_size);

  if((buffer_index >= handle->num_buffers) || (buffer_index < 0))
  { result = GOS_BAD_ARG;
    return result;
  }

  ili9163c_set_addr_window(handle, 0, 0, handle->lcd_width - 1, handle->lcd_height - 1);

  msg.tx_buffer = buffer;
  msg.rx_buffer = 0;
  msg.length = frame_size;

  // start RAM write command
  ili9163c_lcd_command(handle, ILI9163_CMD_RAMWR, 0, 0);

  handle->spi->lcd_bit8 = true;

  ILI9163C_ASSERT_SPI_CS();
  gos_spi_transfer(handle->spi, &msg, 1);
  ILI9163C_RELEASE_SPI_CS();

  // send NOP to terminate write command
  ili9163c_lcd_command(handle, ILI9163_CMD_NOP, 0, 0); 
  return result;
}



gos_result_t ili9163c_lcd_init(ili9163c_device_t *handle)
{ gos_result_t result = GOS_SUCCESS;
  // return;
  uint8_t parameters[8];

  ili9163c_lcd_command(handle, ILI9163_CMD_SWRESET, 0, 0); // software reset
  gos_rtos_delay_milliseconds(500);

  ili9163c_lcd_command(handle, ILI9163_CMD_SLPOUT, 0, 0); // exit sleep
  gos_rtos_delay_milliseconds(120);

  ili9163c_lcd_command(handle, ILI9163_CMD_DISPOFF, 0, 0); // display OFF
  gos_rtos_delay_milliseconds(20);

  if(handle->use_18bpp == true)
    parameters[0] = 0x06; // Set Color Format 18 bit
  else
    parameters[0] = 0x05; // Set Color Format 16 bit
  ili9163c_lcd_command(handle, ILI9163_CMD_PIXFMT, parameters, 1);
  gos_rtos_delay_milliseconds(100);
  
  parameters[0] = 0x04;
  ili9163c_lcd_command(handle, ILI9163_CMD_GAMMASET, parameters, 1); //default gamma curve 3
  gos_rtos_delay_milliseconds(1);

  parameters[0] = 0x01;
  ili9163c_lcd_command(handle, ILI9163_CMD_GAMRSEL, parameters, 1);//Enable Gamma adj
  gos_rtos_delay_milliseconds(1);

  ili9163c_lcd_command(handle, ILI9163_CMD_NORML, 0, 0);

  parameters[0] = 0b11111111;
  parameters[1] = 0b00000110;
  ili9163c_lcd_command(handle, ILI9163_CMD_DFUNCTR, parameters, 2);
  
  ili9163c_lcd_command(handle, ILI9163_CMD_PGAMMAC, pGammaSet, sizeof(pGammaSet));//Positive Gamma Correction Setting
  
  ili9163c_lcd_command(handle, ILI9163_CMD_NGAMMAC, nGammaSet, sizeof(nGammaSet));//Negative Gamma Correction Setting
    
  parameters[0] = 0x08;
  parameters[1] = 0x02;
  ili9163c_lcd_command(handle, ILI9163_CMD_FRMCTR1, parameters, 2);//Frame Rate Control (In normal mode/Full colors)
  gos_rtos_delay_milliseconds(1);

  parameters[0] = 0x07;
  ili9163c_lcd_command(handle, ILI9163_CMD_DINVCTR, parameters, 1);//display inversion
  gos_rtos_delay_milliseconds(1);

  parameters[0] = 0x0A; // 4.3V
  parameters[1] = 0x02;
  ili9163c_lcd_command(handle, ILI9163_CMD_PWCTR1, parameters, 2);//Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
  gos_rtos_delay_milliseconds(1);

  parameters[0] = 0x02;
  ili9163c_lcd_command(handle, ILI9163_CMD_PWCTR2, parameters, 1);//Set BT[2:0] for AVDD & VCL & VGH & VGL
  gos_rtos_delay_milliseconds(1);

  parameters[0] = 0x50;
  parameters[1] = 99;
  ili9163c_lcd_command(handle, ILI9163_CMD_VCOMCTR1, parameters, 2);//Set VMH[6:0] & VML[6:0] for VOMH & VCOML
  gos_rtos_delay_milliseconds(1);

  parameters[0] = 0x00; 
  ili9163c_lcd_command(handle, ILI9163_CMD_VCOMOFFS, parameters, 1);
  gos_rtos_delay_milliseconds(1);

  ili9163c_set_column_address(handle, 0, handle->lcd_width);
  gos_rtos_delay_milliseconds(1);

  ili9163c_set_page_address(handle, 0, handle->lcd_height);
  gos_rtos_delay_milliseconds(1);


  parameters[0] = 0x00;
  parameters[1] = 0x00;
  parameters[2] = 0x00;
  parameters[3] = 0x00;
  parameters[4] = handle->lcd_height;
  parameters[5] = 0x00;
  ili9163c_lcd_command(handle, ILI9163_CMD_VSCLLDEF, parameters, 6);
  gos_rtos_delay_milliseconds(1);

  ili9163c_set_rotation(handle, handle->rotation);

// clear display VRAM
  ili9163c_update_display(handle, 0);


  ili9163c_lcd_command(handle, ILI9163_CMD_DISPON, 0, 0);//display ON
  
  return result;
}

