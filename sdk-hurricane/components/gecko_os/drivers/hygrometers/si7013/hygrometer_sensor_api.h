/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */

#pragma once

#include "gos.h"



typedef struct
{
    gos_i2c_t port;
    bool print_init_details;  //!< Print initialization details
    bool block_while_measure;
    uint8_t init_retries;
} hygrometer_config_t;


/**
 * Hygrometer data
 */
typedef struct
{
    float humidity;
    float temperature;
} hygrometer_data_t;


