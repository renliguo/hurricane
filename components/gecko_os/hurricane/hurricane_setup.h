/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */

#pragma once

/*************************************************************************************************
* Exported Defines
**************************************************************************************************/

#define HURRICANE_NAVIGATION_Y0_COORD              110
#define HURRICANE_SETUP_DISPLAY_NAV_FONT_COLOR     0x247dde
#define HURRICANE_SETUP_TYPICAL_MARGIN             5


/*************************************************************************************************
* Exported Functions
**************************************************************************************************/

void hurricane_enter_device_setup(hurricane_demo_t *handle);
void hurricane_setup_process_button(uint32_t button);
void hurricane_draw_message_screen(char *message, int align, char *button1_str, char *button2_str);


