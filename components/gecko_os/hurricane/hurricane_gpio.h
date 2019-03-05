/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */


#pragma once

#include "gos.h"



typedef enum
{
    HURRICANE_GPIO_LED1,
    HURRICANE_GPIO_LED2,
    HURRICANE_GPIO_LED3,
    HURRICANE_GPIO_LED4,
    HURRICANE_GPIO_LED5,
    HURRICANE_GPIO_3V3_AUX_EN,
    HURRICANE_GPIO_LCD_SUPPLY_EN,
    HURRICANE_GPIO_LCD_BACKLIGHT_EN,
    HURRICANE_GPIO_LCD_RESET,
    HURRICANE_GPIO_LCD_CS,
    HURRICANE_GPIO_I2C_MUX,
    HURRICANE_GPIO_ETH_PHY_EN,
    HURRICANE_GPIO_ETH_RJ45_YELLOW_LED,
    HURRICANE_GPIO_ETH_PHY_RESET,
    HURRICANE_GPIO_AMP_SHUTDOWN,
    HURRICANE_GPIO_SPEAKER_EN
} hurricane_gpio_t;




void hurricane_gpio_init(void);
void hurricane_gpio_write(hurricane_gpio_t gpio, bool state);
