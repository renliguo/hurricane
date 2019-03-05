/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */


#include "gos.h"
#include "hurricane_gpio.h"
#include "pca9535.h"


/*************************************************************************************************
* Non-Exported Definitions
**************************************************************************************************/
#define PCA9535_DEVICE1_ADDR     0x20
#define PCA9535_DEVICE2_ADDR     0x21

/*************************************************************************************************
* Types
**************************************************************************************************/
typedef struct{
  uint8_t device;
  uint8_t bit;
  bool reverse_polarity;
} pca_gpio_t;


/*************************************************************************************************
* Non-Exported Variables
**************************************************************************************************/

static pca_gpio_t pca_gpio[] = 
{
/*GPIO Expander 0 */
  {0,   0,  true},    // GPIO_LED1
  {0,   1,  true},    // GPIO_LED2
  {0,   2,  true},    // GPIO_LED3
  {0,   3,  true},    // GPIO_LED4
  {0,   4,  true},    // GPIO_LED5
  {0,   5,  false},   // GPIO_3V3_AUX_EN
  {0,   6,  false},   // GPIO_LCD_SUPPLY_EN
  {0,   7,  false},   // GPIO_LCD_BACKLIGHT_EN
  {0,   8,  false},   // GPIO_LCD_RESET
  {1,   9,  false},   // GPIO_LCD_CS
  {0,   15, false},   // GPIO_I2C_MUX
  {0,   12, false},   // GPIO_ETH_PHY_EN
  {0,   13, false},   // GPIO_ETH_RJ45_YELLOW_LED
  {0,   14, false},   // GPIO_ETH_PHY_RESET
/* GPIO Expander 1 */
  {1,     14, true},      // AMP_SHUTDOWN
  {1,     15, false},     // SPEAKER_EN
};

static uint16_t pca_dir[2];
static uint16_t pca_out[2];


static gos_i2c_device_t pca9535_i2c_device[] =
{
  {
      .port    = PLATFORM_STD_I2C,
      .address = PCA9535_DEVICE1_ADDR,
      .speed   = GOS_I2C_CLOCK_HIGH_SPEED,
      .retries = 0,
      .flags   = 0
  },
  {
      .port    = PLATFORM_STD_I2C,
      .address = PCA9535_DEVICE2_ADDR,
      .speed   = GOS_I2C_CLOCK_HIGH_SPEED,
      .retries = 0,
      .flags   = 0
  }
};


/*************************************************************************************************
* hurricane_gpio_init()
*
**************************************************************************************************/
void hurricane_gpio_init(void)
{
// reset I2C devices

// init to inputs
  pca_dir[0] = 0xffff;
  pca_dir[1] = 0xffff;
  pca9535_set_dir(&pca9535_i2c_device[0], 0xffff);
  pca9535_set_dir(&pca9535_i2c_device[1], 0xffff);

// set all output drivers to low
  pca9535_write_output(&pca9535_i2c_device[0], 0x0000); 
  pca9535_write_output(&pca9535_i2c_device[1], 0x0000); 
  
}


/*************************************************************************************************
* hurricane_gpio_write()
*
**************************************************************************************************/
void hurricane_gpio_write(hurricane_gpio_t gpio, bool state)
{
  // bounds check
  if(gpio >= ARRAY_COUNT(pca_gpio))
  {
    return;
  }

  if(pca_gpio[gpio].reverse_polarity)
  {
    state = !state;
  }

  pca_dir[pca_gpio[gpio].device] &= ~(1 << pca_gpio[gpio].bit);

  if(state)
  {
    pca_out[pca_gpio[gpio].device] |= (1 << pca_gpio[gpio].bit);
  }
  else
  {
    pca_out[pca_gpio[gpio].device] &= ~(1 << pca_gpio[gpio].bit);
  }

  pca9535_write_output(&pca9535_i2c_device[pca_gpio[gpio].device], pca_out[pca_gpio[gpio].device]);   
  pca9535_set_dir(&pca9535_i2c_device[pca_gpio[gpio].device], pca_dir[pca_gpio[gpio].device]); 
}


