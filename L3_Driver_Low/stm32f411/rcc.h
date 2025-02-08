#include <stdint.h>

void rcc_enable_clock_dma(uint8_t dma);
void rcc_enable_clock_gpio(uint8_t portIdx);
void rcc_enable_clock_i2c(uint8_t channel);
void rcc_enable_clock_spi(uint8_t channel);
void rcc_enable_clock_usart(uint8_t channel);