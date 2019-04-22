/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */


#include "../../accelerometers/icm20648/icm20648.h"

#include "gos.h"



static gos_result_t set_bank(icm20648_device_t *device, uint8_t bank);
static gos_result_t read_reg(icm20648_device_t *device, uint8_t bank, uint8_t reg, uint8_t *value_ptr);
static gos_result_t write_reg(icm20648_device_t *device, uint8_t bank, uint8_t reg, uint8_t data);




/*************************************************************************************************/
gos_result_t icm20648_init(icm20648_device_t *device)
{
    gos_result_t result;

    device->current_bank = 0xff;

    if(GOS_FAILED(result, set_bank(device, 0)))
    {

    }
    // take chip out of sleep mode and set to best available clock source
    else if(GOS_FAILED(result, write_reg(device, 0, ICM20648_REG_PWR_MGMT_1, 0x01)))
    {

    }

    return result;
}


/*************************************************************************************************/
gos_result_t icm20648_read_id(icm20648_device_t *device, uint8_t *id_ptr)
{
    gos_result_t result;

    if(GOS_FAILED(result, read_reg(device, 0, ICM20648_REG_WHOAMI, id_ptr)))
    {

    }

    return result;
}


/*************************************************************************************************/
gos_result_t icm20648_read_accel(icm20648_device_t *device, float *x_ptr, float *y_ptr, float *z_ptr)
{
    gos_result_t result;
    uint8_t i2c_read_data[6];

    if(GOS_FAILED(result, set_bank(device, 0)))
    {
    }
    else if(GOS_FAILED(result, gos_i2c_master_read_reg(device->i2c_device,
                                                       ICM20648_REG_ACCEL_XOUT_H,
                                                       i2c_read_data,
                                                       sizeof(i2c_read_data))))
    {
    }
    else
    {
        const int16_t temp_x = (i2c_read_data[0] << 8) | i2c_read_data[1];
        const int16_t temp_y = (i2c_read_data[2] << 8) | i2c_read_data[3];
        const int16_t temp_z = (i2c_read_data[4] << 8) | i2c_read_data[5];

        *x_ptr = (float)temp_x / 16384.0f;
        *y_ptr = (float)temp_y / 16384.0f;
        *z_ptr = (float)temp_z / 16384.0f;
    }

    return result;
}





/** --------------------------------------------------------------------------------------------
 *  Internal functions
 * -------------------------------------------------------------------------------------------- **/





/*************************************************************************************************/
static gos_result_t set_bank(icm20648_device_t *device, uint8_t bank)
{
    gos_result_t result;
    const gos_i2c_message_t msg =
    {
        .buffer = &bank,
        .length = sizeof(bank),
        .flags = GOS_I2C_MSG_START_SEQ | GOS_I2C_MSG_WRITE | GOS_I2C_MSG_STOP_SEQ
    };

    if(bank == device->current_bank)
    {
        result = GOS_SUCCESS;
    }
    else if(GOS_FAILED(result,  gos_i2c_transfer(device->i2c_device, &msg, 1)))
    {
    }
    else
    {
        device->current_bank = bank;
    }

    return result;
}


/*************************************************************************************************/
static gos_result_t read_reg(icm20648_device_t *device, uint8_t bank, uint8_t reg, uint8_t *value_ptr)
{
    gos_result_t result;

    if(GOS_FAILED(result, set_bank(device, bank)))
    {
    }
    else if(GOS_FAILED(result, gos_i2c_master_read_reg8(device->i2c_device, reg, value_ptr)))
    {
    }

    return result;
}


/*************************************************************************************************/
static gos_result_t write_reg(icm20648_device_t *device, uint8_t bank, uint8_t reg, uint8_t data)
{
    gos_result_t result;

    if(GOS_FAILED(result, set_bank(device, bank)))
    {
    }
    else if(GOS_FAILED(result, gos_i2c_master_write_reg8(device->i2c_device, reg, data)))
    {
    }

    return result;
}


