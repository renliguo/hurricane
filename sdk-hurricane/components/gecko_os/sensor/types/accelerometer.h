/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */
#pragma once


#include "gos.h"
#ifdef FEATURE_SENSOR_TYPE_ACCELEROMETER_ENABLED
// This is defined by the specific driver
#include "accelerometer_sensor_api.h"

#else
typedef void* accelerometer_config_t;
typedef void* accelerometer_data_t;
#endif


/**
 * Initialise accelerometer driver.
 *
 * @param[in] config: config used by initialisation function (specific to device)
 * @return @ref gos_result_t
 */
gos_result_t sensor_accelerometer_init(const accelerometer_config_t *config);

/**
 * Poll accelerometer to see if new data is available
 *
 * @param[out] has_data: True if there is new data available
 * @return @ref gos_result_t
 */
gos_result_t sensor_accelerometer_has_new_data(bool *has_data);

/**
 * Start measuring the sensor
 *
 * @return @ref gos_result_t
 */
gos_result_t sensor_accelerometer_start_measurement(void);

/**
 * Get latest data from accelerometer sensor
 *
 * @param[out] data: Data from sensor in mG
 * @return @ref gos_result_t
 */
gos_result_t sensor_accelerometer_get_data(accelerometer_data_t *data);

