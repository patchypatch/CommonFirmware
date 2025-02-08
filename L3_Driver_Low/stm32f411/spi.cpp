#include <stdio.h>
#include "spi.h"
#include "gpio.h"
#include "rcc.h"
#include "stm32f4xx.h"

SPI_TypeDef* SPI_array[] = {NULL, SPI1, SPI2, SPI3, SPI4, SPI5};

/* CS ON when in hardware CS mode */
void spi_enable(uint8_t channel) {
	/* Read spi data to clear any Rx data not empty flag */
	uint16_t temp = SPI_array[channel]->DR;
	/* Enable SPI Tx/Rx */
	SPI_array[channel]->CR1 |= SPI_CR1_SPE;
}

/* CS OFF when in hardware CS mode */
void spi_disable(uint8_t channel) {
	/* Disable SPI Tx/Rx */
	SPI_array[channel]->CR1 &= ~SPI_CR1_SPE;
}

void spi_enable_dma(uint8_t channel, bool isEnableTxDMA, bool isEnableRxDMA) {
	if(isEnableTxDMA) SPI_array[channel]->CR2 |= SPI_CR2_TXDMAEN;
	if(isEnableRxDMA) SPI_array[channel]->CR2 |= SPI_CR2_RXDMAEN;
}

void spi_init(uint8_t channel, uint8_t port, uint8_t pinCS, uint8_t pinSCK, uint8_t pinMOSI, uint8_t pinMISO,
			  uint8_t idxAF, uint8_t spiMode) {
	/* Enable Clock */
	rcc_enable_clock_spi(channel);
	gpio_enable_spi(port, pinCS, pinSCK, pinMOSI, pinMISO, idxAF);

	/* Disable SPI Tx/Rx */
	spi_disable(channel);

	/* Set baud rate */
	if(channel == 1 || channel == 4 || channel == 5) { // On APB2 (high speed peripheral bus)
		// 50 MHz APB1 clock / 16 == 3.125 Mhz baud
		// 50 MHz APB1 clock / 32 == 1.5625 Mhz baud
		SPI_array[channel]->CR1 &= ~SPI_CR1_BR;
		SPI_array[channel]->CR1 |= 0b100UL << SPI_CR1_BR_Pos;
	} else { // On APB1 (low speed peripheral bus)
		// 50 MHz APB1 clock / 16 == 3.125 Mhz baud
		// 50 MHz APB1 clock / 32 == 1.5625 Mhz baud
		SPI_array[channel]->CR1 &= ~SPI_CR1_BR;
		SPI_array[channel]->CR1 |= 0b100UL << SPI_CR1_BR_Pos;
	}

	/* Set SPI mode */
	// CPOL and CPHA bits
	switch(spiMode) {
		case 0: // CPOL == 0, CPHA == 0
			SPI_array[channel]->CR1 &= ~(SPI_CR1_CPOL | SPI_CR1_CPHA);
			break;
		case 1: // CPOL == 0, CPHA == 1
			SPI_array[channel]->CR1 &= ~SPI_CR1_CPOL;
			SPI_array[channel]->CR1 |= SPI_CR1_CPHA;
			break;
		case 2: // CPOL == 1, CPHA == 0
			SPI_array[channel]->CR1 |= SPI_CR1_CPOL;
			SPI_array[channel]->CR1 &= ~SPI_CR1_CPHA;
			break;
		case 3: // CPOL == 1, CPHA == 1
			SPI_array[channel]->CR1 |= (SPI_CR1_CPOL | SPI_CR1_CPHA);
			break;
	}

	/* Set data frame format */
	// 8-bit or 16-bit
	SPI_array[channel]->CR1 &= ~SPI_CR1_DFF; // 8-bit mode
	// LSB or MSB first
	SPI_array[channel]->CR1 &= ~SPI_CR1_LSBFIRST; // MSB first

	/* Set chip select (CS) behavior */
	// Software or Hardware management
	SPI_array[channel]->CR1 &= ~SPI_CR1_SSM; // Hardware management

	/* Master or Slave */
	SPI_array[channel]->CR1 |= SPI_CR1_MSTR; // Master
	SPI_array[channel]->CR2 |= SPI_CR2_SSOE; // Not multi-master
}

void spi_write_half_word(uint8_t channel, uint16_t data) {
	/* Ensure SPI is disabled */
	spi_disable(channel);

	/* Set data frame format */
	SPI_array[channel]->CR1 |= SPI_CR1_DFF; // 16-bit mode

	/* Enable SPI */
	spi_enable(channel);

	/* Put data in data register */
	SPI_array[channel]->DR = data;

	/* Wait until data transmit complete */
	while(!(SPI_array[channel]->SR & SPI_SR_TXE));

	/* Disable SPI */
	spi_disable(channel);
}

void spi_write_byte(uint8_t channel, uint8_t data) {
	/* Ensure SPI is disabled */
	spi_disable(channel);

	/* Set data frame format */
	SPI_array[channel]->CR1 &= ~SPI_CR1_DFF; // 8-bit mode

	/* Enable SPI */
	spi_enable(channel);

	/* Put data in data register */
	SPI_array[channel]->DR = data;

	/* Wait until data transmit complete */
	while(!(SPI_array[channel]->SR & SPI_SR_TXE));

	/* Disable SPI */
	spi_disable(channel);
}

void spi_read_bytes_start(uint8_t channel, uint8_t data) {
	/* Ensure SPI is disabled */
	spi_disable(channel);

	/* Set data frame format */
	SPI_array[channel]->CR1 &= ~SPI_CR1_DFF; // 8-bit mode

	/* Enable SPI */
	spi_enable(channel);

	/* Put data in data register */
	SPI_array[channel]->DR = data;

	/* Wait until data transmit complete */
	while(!(SPI_array[channel]->SR & SPI_SR_TXE));

	/* Leave spi enabled */
}

void spi_read_end(uint8_t channel) {
	/* Disable SPI */
	spi_disable(channel);
}

void spi_set_data_frame_format(uint8_t channel, uint8_t numBitsFormat) {
	/* Ensure SPI is disabled */
	spi_disable(channel);

	/* Set data frame format */
	if(numBitsFormat == 8) {
		SPI_array[channel]->CR1 &= ~SPI_CR1_DFF; // 8-bit mode
	} else if (numBitsFormat == 16) {
		SPI_array[channel]->CR1 |= SPI_CR1_DFF; // 16-bit mode
	}
}