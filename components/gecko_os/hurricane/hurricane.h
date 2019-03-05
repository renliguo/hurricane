/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */

#pragma once

#include "hurricane_cfg.h"
#include "ili9163c.h"
#include <stdint.h>
#include <stdbool.h>
#include "GUI.h"

#define HURRICANE_DEMO_NAME_MAX_LEN          32


/*  The callback function must return 0 on success and non-zero on fail  */
typedef int (*hurricane_splash_screen_callback_t)(void *arg);


typedef struct hurricane_demo
{ int8_t demo_name[HURRICANE_DEMO_NAME_MAX_LEN];
  gos_handler_t start_demo_callback;
  hurricane_splash_screen_callback_t draw_splash_screen_callback;  //can pass NULL for default
  void *demo_splash_screen_arg;
  uint32_t splash_font_color;
} hurricane_demo_t;

// The below header files must be included after the hurricane_demo_t typedef
#include "hurricane_app.h"
#include "hurricane_display.h"
#include "hurricane_setup.h"
#include "hurricane_dms.h"
#include "hurricane_sensors.h"
#include "hurricane_gpio.h"


#define   HURRICANE_BUTTON1    GOS_GPIO_20
#define   HURRICANE_BUTTON2    GOS_GPIO_17

#define PCA9535_DEVICE1_ADDR     0x20
#define PCA9535_DEVICE2_ADDR     0x21

#define HURRICANE_SPLASH_DEFAULT_FONT_COLOR      HURRICANE_SETUP_DISPLAY_NAV_FONT_COLOR
#define HURRICANE_SPLASH_DEFAULT_FUNCTION        hurricane_draw_jpeg