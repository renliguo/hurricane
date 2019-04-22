/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */
#pragma once

#include "gos.h"


#include "types/hygrometer.h"
#include "types/thermometer.h"
#include "types/accelerometer.h"
#include "types/gyroscope.h"
#include "types/magnetometer.h"



/**
 * @addtogroup lib_sensor
 * @{
 */


/** 
 *  Sensor id enumeration
 */
typedef enum
{
#ifdef FEATURE_SENSOR_TYPE_ACCELEROMETER_ENABLED
    SENSOR_ACCELEROMETER,   //!< Accelerometer
#endif
#ifdef FEATURE_SENSOR_TYPE_HYGROMETER_ENABLED
    SENSOR_HYGROMETER,      //!< Hygrometer
#endif
#ifdef FEATURE_SENSOR_TYPE_THERMOMETER_ENABLED
    SENSOR_THERMOMETER,     //!< Thermometer
#endif
#ifdef FEATURE_SENSOR_TYPE_GYROSCOPE_ENABLED
    SENSOR_GYROSCOPE,       //!< Gyroscope
#endif
#ifdef FEATURE_SENSOR_TYPE_MAGNETOMETER_ENABLED
    SENSOR_MAGNETOMETER,    //!< Magnetometer
#endif
    SENSOR_COUNT
} sensor_id_t;



/**
 * Initialise sensor driver.
 *
 * @param[in] sensor_id: ID of sensor to initialise
 * @param[in] config: config used by initialisation function (specific to hardware), leave NULL to use default
 * @return @ref gos_result_t
 */
gos_result_t sensor_init(sensor_id_t sensor_id, const void *config);

/**
 * Invoke a new measurement for the sensor
 *
 * @param[in] sensor_id: ID of sensor to initialise
 * @return @ref gos_result_t
 */
gos_result_t sensor_start_measurement(sensor_id_t sensor_id);

/**
 * Poll sensor to see if new data is available
 *
 * @param[in] sensor_id: ID of sensor to initialise
 * @param[out] has_data: True if there is new data available
 * @return @ref gos_result_t
 */
gos_result_t sensor_has_new_data(sensor_id_t sensor_id, bool *has_data);

/**
 * Get latest data from sensor
 *
 * @param[in] sensor_id: ID of sensor to initialise
 * @param[out] data: Data from sensor (specific to sensor device)
 * @return @ref gos_result_t
 */
gos_result_t sensor_get_data(sensor_id_t sensor_id, void *data);

/**
 * @}
 */
