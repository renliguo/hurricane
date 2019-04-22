/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */

#pragma once

#include "gos_result.h"

/**
 * This specifies hardware configuration specific to the 'Hurricane v1' development board
 *
 * This overrides default defines in: <root>/hardware/gecko_os/platforms/<code>/platform_common.h
 */


#define PLATFORM_BUTTON1 GOS_GPIO_20

#define PLATFORM_BUTTON2 GOS_GPIO_17

#define PLATFORM_STD_I2C GOS_I2C_0

