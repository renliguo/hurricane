/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) C
 * opyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */
#include "../../accelerometers/icm20648/icm20648.h"
#include "gos.h"

#include "sensor.h"


static gos_result_t initialize_sensor(const accelerometer_config_t *config);


static struct
{
    gos_i2c_device_t i2c;
    icm20648_device_t device;
    bool is_initialized;
} icm20648_context;



/*************************************************************************************************/
gos_result_t sensor_accelerometer_init(const accelerometer_config_t *config)
{
    gos_result_t result = GOS_ERROR;
    static const accelerometer_config_t default_config =
    {
        .port = PLATFORM_STD_I2C,
        .init_retries = 3
    };
    config = (config == NULL) ? &default_config : config;


    for(uint8_t i = config->init_retries; i > 0; --i)
    {
        if(GOS_FAILED(result, initialize_sensor(config)))
        {
            if(i > 1)
            {
                gos_rtos_delay_milliseconds(5);
            }
        }
        else
        {
            break;
        }
    }

    return result;
}

/*************************************************************************************************/
gos_result_t sensor_accelerometer_has_new_data(bool *has_data)
{
    gos_result_t result;

    if(!icm20648_context.is_initialized)
    {
        result = GOS_UNINITIALIZED;
    }
    else
    {
        *has_data = true;
        result = GOS_SUCCESS;
    }

    return result;
}

/*************************************************************************************************/
gos_result_t sensor_accelerometer_start_measurement(void)
{
    gos_result_t result;

    if(!icm20648_context.is_initialized)
    {
        result = GOS_UNINITIALIZED;
    }
    else
    {
        result = GOS_SUCCESS;
    }

    return result;
}

/*************************************************************************************************/
gos_result_t sensor_accelerometer_get_data(accelerometer_data_t *data)
{
    gos_result_t result;

    if(!icm20648_context.is_initialized)
    {
        result = GOS_UNINITIALIZED;
    }
    else if(GOS_FAILED(result, icm20648_read_accel(&icm20648_context.device, &data->x, &data->y, &data->z)))
    {
    }

    return result;
}


/** --------------------------------------------------------------------------------------------
 *  Internal functions
 * -------------------------------------------------------------------------------------------- **/


/*************************************************************************************************/
static gos_result_t initialize_sensor(const accelerometer_config_t *config)
{
    gos_result_t result;

    icm20648_context.i2c.port       = config->port;
    icm20648_context.i2c.address    = ICM20648_ADDR;
    icm20648_context.i2c.speed      = GOS_I2C_CLOCK_HIGH_SPEED;
    icm20648_context.i2c.read_timeout = 0;
    icm20648_context.i2c.retries    = 0;
    icm20648_context.i2c.flags      = 0;
    icm20648_context.device.i2c_device = &icm20648_context.i2c;

    if(GOS_FAILED(result, icm20648_init(&icm20648_context.device)))
    {
    }
    else
    {
        icm20648_context.is_initialized = true;
    }

    return result;
}


