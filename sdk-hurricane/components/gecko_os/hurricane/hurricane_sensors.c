/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */

#include "gos.h"
#include "hurricane.h"

// include the Microchip crypto lib for the secure element
#include "atca_basic.h"

#define MEASURE_PERIOD_MS                   500         // milliseconds
#define PASCAL_TO_INHG_CONVERSION_FACTOR    3386.389
#define SI7013_MEASUREMENT_TIME_MS          50  // the si7013 takes max 50ms to perform measurement


static gos_i2c_device_t si1133_uv_als_device =
{
    .port    = PLATFORM_STD_I2C,
    .address = SI1133_UVALS_ADDR,
    .speed   = GOS_I2C_CLOCK_HIGH_SPEED,
    .retries = 0,
    .flags   = 0
};

static gos_i2c_device_t si1153_gesture_device =
{
    .port    = PLATFORM_STD_I2C,
    .address = SI1153_PROX_GEST_ADDR,
    .speed   = GOS_I2C_CLOCK_HIGH_SPEED,
    .retries = 0,
    .flags   = 0
};

static gos_i2c_device_t si1153_long_range_device =
{
    .port    = PLATFORM_STD_I2C,
    .address = SI1153_LONG_RANGE_PROX_ADDR,
    .speed   = GOS_I2C_CLOCK_HIGH_SPEED,
    .retries = 0,
    .flags   = 0
};

static gos_i2c_device_t si7021_temp_rh_device =
{
    .port    = PLATFORM_STD_I2C,
    .address = SI7021_ADDR >> 1,
    .speed   = GOS_I2C_CLOCK_HIGH_SPEED,
    .read_timeout = 50,     //si7021 can take up to 25ms to finish a conversion
    .retries = 0,
    .flags   = 0
};

static gos_i2c_device_t si7200_hall_effect_device =
{
    .port    = PLATFORM_STD_I2C,
    .address = SI7200_ADDR_0 >> 1,
    .speed   = GOS_I2C_CLOCK_HIGH_SPEED,
    .retries = 0,
    .flags   = 0
};

static struct bmp280_dev bmp280 = 
{
  	.chip_id = 0,
	.dev_id = 0,
	.intf = BMP280_I2C_INTF,
	.read = bmp280_ReadRegister,
	.write = bmp280_WriteRegister,
	.delay_ms = bmp280_delay_ms,
};

const gos_i2c_device_t icm20648_i2c = {
        .port    = PLATFORM_STD_I2C,
        .address = ICM20648_ADDR,
        .speed   = GOS_I2C_CLOCK_HIGH_SPEED,
        .retries = 0,
        .flags   = 0
    };
    
static icm20648_device_t icm20648_device = 
{ .i2c_device = &icm20648_i2c,
  .current_bank = 0
   
};

static uint8_t measurement_time;

static float data_temperature;
static float data_RH;
static float data_pressure;
static float data_UV;
static float data_ALS;
static float data_gest_1;
static float data_gest_2;
static float data_prox;
static float data_mag;
static float data_accel_x;
static float data_accel_y;
static float data_accel_z;


static void read_measure_event_handler(void *arg)
{ 
    uint32_t rhData;
    int32_t tData;
    Si115x_GestureSample samples_gest;
    Si115xSample_t samples_lr;
    Si115xSample_t samples_uv_als;
    double pressure_pa;
    struct bmp280_uncomp_data ucomp_data;
    int16_t magData;

    // get temperature (in C) and RH
    Si7013_ReadNoHoldRHAndTemp(&si7021_temp_rh_device, 0, &rhData, &tData);

    data_temperature  = tData / 1000.0;
    data_RH           = rhData / 1000.0;


    Si115x_GestureHandler(&si1153_gesture_device, &samples_gest);
    data_gest_1 = samples_gest.ps2;
    data_gest_2 = samples_gest.ps3;

    Si115xHandler(&si1153_long_range_device, &samples_lr);
    data_prox = samples_lr.ch0;

    Si115xHandler(&si1133_uv_als_device, &samples_uv_als);

    // get UV/ALS data
    data_UV = Si1133_getUVReading(&si1133_uv_als_device, &samples_uv_als);
    data_ALS = Si1133_getLuxReading(&si1133_uv_als_device, &samples_uv_als);

    if(data_UV < 0.0)
    {
        data_UV = 0.0;
    }
    if(data_UV > 12)
    {
        data_UV = 12.0;
    }

    // read pressure sensor
    if(bmp280_get_uncomp_data(&ucomp_data, &bmp280) == BMP280_OK)
    {
        pressure_pa = bmp280_comp_pres_double(ucomp_data.uncomp_press, &bmp280);
        data_pressure = (float)pressure_pa / PASCAL_TO_INHG_CONVERSION_FACTOR;
    }
    else
    {
        data_pressure = 0.0;
    }

    // read hall effect
    if(Si72xx_ReadMagFieldDataAndSleep(&si7200_hall_effect_device, 0, SI7210_20MT, SI72XX_SLEEP_MODE, &magData) == 0)
    {
        data_mag = Si72xx_ConvertDataCodesToMagneticField(SI7210_20MT, magData);
    }

    // read accel
    icm20648_read_accel(&icm20648_device, &data_accel_x, &data_accel_y, &data_accel_z);
}

static void start_measure_event_handler(void *arg)
{
  bmp280_set_power_mode(BMP280_FORCED_MODE, &bmp280);
  Si115xForce(&si1153_gesture_device);
  Si115xForce(&si1153_long_range_device);
  Si115xForce(&si1133_uv_als_device);
  Si7013_StartNoHoldMeasureRHAndTemp(&si7021_temp_rh_device, 0);

  gos_event_register_timed(read_measure_event_handler, arg, measurement_time, 0);
}


void hurricane_sensors_init(void)
{
    // uint8_t fwRev;
    // uint8_t deviceId;
    struct bmp280_config pressure_sensor_config;

    // Si7013_Detect(&si7021_temp_rh_device, 0, &deviceId);

    // switch(deviceId)
    // {
    //     case 0x0D: GOS_LOG("Si7013 detected"); break;
    //     case 0x14: GOS_LOG("Si7020 detected"); break;
    //     case 0x15: GOS_LOG("Si7021 detected"); break;
    //     default: GOS_LOG("Unknown device detected");
    // }
  
    // Si7013_GetFirmwareRevision(&si7021_temp_rh_device, 0, &fwRev);
    // GOS_LOG("Firmware Version:  0x%x", fwRev);

    Si1133InitUvAls(&si1133_uv_als_device);
    Si115xInitSunlightImmuneProx(&si1153_long_range_device);
    Si115xInitGesture(&si1153_gesture_device);

    //Initialize and configure the pressure sensor
    bmp280_init(&bmp280);
    bmp280_get_config(&pressure_sensor_config, &bmp280);
    pressure_sensor_config.filter = BMP280_FILTER_OFF;
    pressure_sensor_config.os_pres = BMP280_OS_1X;
    pressure_sensor_config.os_temp = BMP280_OS_NONE;
    pressure_sensor_config.odr = BMP280_ODR_62_5_MS;

    bmp280_set_config(&pressure_sensor_config, &bmp280);
    bmp280_set_power_mode(BMP280_FORCED_MODE, &bmp280);

    measurement_time = bmp280_compute_meas_time(&bmp280);
  
    if(measurement_time < SI7013_MEASUREMENT_TIME_MS)   //if the bma250 measurement time is less than the si7013 use the si7013's
    {
        measurement_time = SI7013_MEASUREMENT_TIME_MS;
    }

    hurricane_sensors_start_periodic_measurements();
}   

void hurricane_sensors_start_periodic_measurements(void)
{ gos_event_register_periodic(start_measure_event_handler, 0, MEASURE_PERIOD_MS, 0);
}

void hurricane_sensors_stop_periodic_measurements(void)
{ gos_event_unregister_all(start_measure_event_handler);
}

float hurricane_sensor_get_temp(void)
{
    return data_temperature;
}

float hurricane_sensor_get_RH(void)
{
    return data_RH;
}

float hurricane_sensor_get_pressure(void)
{
    return data_pressure;
}

float hurricane_sensor_get_UV(void)
{
    return data_UV;
}

float hurricane_sensor_get_ALS(void)
{
    return data_ALS;
}

float hurricane_sensor_get_gesture_1(void)
{
    return data_gest_1;
}

float hurricane_sensor_get_gesture_2(void)
{
    return data_gest_2;
}

float hurricane_sensor_get_prox(void)
{
    return data_prox;
}

float hurricane_sensor_get_mag(void)
{
    return data_mag;
}

float hurricane_sensor_get_accel_x(void)
{
    return data_accel_x;
}

float hurricane_sensor_get_accel_y(void)
{
    return data_accel_y;
}

float hurricane_sensor_get_accel_z(void)
{
    return data_accel_z;
}

void hurricane_sensor_read_accel_immediate(float *x, float *y, float *z)
{
    icm20648_read_accel(&icm20648_device, x, y, z);
}

uint32_t hurricane_sensor_self_test(hurricane_sensor_message_callback_t message_callback)
{  
  uint32_t result = HURRICANE_SUCCESS;
  int16_t data;
  uint8_t part_id, part_rev, base_pn, pn_variant;
  char buffer[128];
  
  part_id = Si115xReadFromRegister(&si1133_uv_als_device, SI115x_REG_PART_ID);
  data = Si115xReadFromRegister(&si1133_uv_als_device, SI115x_REG_REV_ID);
  
  sprintf(buffer, "UV/ALS : ");
  if(part_id == SI1133_PART_ID)
  { sprintf(&buffer[strlen(buffer)], "Si11%x Rev %d.%d", part_id, ((data >> 4) & 0xF), (data & 0xF));
  }
  else
  { sprintf(&buffer[strlen(buffer)], "NOT FOUND");
    result = HURRICANE_FAIL;     
  }
  message_callback(buffer);


  part_id = Si115xReadFromRegister(&si1153_gesture_device, SI115x_REG_PART_ID);
  data = Si115xReadFromRegister(&si1153_gesture_device, SI115x_REG_REV_ID);
  sprintf(buffer, "Prox/Gesture : ");
  if(part_id == SI1153_PART_ID)
  {   sprintf(&buffer[strlen(buffer)], "Si11%x Rev %d.%d", part_id, ((data >> 4) & 0xF), (data & 0xF));
  }
  else
  { sprintf(&buffer[strlen(buffer)], "NOT FOUND");
    result = HURRICANE_FAIL;     
  }
  message_callback(buffer);

  part_id = Si115xReadFromRegister(&si1153_long_range_device, SI115x_REG_PART_ID);
  sprintf(buffer, "LR Prox : ");
  if(part_id == SI1153_PART_ID)
  {
    sprintf(&buffer[strlen(buffer)], "Si11%x Rev %d.%d", part_id, ((data >> 4) & 0xF), (data & 0xF));
  }
  else
  { sprintf(&buffer[strlen(buffer)], "NOT FOUND");
    result = HURRICANE_FAIL;     
  }
  message_callback(buffer);


  data = Si7013_Detect(&si7021_temp_rh_device, 0, &part_id);
  switch (part_id)
  { case SI7021_DEVICE_ID:
      data = 0x21;
      break;
    case SI7020_DEVICE_ID:
      data = 0x20;
      break;
    case SI7013_DEVICE_ID:
      data = 0x13;
      break;
    
  }
  Si7013_GetFirmwareRevision(&si7021_temp_rh_device, 0, &part_rev);
  if(part_rev == 0xFF)   //for the si70xx a FW rev = 0xFF is 1.0
    part_rev = 0x10;
  sprintf(buffer, "RH/Temp : ");
  if(part_id == SI7021_DEVICE_ID)
  { sprintf(&buffer[strlen(buffer)], "Si70%x FW %d.%d", data, ((part_rev >> 4) & 0xF), (part_rev & 0xF));
  }
  else
  { sprintf(&buffer[strlen(buffer)], "NOT FOUND");
    result = HURRICANE_FAIL;     
  }
  message_callback(buffer);


  Si72xx_IdentifyAndSleep(&si7200_hall_effect_device, 0, &part_id, &part_rev);
  Si72xx_ReadVariantAndSleep(&si7200_hall_effect_device, 0, &base_pn, &pn_variant);
  sprintf(buffer, "Hall Effect : ");
  if ((part_id == SI7210_PART_ID) && (base_pn == SI7210_BASE_PART_NUM))
  { sprintf(&buffer[strlen(buffer)], "Si72%d-%c", base_pn, ((part_rev==0x4)?'B':'?'));
  }
  else
  { sprintf(&buffer[strlen(buffer)], "NOT FOUND");
    result = HURRICANE_FAIL;     
  }
  message_callback(buffer);

  bmp280_init(&bmp280);
  sprintf(buffer, "Pressure : ");
  if((bmp280.chip_id != BMP280_CHIP_ID1) &&
     (bmp280.chip_id != BMP280_CHIP_ID2) &&
     (bmp280.chip_id != BMP280_CHIP_ID3))
  { sprintf(&buffer[strlen(buffer)], "NOT FOUND");
    result = HURRICANE_FAIL;     
  }
  else
  { sprintf(&buffer[strlen(buffer)], "BMP280");
  }  
  message_callback(buffer);


  icm20648_init(&icm20648_device);
  icm20648_read_id(&icm20648_device, &part_id);
  sprintf(buffer, "Accelerometer : ");  
  if(part_id == ICM20648_WHOAMI)
  { sprintf(&buffer[strlen(buffer)], "ICM-20648");
  }
  else
  { sprintf(&buffer[strlen(buffer)], "NOT FOUND");
    result = HURRICANE_FAIL;
  }
  message_callback(buffer);


  // self test secure element

  // set default i2c device
  ATCAIfaceCfg *se_interface = &cfg_ateccx08a_i2c_default;
  ATCA_STATUS status;
  uint8_t temp_buf[4];
  uint8_t device_type, revision;
  memset(temp_buf, 0, sizeof(temp_buf));

  sprintf(buffer, "Secure Element : ");

  status = atcab_init(se_interface);
  if (status == ATCA_SUCCESS)
  { 
    status = atcab_info(temp_buf);
    atcab_release();
    if (status == ATCA_SUCCESS)
    {
      
    }
    else
    { GOS_LOG("SE Debug Error = %d", status);
      sprintf(&buffer[strlen(buffer)], "NOT FOUND");
      result = HURRICANE_FAIL;
    }
  }
  else
  {
    sprintf(&buffer[strlen(buffer)], "NOT FOUND");
    result = HURRICANE_FAIL;
  }

  if(result != HURRICANE_FAIL)
  {
    device_type = temp_buf[2];
    revision = temp_buf[3];

    if(device_type == ECC508A_PART_NUM)
    {
      sprintf(&buffer[strlen(buffer)], "ECC508A");  
    }
    else if(device_type == ECC608A_PART_NUM)
    {
      sprintf(&buffer[strlen(buffer)], "ECC608A");  
    }
    else
    {
      sprintf(&buffer[strlen(buffer)], "UNKNOWN");   
    }

    sprintf(&buffer[strlen(buffer)], " Rev: %d", revision);
  }

  message_callback(buffer);

  return result;  
}


