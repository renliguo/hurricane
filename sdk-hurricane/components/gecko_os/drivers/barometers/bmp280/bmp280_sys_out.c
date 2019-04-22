#include "gos.h"
#include "bmp280.h"
#include "bmp280_sys_out.h"

static gos_i2c_device_t bmp280_i2c_device =
{
    .port    = PLATFORM_STD_I2C,
    .address = BMP280_I2C_ADDR_SEC,
    .speed   = GOS_I2C_CLOCK_HIGH_SPEED,
    .retries = 0,
    .flags   = 0
};


int8_t bmp280_ReadRegister(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{

  gos_result_t ret;

  uint8_t i2c_write_data[1]; 
  gos_i2c_message_t messages[2];
  
  i2c_write_data[0] = reg_addr;
  messages[0].buffer = i2c_write_data;
  messages[0].length = 1;
  messages[0].flags = GOS_I2C_MSG_START_SEQ | GOS_I2C_MSG_WRITE ;
  messages[1].buffer = data;
  messages[1].length = len;
  messages[1].flags = GOS_I2C_MSG_STOP_SEQ | GOS_I2C_MSG_READ;

  ret = gos_i2c_transfer(&bmp280_i2c_device, messages, 2);

  if(ret != GOS_SUCCESS)
  { *data = 0xff;
    return (int8_t)ret;
  }
  return (int8_t)0;  
}




int8_t bmp280_WriteRegister(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
  gos_result_t ret;
  uint8_t i2c_write_data[256];
  gos_i2c_message_t messages[1];
  int32_t i;

  i2c_write_data[0] = reg_addr;
  for(i=0; i<len; i++)
  {
    i2c_write_data[i+1] = data[i];
    
  }
  messages[0].buffer = i2c_write_data;
  messages[0].length = len+1;
  messages[0].flags = GOS_I2C_MSG_START_SEQ | GOS_I2C_MSG_STOP_SEQ | GOS_I2C_MSG_WRITE;

  ret = gos_i2c_transfer(&bmp280_i2c_device, messages, 1);

  if (ret != GOS_SUCCESS)
  {
    return (int8_t)ret;
  }
  return (int8_t)0;  
}

void bmp280_delay_ms(uint32_t period)
{
  gos_rtos_delay_milliseconds(period);
  
}
