#include "gos.h"
#include "pca9535.h"
#include "stdint.h"



static uint32_t pca9535_read_reg(gos_i2c_device_t *i2c_device, uint8_t regaddr);


static uint32_t pca9535_read_reg(gos_i2c_device_t *i2c_device, uint8_t regaddr)
{
	uint32_t data;
  gos_result_t ret;
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

  ret = gos_i2c_transfer(i2c_device, messages, 2);
  if(ret != GOS_SUCCESS)
  { data = 0;
    return (uint32_t)-1;
  }
  else
  { data = i2c_read_data[1];
    data |= i2c_read_data[2] << 8;
    return data;
  }
}




static void pca9535_write_reg(gos_i2c_device_t *i2c_device, uint8_t regaddr, uint16_t data)
{
  uint8_t i2c_write_data[3];
  gos_i2c_message_t messages[1];

  i2c_write_data[0] = regaddr;
	i2c_write_data[1] = data & 0xff;
  i2c_write_data[2] = data >> 8;
  messages[0].buffer = i2c_write_data;
  messages[0].length = 3;
  messages[0].flags = GOS_I2C_MSG_START_SEQ | GOS_I2C_MSG_STOP_SEQ | GOS_I2C_MSG_WRITE;


  gos_i2c_transfer(i2c_device, messages, 1);

}
	
	
uint32_t pca9535_read_input(gos_i2c_device_t *i2c_device)
{
	return pca9535_read_reg(i2c_device, 0);
}


void pca9535_write_output(gos_i2c_device_t *i2c_device, uint16_t param)
{
	pca9535_write_reg(i2c_device, 2, param);
}

void pca9535_set_dir(gos_i2c_device_t *i2c_device, uint16_t param)
{
	pca9535_write_reg(i2c_device, 6, param);
}

