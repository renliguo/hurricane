/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */

#pragma once

#include "si7013.h"
#include "si72xx.h"
#include "si115xdrv.h"
#include "icm20648.h"
#include "bmp280.h"
#include "bmp280_sys_out.h"


#define SI1133_UVALS_ADDR  				0x55
#define SI1153_PROX_GEST_ADDR  			0x53
#define SI1153_LONG_RANGE_PROX_ADDR  	0x52

#define SI1133_PART_ID 					0x33
#define SI1153_PART_ID 					0x53
#define SI7210_PART_ID 					0x01
#define SI7210_BASE_PART_NUM    0xA

#define ECC508A_PART_NUM				0x50
#define ECC608A_PART_NUM				0x60

/*  The callback function must return 0 on success and non-zero on fail  */
typedef void (*hurricane_sensor_message_callback_t)(char *arg);


void hurricane_sensors_init(void);

float hurricane_sensor_get_temp(void);
float hurricane_sensor_get_RH(void);
float hurricane_sensor_get_pressure(void);
float hurricane_sensor_get_UV(void);
float hurricane_sensor_get_ALS(void);
float hurricane_sensor_get_gesture_1(void);
float hurricane_sensor_get_gesture_2(void);
float hurricane_sensor_get_prox(void);
float hurricane_sensor_get_mag(void);
float hurricane_sensor_get_accel_x(void);
float hurricane_sensor_get_accel_y(void);
float hurricane_sensor_get_accel_z(void);
void hurricane_sensor_read_accel_immediate(float *x, float *y, float *z);
void hurricane_sensors_start_periodic_measurements(void);
void hurricane_sensors_stop_periodic_measurements(void);

uint32_t hurricane_sensor_self_test(hurricane_sensor_message_callback_t message_callback);

