/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */


#include "gos.h"
#include "hurricane_gpio.h"



/*************************************************************************************************/
gos_result_t gos_board_hardware_init(void)
{
    gos_result_t result;

    GOS_LOG("Initializing Hurricane-specific hardware ...");


    // Initialize the GPIO Expanders and turn on key components
    if(GOS_FAILED(result, hurricane_gpio_init()))
    {
    }
    else if(GOS_FAILED(result, hurricane_gpio_write(HURRICANE_GPIO_3V3_AUX_EN, true)))
    {
    }
    else if(GOS_FAILED(result, hurricane_gpio_write(HURRICANE_GPIO_LCD_BACKLIGHT_EN, true)))
    {
    }
    else if(GOS_FAILED(result, hurricane_gpio_write(HURRICANE_GPIO_LCD_SUPPLY_EN, true)))
    {
    }
    else if(GOS_FAILED(result, hurricane_gpio_write(HURRICANE_GPIO_I2C_MUX, true)))
    {
    }
    else
    {
        //delay to give components time to power on (the si115x/3x sensors require min 25ms start time)
        gos_rtos_delay_milliseconds(100);
    }

    return result;
}
