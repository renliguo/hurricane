/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */

#pragma once

#include "gos.h"

#define ICM20648_ADDR                   0x68

#define ICM20648_REG_BANK_SEL           0x7F


#define ICM20648_WHOAMI                 0xE0


// bank 0:

#define ICM20648_REG_WHOAMI             0x00
#define ICM20648_REG_USER_CTRL          0x03
#define ICM20648_REG_PWR_MGMT_1         0x06
#define ICM20648_REG_PWR_MGMT_2         0x07

#define ICM20648_REG_ACCEL_XOUT_H       0x2D
#define ICM20648_REG_ACCEL_XOUT_L       0x2E
#define ICM20648_REG_ACCEL_YOUT_H       0x2F
#define ICM20648_REG_ACCEL_YOUT_L       0x30
#define ICM20648_REG_ACCEL_ZOUT_H       0x31
#define ICM20648_REG_ACCEL_ZOUT_L       0x32

#define ICM20648_REG_GYRO_XOUT_H        0x33
#define ICM20648_REG_GYRO_XOUT_L        0x34
#define ICM20648_REG_GYRO_YOUT_H        0x35
#define ICM20648_REG_GYRO_YOUT_L        0x36
#define ICM20648_REG_GYRO_ZOUT_H        0x37
#define ICM20648_REG_GYRO_ZOUT_L        0x38

#define ICM20648_REG_TEMP_OUT_H         0x39
#define ICM20648_REG_TEMP_OUT_L         0x3A


typedef struct{
    const gos_i2c_device_t *i2c_device;
    uint8_t current_bank;
} icm20648_device_t;


gos_result_t icm20648_init(icm20648_device_t *device);
gos_result_t icm20648_read_id(icm20648_device_t *device, uint8_t *id_ptr);
gos_result_t icm20648_read_accel(icm20648_device_t *device, float *x_ptr, float *y_ptr, float *z_ptr);

