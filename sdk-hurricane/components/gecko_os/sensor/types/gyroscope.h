/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */

#pragma once


#include "gos.h"

#ifdef FEATURE_SENSOR_TYPE_GYROSCOPE_ENABLED
// This is defined by the specific driver
#include "gyroscope_sensor_api.h"

#else
typedef void* gyroscope_config_t;
typedef void* gyroscope_data_t;
#endif



/**
 * Initialise gyroscope driver.
 *
 * @param[in] config: config used by initialisation function
 * @return @ref gos_result_t
 */
gos_result_t sensor_gyroscope_init(const gyroscope_config_t *config);

/**
 * Poll gyroscope to see if new data is available
 *
 * @param[out] has_data: True if there is new data available
 * @return @ref gos_result_t
 */
gos_result_t sensor_gyroscope_has_new_data(bool *has_data);

/**
 * Start measuring the sensor
 *
 * @return @ref gos_result_t
 */
gos_result_t sensor_gyroscope_start_measurement(void);

/**
 * Get latest data from gyroscope
 *
 * @param[out] data: See @ref gyroscope_data_t
 * @return @ref gos_result_t
 */
gos_result_t sensor_gyroscope_get_data(gyroscope_data_t *data);



