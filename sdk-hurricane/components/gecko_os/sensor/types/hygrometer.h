/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */
#pragma once

#include "gos.h"

#ifdef FEATURE_SENSOR_TYPE_HYGROMETER_ENABLED
// This is defined by the specific driver
#include "hygrometer_sensor_api.h"

#else
typedef void* hygrometer_config_t;
typedef void* hygrometer_data_t;
#endif



/**
 * Initialise hygrometer driver.
 *
 * @param[in] config: config used by initialisation function
 * @return @ref gos_result_t
 */
gos_result_t sensor_hygrometer_init(const hygrometer_config_t *config);

/**
 * Poll hygrometer to see if new data is available
 *
 * @param[out] has_data: True if there is new data available
 * @return @ref gos_result_t
 */
gos_result_t sensor_hygrometer_has_new_data(bool *has_data);

/**
 * Start measuring the sensor
 *
 * @return @ref gos_result_t
 */
gos_result_t sensor_hygrometer_start_measurement(void);

/**
 * Get latest data from hygrometer
 *
 * @param[out] data: See @ref hygrometer_data_t
 * @return @ref gos_result_t
 */
gos_result_t sensor_hygrometer_get_data(hygrometer_data_t *data);

