/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */

#pragma once



void hurricane_dms_init(void);
gos_result_t hurricane_dms_request_echo(char *echo_str, gos_handler_t handler);
void hurricane_dms_request_pin(gos_handler_t pin, gos_handler_t claim);

void hurricane_init_sensor_data(uint32_t number_of_items);
void hurricane_add_sensor_data(char *name, float data);
void hurricane_flush_sensor_data(void);

void hurricane_post_game_score(float points);
void hurricane_dms_dfu(void);

