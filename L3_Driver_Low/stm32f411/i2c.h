#include "stm32f4xx.h"

void i2c_init_fm(uint8_t i2c_channel, uint8_t i2c_pin_sda, uint8_t i2c_pin_scl);
void i2c_init_sm(uint8_t i2c_channel, uint8_t i2c_pin_sda, uint8_t i2c_pin_scl);
void i2c_write8(uint8_t i2c_channel, uint8_t sid, uint8_t addr, uint8_t data);
void i2c_writeAddr(uint8_t i2c_channel, uint8_t sid, uint8_t addr);
uint16_t i2c_read16(uint8_t i2c_channel, uint8_t sid);
