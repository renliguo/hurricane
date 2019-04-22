/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */

#include "gos.h"

#include "si7013.h"
#include "sensor.h"


#define MEASURE_DELAY_MS 35



static gos_result_t initialize_sensor(const hygrometer_config_t *config);
static bool         data_is_ready(void);


static struct
{
    gos_i2c_device_t i2c;
    bool block_while_measure;
    uint32_t measure_timestamp;
    bool is_measuring;
    bool is_initialized;
} si7013_context;




/*************************************************************************************************/
gos_result_t sensor_hygrometer_init(const hygrometer_config_t *config)
{
    gos_result_t result = GOS_SUCCESS;
    static const hygrometer_config_t default_config =
    {
        .port                   = PLATFORM_STD_I2C,
        .print_init_details     = false,
        .block_while_measure    = true,
        .init_retries           = 3
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
gos_result_t sensor_hygrometer_has_new_data(bool *has_data)
{
    if(!si7013_context.is_initialized)
    {
        return GOS_UNINITIALIZED;
    }


    // If we block while measuring then data will always be available
    if(si7013_context.block_while_measure)
    {
        *has_data = true;
    }
    // Else if we're actively measuring AND
    // enough time has passed then data is available
    else if(data_is_ready())
    {
        *has_data = true;
    }
    else
    {
        *has_data = false;
    }

    return GOS_SUCCESS;
}

/*************************************************************************************************/
gos_result_t sensor_hygrometer_start_measurement(void)
{
    gos_result_t result;

    if(!si7013_context.is_initialized)
    {
        return GOS_UNINITIALIZED;
    }

    if(GOS_FAILED(result, (gos_result_t)Si7013_StartNoHoldMeasureRHAndTemp(&si7013_context.i2c, 0)))
    {

    }
    else
    {
        si7013_context.is_measuring = true;
        si7013_context.measure_timestamp = gos_rtos_get_time();
    }

    return result;
}

/*************************************************************************************************/
gos_result_t sensor_hygrometer_get_data(hygrometer_data_t *data)
{
    gos_result_t result;
    uint32_t rhData;
    int32_t tData;

    if(!si7013_context.is_initialized)
    {
        result = GOS_UNINITIALIZED;
    }
    else if(si7013_context.block_while_measure)
    {
        result = (gos_result_t)Si7013_MeasureRHAndTemp(&si7013_context.i2c, 0, &rhData, &tData);
        si7013_context.is_measuring = false;
    }
    else if(!data_is_ready())
    {
        result = GOS_PENDING;
    }
    else
    {
        result = (gos_result_t)Si7013_ReadNoHoldRHAndTemp(&si7013_context.i2c, 0, &rhData, &tData);
        si7013_context.is_measuring = false;
    }


    if(result == GOS_SUCCESS)
    {
        data->humidity = rhData / 1000.0f;
        data->temperature = tData / 1000.0f;
    }

    return result;
}


/** --------------------------------------------------------------------------------------------
 *  Internal functions
 * -------------------------------------------------------------------------------------------- **/


/*************************************************************************************************/
static gos_result_t initialize_sensor(const hygrometer_config_t *config)
{
    gos_result_t result;
    uint8_t deivce_id;

    si7013_context.i2c.port       = config->port;
    si7013_context.i2c.address    = SI7021_ADDR >> 1;
    si7013_context.i2c.speed      = GOS_I2C_CLOCK_HIGH_SPEED;
    si7013_context.i2c.read_timeout = 50;     //si7021 can take up to 25ms to finish a conversion
    si7013_context.i2c.retries    = 0;
    si7013_context.i2c.flags      = 0;


    if(!Si7013_Detect(&si7013_context.i2c, 0, &deivce_id))
    {
        result = GOS_UNINITIALIZED;
    }
    else
    {
        if(config->print_init_details)
        {
            uint8_t fw_version;

            Si7013_GetFirmwareRevision(&si7013_context.i2c, 0, &fw_version);

            switch(deivce_id)
            {
            case 0x0D: GOS_LOG("Si7013 detected, ver: 0x02X", fw_version); break;
            case 0x14: GOS_LOG("Si7020 detected, ver: 0x02X", fw_version); break;
            case 0x15: GOS_LOG("Si7021 detected, ver: 0x02X", fw_version); break;
            default:   GOS_LOG("Unknown hygrometer/thermometer detected");
            }
        }

        si7013_context.is_initialized = true;
        si7013_context.block_while_measure = config->block_while_measure;
        result = GOS_SUCCESS;
    }

    return result;
}


/*************************************************************************************************/
static bool data_is_ready(void)
{
    return (si7013_context.is_measuring &&
           ((gos_rtos_get_time() - si7013_context.measure_timestamp) > MEASURE_DELAY_MS));
}
