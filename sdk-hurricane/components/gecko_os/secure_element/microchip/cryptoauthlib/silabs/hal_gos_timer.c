/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */

#include "gos.h"

#include "atca_hal.h"

void atca_delay_us(uint32_t delay)
{
    gos_wait_microseconds(delay);
}

void atca_delay_10us(uint32_t delay)
{
    gos_wait_microseconds(delay * 10);
}

void atca_delay_ms(uint32_t delay)
{   
    gos_rtos_delay_milliseconds(delay);
}
