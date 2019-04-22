/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */

#include "sensor.h"



/*************************************************************************************************/
gos_result_t sensor_thermometer_init(const thermometer_config_t *config)
{
    return sensor_hygrometer_init(config);
}

/*************************************************************************************************/
gos_result_t sensor_thermometer_has_new_data(bool *has_data)
{
    return sensor_hygrometer_has_new_data(has_data);
}

/*************************************************************************************************/
gos_result_t sensor_thermometer_start_measurement(void)
{
    return sensor_hygrometer_start_measurement();
}

/*************************************************************************************************/
gos_result_t sensor_thermometer_get_data(thermometer_data_t *data)
{
    return sensor_hygrometer_get_data(data);
}
