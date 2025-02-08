#include <stdint.h>

void spi_init(uint8_t channel, uint8_t port, uint8_t pinCS, uint8_t pinSCK,
			  uint8_t pinMOSI, uint8_t pinMISO, uint8_t idxAF, uint8_t spiMode);

void spi_enable(uint8_t channel);
void spi_disable(uint8_t channel);
void spi_enable_dma(uint8_t channel, bool isEnableTxDMA, bool isEnableRxDMA);

void spi_write_byte(uint8_t channel, uint8_t data);
void spi_write_half_word(uint8_t channel, uint16_t data);

void spi_read_bytes_start(uint8_t channel, uint8_t data);
void spi_read_end(uint8_t channel);

void spi_set_data_frame_format(uint8_t channel, uint8_t numBitsFormat);