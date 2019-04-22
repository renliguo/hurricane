/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */

#include "gos.h"
#include "hal_gos_i2c.h"

#include "atca_hal.h"

#define I2C_NORMAL_SPEED 	GOS_I2C_CLOCK_HIGH_SPEED
#define I2C_WAKE_SPEED 		GOS_I2C_CLOCK_STANDARD_SPEED

static gos_i2c_device_t atca_i2c_device =
{
    .port    = PLATFORM_STD_I2C,
    .speed   = I2C_NORMAL_SPEED,
    .address = 0,
    .retries = 0,
    .flags   = 0
};

ATCA_STATUS hal_i2c_init(void *hal, ATCAIfaceCfg *cfg)
{	
	atca_i2c_device.address = cfg->atcai2c.slave_address >> 1;

	return ATCA_SUCCESS;
}

ATCA_STATUS hal_i2c_post_init(ATCAIface iface)
{
	return ATCA_SUCCESS;
}

ATCA_STATUS hal_i2c_send(ATCAIface iface, uint8_t *txdata, int txlength)
{
	txdata[0] = 0x03;   // set word address = command
    txlength++;

	gos_i2c_message_t msg;
	msg.buffer = txdata;
	msg.length = txlength;
	msg.flags = GOS_I2C_MSG_START_SEQ | GOS_I2C_MSG_WRITE | GOS_I2C_MSG_STOP_SEQ;

	if(gos_i2c_transfer(&atca_i2c_device, &msg, 1) != GOS_SUCCESS)
	{
		return ATCA_COMM_FAIL;		
	}

	return ATCA_SUCCESS;
}

ATCA_STATUS hal_i2c_receive(ATCAIface iface, uint8_t *rxdata, uint16_t *rxlength)
{
	gos_i2c_message_t msg;
	msg.buffer = rxdata;
	msg.length = *rxlength;
	msg.flags = GOS_I2C_MSG_START_SEQ | GOS_I2C_MSG_READ | GOS_I2C_MSG_STOP_SEQ;

	*rxlength = 0;

	if(gos_i2c_transfer(&atca_i2c_device, &msg, 1) != GOS_SUCCESS)
	{
		return ATCA_COMM_FAIL;		
	}

	*rxlength = msg.length;

	return ATCA_SUCCESS;
}

ATCA_STATUS hal_i2c_wake(ATCAIface iface)
{
	ATCAIfaceCfg *cfg = atgetifacecfg(iface);
	uint8_t data[4] = { 0x00, 0x00, 0x00, 0x00 };
    uint16_t data_size = sizeof(data);
    uint8_t address = atca_i2c_device.address;
    int status;
    gos_i2c_message_t msg;
    

	// change I2C speed so we get at least 80 microseconds of 0 time on SDA to wake up the device
	atca_i2c_device.speed = I2C_WAKE_SPEED;
			
	// change address to 0, so we clock out only 0s.
	atca_i2c_device.address = 0;

	msg.buffer = 0;
	msg.length = 0;
	msg.flags = GOS_I2C_MSG_START_SEQ | GOS_I2C_MSG_WRITE | GOS_I2C_MSG_STOP_SEQ;

	// send 0s
	gos_i2c_transfer(&atca_i2c_device, &msg, 1);

	// change speed back to normal
	atca_i2c_device.speed = I2C_NORMAL_SPEED;

	// restore address
	atca_i2c_device.address = address;

	// Wait tWHI + tWLO
    gos_rtos_delay_milliseconds((cfg->wake_delay + 500) /1000);    //+500 is to round up to the nearest milliseconds
//    gos_wait_microseconds(cfg->wake_delay);

	// Receive Wake Response
    status = hal_i2c_receive(iface, data, &data_size);
    if (status != ATCA_SUCCESS)
    {
        return ATCA_COMM_FAIL;
    }


    return hal_check_wake(data, data_size);
}

ATCA_STATUS hal_i2c_idle(ATCAIface iface)
{
    gos_i2c_message_t msg;

    // idle command
    uint8_t cmd = 0x02;

	msg.buffer = &cmd;
	msg.length = sizeof(cmd);
	msg.flags = GOS_I2C_MSG_START_SEQ | GOS_I2C_MSG_WRITE | GOS_I2C_MSG_STOP_SEQ;

	if(gos_i2c_transfer(&atca_i2c_device, &msg, 1) != GOS_SUCCESS)
	{
		return ATCA_TX_TIMEOUT;
	}

	return ATCA_SUCCESS;
}

ATCA_STATUS hal_i2c_sleep(ATCAIface iface)
{
    gos_i2c_message_t msg;

    // sleep command
    uint8_t cmd = 0x01;

	msg.buffer = &cmd;
	msg.length = sizeof(cmd);
	msg.flags = GOS_I2C_MSG_START_SEQ | GOS_I2C_MSG_WRITE | GOS_I2C_MSG_STOP_SEQ;

	if(gos_i2c_transfer(&atca_i2c_device, &msg, 1) != GOS_SUCCESS)
	{
		return ATCA_TX_TIMEOUT;
	}

	return ATCA_SUCCESS;
}

ATCA_STATUS hal_i2c_release(void *hal_data)
{
	return ATCA_SUCCESS;
}

ATCA_STATUS hal_i2c_discover_buses(int i2c_buses[], int max_buses)
{
	i2c_buses[0] = 0;

	return ATCA_SUCCESS;
}

ATCA_STATUS hal_i2c_discover_devices(int bus_num, ATCAIfaceCfg *cfg, int *found)
{
	return ATCA_SUCCESS;
}

