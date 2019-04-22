#pragma once





int8_t bmp280_ReadRegister(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len);
int8_t bmp280_WriteRegister(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len);
void bmp280_delay_ms(uint32_t period);
