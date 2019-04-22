/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */


#include "pca9535.h"



static gos_result_t pca9535_read_reg(const gos_i2c_device_t *i2c_device, uint8_t regaddr, uint32_t *value_ptr);
static gos_result_t pca9535_write_reg(const gos_i2c_device_t *i2c_device, uint8_t regaddr, uint16_t data);





/*************************************************************************************************/
gos_result_t pca9535_read_input(const gos_i2c_device_t *i2c_device, uint32_t *value_ptr)
{
    return pca9535_read_reg(i2c_device, 0, value_ptr);
}

/*************************************************************************************************/
gos_result_t pca9535_write_output(const gos_i2c_device_t *i2c_device, uint16_t param)
{
   return  pca9535_write_reg(i2c_device, 2, param);
}

/*************************************************************************************************/
gos_result_t pca9535_set_dir(const gos_i2c_device_t *i2c_device, uint16_t param)
{
    return pca9535_write_reg(i2c_device, 6, param);
}


/** --------------------------------------------------------------------------------------------
 *  Internal functions
 * -------------------------------------------------------------------------------------------- **/


/*************************************************************************************************/
static gos_result_t pca9535_read_reg(const gos_i2c_device_t *i2c_device, uint8_t regaddr, uint32_t *value_ptr)
{
    gos_result_t result;
    uint8_t i2c_read_data[3];
    uint8_t i2c_write_data[1];
    gos_i2c_message_t messages[2];

    i2c_write_data[0] = regaddr;
    messages[0].buffer = i2c_write_data;
    messages[0].length = 1;
    messages[0].flags = GOS_I2C_MSG_START_SEQ | GOS_I2C_MSG_WRITE;
    messages[1].buffer = i2c_read_data;
    messages[1].length = 3;
    messages[1].flags = GOS_I2C_MSG_STOP_SEQ | GOS_I2C_MSG_READ;


    if(GOS_FAILED(result, gos_i2c_transfer(i2c_device, messages, 2)))
    {
        *value_ptr = UINT32_MAX;
    }
    else
    {
        uint32_t data;

        data = i2c_read_data[1];
        data |= i2c_read_data[2] << 8;

        *value_ptr = data;
    }

    return result;
}

/*************************************************************************************************/
static gos_result_t pca9535_write_reg(const gos_i2c_device_t *i2c_device, uint8_t regaddr, uint16_t data)
{
    uint8_t i2c_write_data[3];
    gos_i2c_message_t messages[1];

    i2c_write_data[0] = regaddr;
    i2c_write_data[1] = data & 0xff;
    i2c_write_data[2] = data >> 8;
    messages[0].buffer = i2c_write_data;
    messages[0].length = 3;
    messages[0].flags = GOS_I2C_MSG_START_SEQ | GOS_I2C_MSG_STOP_SEQ | GOS_I2C_MSG_WRITE;


    return gos_i2c_transfer(i2c_device, messages, 1);
}
