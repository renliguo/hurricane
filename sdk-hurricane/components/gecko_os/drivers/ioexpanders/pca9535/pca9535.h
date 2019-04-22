/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */

#pragma once



#include "gos.h"



#define PCA9535_DEVICE1_ADDR     0x20
#define PCA9535_DEVICE2_ADDR     0x21


gos_result_t pca9535_read_input(const gos_i2c_device_t *i2c_device, uint32_t *value_ptr);
gos_result_t pca9535_write_output(const gos_i2c_device_t *i2c_device, uint16_t param);
gos_result_t pca9535_set_dir(const gos_i2c_device_t *i2c_device, uint16_t param);
