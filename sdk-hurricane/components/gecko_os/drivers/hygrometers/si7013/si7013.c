/***************************************************************************//**
 * @file
 * @brief Driver for the Si7013 Temperature / Humidity sensor
 * @version 5.5.0
 *******************************************************************************
 * # License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include <stddef.h>
#include "si7013.h"
#include "gos.h"

#include "stddef.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/** Si7013 Read Temperature Command */
#define SI7013_READ_TEMP       0xE0  /* Read previous T data from RH measurement
                                      * command*/
/** Si7013 Read RH Command */
#define SI7013_READ_RH         0xE5  /* Perform RH (and T) measurement. */
/** Si7013 Read RH (no hold) Command */
#define SI7013_READ_RH_NH      0xF5  /* Perform RH (and T) measurement in no hold mode. */
/** Si7013 Read Thermistor Command */
#define SI7013_READ_VIN        0xEE  /* Perform thermistor measurement. */
/** Si7013 Read ID */
#define SI7013_READ_ID1_1      0xFA
#define SI7013_READ_ID1_2      0x0F
#define SI7013_READ_ID2_1      0xFc
#define SI7013_READ_ID2_2      0xc9
/** Si7013 Read Firmware Revision */
#define SI7013_READ_FWREV_1    0x84
#define SI7013_READ_FWREV_2    0xB8






/** @endcond */




/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
/**************************************************************************//**
 * @brief
 *  Reads data from the Si7013 sensor.
 * @param[in] i2c
 *   The I2C peripheral to use (not used).
 * @param[in] addr
 *   The I2C address of the sensor.
 * @param[out] data
 *   The data read from the sensor.
 * @param[in] command
 *   The command to send to device. See the \#define's for details.
 * @return
 *   Returns number of bytes read on success. Otherwise returns error codes
 *   based on the I2CDRV.
 *****************************************************************************/
static int32_t Si7013_Measure(const gos_i2c_device_t *i2c_device, uint8_t addr, uint32_t *data,
                              uint8_t command)
{
  gos_result_t ret;
  uint8_t i2c_read_data[2];
  uint8_t i2c_write_data[1]; 
  gos_i2c_message_t messages[2];
  
  i2c_write_data[0] = command;
  messages[0].buffer = i2c_write_data;
  messages[0].length = 1;
  messages[0].flags = GOS_I2C_MSG_START_SEQ | GOS_I2C_MSG_WRITE;
  messages[1].buffer = i2c_read_data;
  messages[1].length = 2;
  messages[1].flags = GOS_I2C_MSG_STOP_SEQ | GOS_I2C_MSG_READ;

  ret = gos_i2c_transfer(i2c_device, messages, 2);
  if(ret != GOS_SUCCESS)
  { *data = 0;
    return (int32_t) ret;
  }
  else
  { *data = ((uint32_t) i2c_read_data[0] << 8) + (i2c_read_data[1] & 0xfc);

  }


  return((int) 2);
}
/** @endcond */

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
/**************************************************************************//**
 * @brief
 *  Starts a no hold measurement in the Si7013 sensor.
 * @param[in] i2c
 *   The I2C peripheral to use (not used).
 * @param[in] addr
 *   The I2C address of the sensor.
 * @param[in] command
 *   The command to send to device. See the \#define's for details.
 * @return
 *   Returns number of bytes read on success. Otherwise returns error codes
 *   based on the I2CDRV.
 *****************************************************************************/
static int32_t Si7013_StartNoHoldMeasure(const gos_i2c_device_t *i2c_device, uint8_t addr, uint8_t command)

{
  gos_result_t ret;
  uint8_t i2c_write_data[1];
  gos_i2c_message_t messages[1];

  i2c_write_data[0] = command;
  messages[0].buffer = i2c_write_data;
  messages[0].length = 1;
  messages[0].flags = GOS_I2C_MSG_START_SEQ | GOS_I2C_MSG_STOP_SEQ | GOS_I2C_MSG_WRITE;


  ret = gos_i2c_transfer(i2c_device, messages, 1);
  if(ret != GOS_SUCCESS)
  { return (int32_t) ret;
  }
  else
  { return((int32_t) 0);
  }

}
/** @endcond */

/**************************************************************************//**
 * @brief
 *  Reads Firmware Revision from a Si7013 sensor.
 * @param[in] i2c
 *   The I2C peripheral to use.
 * @param[in] addr
 *   The I2C address of the sensor.
 * @param[out] fwRev
 *   The internal firmware revision. 0xFF === 1.0
 * @return
 *   Returns zero on OK, non-zero otherwise.
 *****************************************************************************/
int32_t Si7013_GetFirmwareRevision(const gos_i2c_device_t *i2c_device, uint8_t addr, uint8_t *fwRev)
{
    uint8_t i2c_write_data[2] = { SI7013_READ_FWREV_1, SI7013_READ_FWREV_2 };
    uint8_t i2c_read_data[1];
    gos_i2c_message_t messages[2];
    
    messages[0].buffer = i2c_write_data;
    messages[0].length = 2;
    messages[0].flags = GOS_I2C_MSG_START_SEQ | GOS_I2C_MSG_WRITE;
    messages[1].buffer = i2c_read_data;
    messages[1].length = 1;
    messages[1].flags = GOS_I2C_MSG_STOP_SEQ | GOS_I2C_MSG_READ;

    if(gos_i2c_transfer(i2c_device, messages, 2) != GOS_SUCCESS)
    { *fwRev = 0;
      return false;
    }
    
    *fwRev = i2c_read_data[0];
    return 0;
}

/**************************************************************************//**
 * @brief
 * Starts no hold measurement of relative humidity and temperature from a Si7013 sensor.
 * @param[in] i2c
 *   The I2C peripheral to use.
 * @param[in] addr
 *   The I2C address of the sensor.
 * @return
 *   Returns zero on OK, non-zero otherwise.
 *****************************************************************************/
int32_t Si7013_StartNoHoldMeasureRHAndTemp(const gos_i2c_device_t *i2c_device, uint8_t addr)
{
  int ret = Si7013_StartNoHoldMeasure(i2c_device, addr, SI7013_READ_RH_NH);

  return ret;
}

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
/**************************************************************************//**
 * @brief
 *  Reads data from the Si7013 sensor.
 * @param[in] i2c
 *   The I2C peripheral to use (not used).
 * @param[in] addr
 *   The I2C address of the sensor.
 * @param[out] data
 *   The data read from the sensor.
 * @return
 *   Returns number of bytes read on success. Otherwise returns error codes
 *   based on the I2CDRV.
 *****************************************************************************/
static int32_t Si7013_ReadNoHoldData(const gos_i2c_device_t *i2c_device, uint8_t addr, uint32_t *data)
{

  gos_result_t ret;
  uint8_t i2c_read_data[2];
  gos_i2c_message_t messages[2];

  messages[0].buffer = i2c_read_data;
  messages[0].length = 2;
  messages[0].flags = GOS_I2C_MSG_START_SEQ | GOS_I2C_MSG_READ;

  messages[1].buffer = i2c_read_data;
  messages[1].length = 2;
  messages[1].flags = GOS_I2C_MSG_STOP_SEQ | GOS_I2C_MSG_READ;


  ret = gos_i2c_transfer(i2c_device, messages, 2);
  if(ret != GOS_SUCCESS)
  { *data = 0;
    return (int32_t) ret;
  }
  else
  {  *data = ((uint32_t) i2c_read_data[0] << 8) + (i2c_read_data[1] & 0xfc);
  }

  return((int) 2);
}
/** @endcond */

/**************************************************************************//**
 * @brief
 *  Reads relative humidity and temperature from a Si7013 sensor.
 * @param[in] i2c
 *   The I2C peripheral to use.
 * @param[in] addr
 *   The I2C address of the sensor.
 * @param[out] rhData
 *   The relative humidity in percent (multiplied by 1000).
 * @param[out] tData
 *   The temperature in milli-Celsius.
 * @return
 *   Returns zero on OK, non-zero otherwise.
 *****************************************************************************/
int32_t Si7013_ReadNoHoldRHAndTemp(const gos_i2c_device_t *i2c_device, uint8_t addr, uint32_t *rhData,
                                   int32_t *tData)
{
  int ret = Si7013_ReadNoHoldData(i2c_device, addr, rhData);

  if (ret == 2) {
    /* convert to milli-percent */
    *rhData = (((*rhData) * 15625L) >> 13) - 6000;
  } else {
    return -1;
  }

  ret = Si7013_Measure(i2c_device, addr, (uint32_t *) tData, SI7013_READ_TEMP);

  if (ret == 2) {
    *tData = (((*tData) * 21965L) >> 13) - 46850; /* convert to milli-degC */
  } else {
    return -1;
  }

  return 0;
}

/**************************************************************************//**
 * @brief
 *  Reads relative humidity and temperature from a Si7013 sensor.
 * @param[in] i2c
 *   The I2C peripheral to use.
 * @param[in] addr
 *   The I2C address of the sensor.
 * @param[out] rhData
 *   The relative humidity in percent (multiplied by 1000).
 * @param[out] tData
 *   The temperature in milli-Celsius.
 * @return
 *   Returns zero on OK, non-zero otherwise.
 *****************************************************************************/
int32_t Si7013_MeasureRHAndTemp(const gos_i2c_device_t *i2c_device, uint8_t addr, uint32_t *rhData,
                                int32_t *tData)
{
  int ret = Si7013_Measure(i2c_device, addr, rhData, SI7013_READ_RH);

  if (ret == 2) {
    /* convert to milli-percent */
    *rhData = (((*rhData) * 15625L) >> 13) - 6000;
  } else {
    return -1;
  }

  ret = Si7013_Measure(i2c_device, addr, (uint32_t *) tData, SI7013_READ_TEMP);

  if (ret == 2) {
    *tData = (((*tData) * 21965L) >> 13) - 46850; /* convert to milli-degC */
  } else {
    return -1;
  }

  return 0;
}

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
static int32_t Si7013_WriteUserReg2(const gos_i2c_device_t *i2c_device, uint8_t addr, int8_t data)
{
  gos_result_t ret;
  uint8_t i2c_write_data[2];
  gos_i2c_message_t messages[1];

  i2c_write_data[0] = 0x50;
  i2c_write_data[1] = data;
  messages[0].buffer = i2c_write_data;
  messages[0].length = 2;
  messages[0].flags = GOS_I2C_MSG_START_SEQ | GOS_I2C_MSG_STOP_SEQ | GOS_I2C_MSG_WRITE;

  ret = gos_i2c_transfer(i2c_device, messages, 1);
  if(ret != GOS_SUCCESS)
  { return (int32_t) ret;
  }


  return((int) 0);
}
/** @endcond */

/**************************************************************************//**
 * @brief
 *  Reads relative humidity and temperature from a Si7013 sensor.
 * @param[in] i2c
 *   The I2C peripheral to use.
 * @param[in] addr
 *   The I2C address of the sensor.
 * @param[out] vData
 *   The data read from the sensor.
 * @return
 *   Returns zero on OK, non-zero otherwise.
 *****************************************************************************/
int32_t Si7013_MeasureV(const gos_i2c_device_t *i2c_device, uint8_t addr, int32_t *vData)
{
  int ret;
  Si7013_WriteUserReg2(i2c_device, addr, 0x0e);
  ret = Si7013_Measure(i2c_device, addr, (uint32_t *) vData, SI7013_READ_VIN);
  Si7013_WriteUserReg2(i2c_device, addr, 0x09);
  if (ret == 2) {
    /* convert  */
  } else {
    return -1;
  }
  return 0;
}

/**************************************************************************//**
 * @brief
 *   Checks if a Si7013 is present on the I2C bus or not.
 * @param[in] i2c
 *   The I2C peripheral to use.
 * @param[in] addr
 *   The I2C address to probe.   (not used in GOS)
 * @param[out] deviceId
 *   Write device ID from SNB_3 if device responds. Pass in NULL to discard.
 *   Should be 0x0D for Si7013, 0x14 for Si7020 or 0x15 for Si7021
 * @return
 *   True if a Si70xx is detected, false otherwise.
 *****************************************************************************/
bool Si7013_Detect(const gos_i2c_device_t *i2c_device, uint8_t addr, uint8_t *deviceId)
{

    // Write command bytes
    // For Si7021 this consists of 2 bytes address for electronic id register b
    // Followed by the upper two bytes of register b plus an 8 bit CRC
    // Followed by the lower two bytes of register b plus an 8 bit CRC
    uint8_t i2c_write_data[2] = { SI7013_READ_ID2_1, SI7013_READ_ID2_2 };
    uint8_t i2c_read_data[8];
    gos_i2c_message_t messages[2];
    
    messages[0].buffer = i2c_write_data;
    messages[0].length = 2;
    messages[0].flags = GOS_I2C_MSG_START_SEQ | GOS_I2C_MSG_WRITE;
    messages[1].buffer = i2c_read_data;
    messages[1].length = 8;
    messages[1].flags = GOS_I2C_MSG_STOP_SEQ | GOS_I2C_MSG_READ;

    if(gos_i2c_transfer(i2c_device, messages, 2) != GOS_SUCCESS)
    {
        return false;
    }
    else
    { if (deviceId != NULL) 
        *deviceId = i2c_read_data[0];  // Top byte of register B stores device identification byte
      return true;
    }

}
