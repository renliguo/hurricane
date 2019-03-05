/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */


#include "gos.h"

#include "icm20648.h"

static void set_bank(icm20648_device_t *device, uint8_t bank)
{
	if(bank == device->current_bank)
	{
		return;
	}

	gos_i2c_message_t msg;
	msg.buffer = &bank;
	msg.length = sizeof(bank);
	msg.flags = GOS_I2C_MSG_START_SEQ | GOS_I2C_MSG_WRITE | GOS_I2C_MSG_STOP_SEQ;

	gos_i2c_transfer(&device->i2c_device, &msg, 1);

	device->current_bank = bank;
}

static uint8_t read_reg(icm20648_device_t *device, uint8_t bank, uint8_t reg)
{
	gos_result_t ret;
	uint8_t i2c_read_data[1];
	
	set_bank(device, bank);

	ret = gos_i2c_master_read_reg8(&device->i2c_device, reg, i2c_read_data);

	if(ret != GOS_SUCCESS)
	{ 
		return 0;
	}
	
	return i2c_read_data[0];
}

static void write_reg(icm20648_device_t *device, uint8_t bank, uint8_t reg, uint8_t data)
{
	set_bank(device, bank);

	gos_i2c_master_write_reg8(&device->i2c_device, reg, data);
}

// static uint16_t read_reg16(uint8_t bank, uint8_t reg)
// {
// 	gos_result_t ret;
// 	uint8_t i2c_read_data[2];
	
// 	set_bank(bank);

// 	ret = gos_i2c_master_read_reg(&i2c_device, reg, i2c_read_data, sizeof(i2c_read_data));

// 	if(ret != GOS_SUCCESS)
// 	{ 
// 		return 0;
// 	}

// 	return (i2c_read_data[0] << 8) | i2c_read_data[1];
// }

void icm20648_init(icm20648_device_t *device)
{	
	device->current_bank = 0xff;
	set_bank(device, 0);

	// take chip out of sleep mode and set to best available clock source
	write_reg(device, 0, ICM20648_REG_PWR_MGMT_1, 0x01);
}	

uint8_t icm20648_read_id(icm20648_device_t *device)
{
	return read_reg(device, 0, ICM20648_REG_WHOAMI);
}

gos_result_t icm20648_read_accel(icm20648_device_t *device, float *x, float *y, float *z)
{
	gos_result_t ret;
	uint8_t i2c_read_data[6];

	set_bank(device, 0);

	ret = gos_i2c_master_read_reg(&device->i2c_device, ICM20648_REG_ACCEL_XOUT_H, i2c_read_data, sizeof(i2c_read_data));
	
	if(ret != GOS_SUCCESS)
	{ 
		return ret;
	}		

	int16_t temp_x = (i2c_read_data[0] << 8) | i2c_read_data[1];
	int16_t temp_y = (i2c_read_data[2] << 8) | i2c_read_data[3];
	int16_t temp_z = (i2c_read_data[4] << 8) | i2c_read_data[5];

	*x = (float)temp_x / 16384.0;
	*y = (float)temp_y / 16384.0;
	*z = (float)temp_z / 16384.0;
	
	return ret;
}
