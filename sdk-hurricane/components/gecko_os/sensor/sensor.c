/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */
#include "sensor.h"

/**
 * @brief Register a sensor type with the sensor library
 * @def SENSOR_TYPE_REGISTER_SENSOR_TYPE(name, id)
 */
#define REGISTER_SENSOR_TYPE(name, id) \
[SENSOR_ ## id] = \
{ \
    .init     = (sensor_init_prototype_t)    sensor_ ## name ## _init, \
    .has_data = (sensor_has_data_prototype_t)sensor_ ## name ## _has_new_data, \
    .get_data = (sensor_get_data_prototype_t)sensor_ ## name ## _get_data, \
    .start_measurement = (sensor_start_measurement_prototype_t)sensor_ ## name ## _start_measurement \
}



/* Each low level sensor driver must implement the following functions */
typedef gos_result_t (*sensor_init_prototype_t)(const void *config);
typedef gos_result_t (*sensor_has_data_prototype_t)(bool *has_data);
typedef gos_result_t (*sensor_get_data_prototype_t)(void *data);
typedef gos_result_t (*sensor_start_measurement_prototype_t)(void);



static const struct
{
    const sensor_init_prototype_t init;
    const sensor_has_data_prototype_t has_data;
    const sensor_get_data_prototype_t get_data;
    sensor_start_measurement_prototype_t start_measurement;
}
sensor_functions[] =
{
#ifdef FEATURE_SENSOR_TYPE_ACCELEROMETER_ENABLED
        REGISTER_SENSOR_TYPE(accelerometer, ACCELEROMETER),
#endif
#ifdef FEATURE_SENSOR_TYPE_HYGROMETER_ENABLED
        REGISTER_SENSOR_TYPE(hygrometer, HYGROMETER),
#endif
#ifdef FEATURE_SENSOR_TYPE_THERMOMETER_ENABLED
        REGISTER_SENSOR_TYPE(thermometer, THERMOMETER),
#endif
#ifdef FEATURE_SENSOR_TYPE_GYROSCOPE_ENABLED
        REGISTER_SENSOR_TYPE(gyroscope, GYROSCOPE),
#endif
#ifdef FEATURE_SENSOR_TYPE_MAGNETOMETER_ENABLED
        REGISTER_SENSOR_TYPE(magnetometer, MAGNETOMETER),
#endif
};


/* Input sensor and config */
gos_result_t sensor_init(sensor_id_t sensor_id, const void *config)
{
    if(sensor_id >= SENSOR_COUNT)
    {
        return GOS_INVALID_ARG;
    }

    return sensor_functions[sensor_id].init(config);
}

/*************************************************************************************************/
gos_result_t sensor_start_measurement(sensor_id_t sensor_id)
{
    if(sensor_id >= SENSOR_COUNT)
    {
        return GOS_INVALID_ARG;
    }

    return sensor_functions[sensor_id].start_measurement();
}

/*************************************************************************************************/
/* Input sensor, output true if new data available*/
gos_result_t sensor_has_new_data(sensor_id_t sensor_id, bool *has_data)
{
    *has_data = false;

    if(sensor_id >= SENSOR_COUNT)
    {
        return GOS_INVALID_ARG;
    }

    return sensor_functions[sensor_id].has_data(has_data);
}



/*************************************************************************************************/
/* Input sensor, output data */
gos_result_t sensor_get_data(sensor_id_t sensor_id, void *data)
{
    if(sensor_id >= SENSOR_COUNT)
    {
        return GOS_INVALID_ARG;
    }

    return sensor_functions[sensor_id].get_data(data);
}


