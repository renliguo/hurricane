#pragma once

uint32_t pca9535_read_input(gos_i2c_device_t *i2c_device);
void pca9535_write_output(gos_i2c_device_t *i2c_device, uint16_t param);
void pca9535_set_dir(gos_i2c_device_t *i2c_device, uint16_t param);


