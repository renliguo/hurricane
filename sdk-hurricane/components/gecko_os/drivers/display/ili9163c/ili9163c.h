/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */

#pragma once

#include "gos.h"


//Configuration defines
#define LCD_GAMMA 		0
#define ILI9163_GPIO_SPI_CS   true


#define ILI9163_CMD_NOP     	0x00 // Non operation
#define ILI9163_CMD_SWRESET 	0x01 // Soft Reset
#define ILI9163_CMD_SLPIN   	0x10 // Sleep ON
#define ILI9163_CMD_SLPOUT  	0x11 // Sleep OFF
#define ILI9163_CMD_PTLON   	0x12 // Partial Mode ON
#define ILI9163_CMD_NORML   	0x13 // Normal Display ON
#define ILI9163_CMD_DINVOF  	0x20 // Display Inversion OFF
#define ILI9163_CMD_DINVON   	0x21 // Display Inversion ON
#define ILI9163_CMD_GAMMASET 	0x26 // Gamma Set (0x01[1],0x02[2],0x04[3],0x08[4])
#define ILI9163_CMD_DISPOFF 	0x28 // Display OFF
#define ILI9163_CMD_DISPON  	0x29 // Display ON
#define ILI9163_CMD_IDLEON  	0x39 // Idle Mode ON
#define ILI9163_CMD_IDLEOF  	0x38 // Idle Mode OFF
#define ILI9163_CMD_CLMADRS   	0x2A // Column Address Set
#define ILI9163_CMD_PGEADRS   	0x2B // Page Address Set

#define ILI9163_CMD_RAMWR   	0x2C // Memory Write
#define ILI9163_CMD_RAMRD   	0x2E // Memory Read
#define ILI9163_CMD_CLRSPACE   	0x2D // Color Space : 4K/65K/262K
#define ILI9163_CMD_PARTAREA	0x30 // Partial Area
#define ILI9163_CMD_VSCLLDEF	0x33 // Vertical Scroll Definition
#define ILI9163_CMD_TEFXLON		0x35 // Tearing Effect Line ON
#define ILI9163_CMD_TEFXLOF		0x34 // Tearing Effect Line OFF
#define ILI9163_CMD_MADCTL  	0x36 // Memory Access Control
#define ILI9163_CMD_VSSTADRS	0x37 // Vertical Scrolling Start address
#define ILI9163_CMD_PIXFMT  	0x3A // Interface Pixel Format
#define ILI9163_CMD_FRMCTR1 	0xB1 // Frame Rate Control (In normal mode/Full colors)
#define ILI9163_CMD_FRMCTR2 	0xB2 // Frame Rate Control(In Idle mode/8-colors)
#define ILI9163_CMD_FRMCTR3 	0xB3 // Frame Rate Control(In Partial mode/full colors)
#define ILI9163_CMD_DINVCTR		0xB4 // Display Inversion Control
#define ILI9163_CMD_RGBBLK		0xB5 // RGB Interface Blanking Porch setting
#define ILI9163_CMD_DFUNCTR 	0xB6 // Display Fuction set 5
#define ILI9163_CMD_SDRVDIR 	0xB7 // Source Driver Direction Control
#define ILI9163_CMD_GDRVDIR 	0xB8 // Gate Driver Direction Control 

#define ILI9163_CMD_PWCTR1  	0xC0 // Power_Control1
#define ILI9163_CMD_PWCTR2  	0xC1 // Power_Control2
#define ILI9163_CMD_PWCTR3  	0xC2 // Power_Control3
#define ILI9163_CMD_PWCTR4  	0xC3 // Power_Control4
#define ILI9163_CMD_PWCTR5  	0xC4 // Power_Control5
#define ILI9163_CMD_VCOMCTR1  	0xC5 // VCOM_Control 1
#define ILI9163_CMD_VCOMCTR2  	0xC6 // VCOM_Control 2
#define ILI9163_CMD_VCOMOFFS  	0xC7 // VCOM Offset Control
#define ILI9163_CMD_PGAMMAC		0xE0 // Positive Gamma Correction Setting
#define ILI9163_CMD_NGAMMAC		0xE1 // Negative Gamma Correction Setting
#define ILI9163_CMD_GAMRSEL		0xF2 // GAM_R_SEL


typedef struct ili9163c_device
{ gos_spi_device_t *spi;
  void (*assert_cs)();
  void (*release_cs)();
  uint8_t *vram;            // pointer to the display buffer
  uint8_t num_buffers;
  uint16_t lcd_height;
  uint16_t lcd_width;
  uint8_t rotation;         // screen rotation 0 to 3
  bool use_18bpp;           // true = 18bpp, false = 16bpp
  
}ili9163c_device_t;


gos_result_t ili9163c_lcd_init(ili9163c_device_t *handle);
gos_result_t ili9163c_lcd_command(ili9163c_device_t *handle, uint8_t command, const uint8_t *parameters, uint32_t param_len);
gos_result_t ili9163c_set_rotation(ili9163c_device_t *handle, uint8_t rotation);
gos_result_t ili9163c_update_display(ili9163c_device_t *handle, int8_t buffer_index);


