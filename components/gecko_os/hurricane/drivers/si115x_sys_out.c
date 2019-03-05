//-----------------------------------------------------------------------------
// Si115x_sys_out.c
//-----------------------------------------------------------------------------
// Copyright 2018 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// File Description: This file provides low level functions such as writing to
//						and reading from registers
//
// MCU-specific routines to needed by Si115xDrv.c
//
// Target:         Silicon Labs EFM32
// Command Line:   None
//
//-----------------------------------------------------------------------------

#include <si115xdrv.h>
#include "gos.h"

/*  Critical Sections in this code are meant to prevent I2C or SPI collisions. If bus transactions occur
 *  inside an interrupt context then collisions can occur.  In this case, the code should disable any
 *  interrupts in which a collision may occur.  The "safe" thing to do is to ensure that bus transactions
 *  never occur in the interrupt context.  In this case critical sections are not needed.
 */
#define SI115X_USE_CRITICAL_SECTIONS  0

#if (SI115X_USE_CRITICAL_SECTIONS == 1)
	void Si115xEnterCriticalSection(void)
	{
	   /*  add code here to disable any interrupts that may impact this section
	    *  typically this is any interrupt in which an I2C or SPI transaction may occur
	    */

	}

	void Si115xExitCriticalSection(void)
	{
		   /*  add code here to re-enable any interrupts that may impact this section
		    *  typically this is any interrupt in which an I2C or SPI transaction may occur
		    *  Be careful not to accidently enable interrupts that were not previously disabled
		    */

	}

#else
  #define Si115xEnterCriticalSection()     //Do nothing
  #define Si115xExitCriticalSection()      //Do nothing
#endif


/**************************************************************************//**
 * @brief Write to Si115x i2c.
 *****************************************************************************/
int16_t Si115xWriteToRegister(HANDLE si115x_handle, uint8_t address, uint8_t data)
{
  gos_result_t ret;
  uint8_t i2c_write_data[2];
  gos_i2c_message_t messages[1];
  gos_i2c_device_t *i2c_device = (gos_i2c_device_t *)si115x_handle;

  i2c_write_data[0] = address | 0x40;
	i2c_write_data[1] = data;
  messages[0].buffer = i2c_write_data;
  messages[0].length = 2;
  messages[0].flags = GOS_I2C_MSG_START_SEQ | GOS_I2C_MSG_STOP_SEQ | GOS_I2C_MSG_WRITE;

	Si115xEnterCriticalSection();

  ret = gos_i2c_transfer(i2c_device, messages, 1);

  Si115xExitCriticalSection();

  if (ret != GOS_SUCCESS)
  {
    return (int16_t)ret;
  }
  return (int16_t)0;
}


/**************************************************************************//**
 * @brief Read from Si115x i2c.
 *****************************************************************************/
int16_t Si115xReadFromRegister(HANDLE si115x_handle, uint8_t address)
{
  gos_result_t ret;
  gos_i2c_device_t *i2c_device = (gos_i2c_device_t *)si115x_handle;
  uint8_t i2c_read_data[1];
  uint8_t i2c_write_data[1]; 
  gos_i2c_message_t messages[2];
  
  i2c_write_data[0] = address | 0x40;
  messages[0].buffer = i2c_write_data;
  messages[0].length = 1;
  messages[0].flags = GOS_I2C_MSG_START_SEQ | GOS_I2C_MSG_WRITE ;
  messages[1].buffer = i2c_read_data;
  messages[1].length = 1;
  messages[1].flags = GOS_I2C_MSG_STOP_SEQ | GOS_I2C_MSG_READ;

	Si115xEnterCriticalSection();

  ret = gos_i2c_transfer(i2c_device, messages, 2);

	Si115xExitCriticalSection();

  if(ret != GOS_SUCCESS)
	{
		ret = 0xff;
		return((int) ret);
	}
	return((int) i2c_read_data[0]);
}

/**************************************************************************//**
 * @brief Write block of data to Si117x i2c.
 *****************************************************************************/
int16_t Si115xBlockWrite(HANDLE si115x_handle, uint8_t address, uint8_t length, uint8_t * data)
{ int32_t i;
  gos_result_t ret;
  uint8_t i2c_write_data[10];
  gos_i2c_message_t messages[1];
  gos_i2c_device_t *i2c_device = (gos_i2c_device_t *)si115x_handle;

  i2c_write_data[0] = address;
  for (i=0; i<length;i++)
  {
    i2c_write_data[i+1] = data[i];
  }
  messages[0].buffer = i2c_write_data;
  messages[0].length = 1+length;
  messages[0].flags = GOS_I2C_MSG_START_SEQ | GOS_I2C_MSG_STOP_SEQ | GOS_I2C_MSG_WRITE;

	Si115xEnterCriticalSection();

  ret = gos_i2c_transfer(i2c_device, messages, 1);

  Si115xExitCriticalSection();
 
  if(ret != GOS_SUCCESS)
  {
    return((int) ret);
  }

  return((int) 0);
}

/**************************************************************************//**
 * @brief Read block of data from Si117x i2c.
 *****************************************************************************/
int16_t Si115xBlockRead(HANDLE si115x_handle, uint8_t address, uint8_t length, uint8_t* data)
{
  gos_result_t ret;
  gos_i2c_device_t *i2c_device = (gos_i2c_device_t *)si115x_handle;
  uint8_t i2c_write_data[1]; 
  gos_i2c_message_t messages[2];
  
  i2c_write_data[0] = address;
  messages[0].buffer = i2c_write_data;
  messages[0].length = 1;
  messages[0].flags = GOS_I2C_MSG_START_SEQ | GOS_I2C_MSG_WRITE ;
  messages[1].buffer = data;
  messages[1].length = length;
  messages[1].flags = GOS_I2C_MSG_STOP_SEQ | GOS_I2C_MSG_READ;

	Si115xEnterCriticalSection();

  ret = gos_i2c_transfer(i2c_device, messages, 2);

	Si115xExitCriticalSection();

  if(ret != GOS_SUCCESS)
  {
    return((int) ret);
  }

  return((int) 0);
}


/**************************************************************************//**
 * @brief Hardware implemented delay function. Does not need to be accurate.
 *****************************************************************************/
void delay_ms(int ms) 
{
  
  gos_rtos_delay_milliseconds(ms);
  
}

/**************************************************************************//**
 * @brief 10ms delay required by Si115x reset sequence.
 *****************************************************************************/
void Si115xDelay_10ms(void)
{
  gos_rtos_delay_milliseconds(10);
}
