#include <stddef.h>
#include "rcc.h"
#include "stm32f4xx.h"

// Arrays ordered by port index (A = 0, B = 1, ...)
uint8_t RCC_AHB_GPIO_Pos_array[] = {RCC_AHB1ENR_GPIOAEN_Pos, RCC_AHB1ENR_GPIOBEN_Pos,
	RCC_AHB1ENR_GPIOCEN_Pos, RCC_AHB1ENR_GPIODEN_Pos, RCC_AHB1ENR_GPIOEEN_Pos};

// Arrays are ordered by peripheral channel number
volatile uint32_t* RCC_APB_ENR_I2C_array[] = {NULL, &(RCC->APB1ENR), &(RCC->APB1ENR),
	&(RCC->APB1ENR)};

volatile uint32_t* RCC_APB_ENR_SPI_array[] = {NULL, &(RCC->APB2ENR), &(RCC->APB1ENR),
	&(RCC->APB1ENR), &(RCC->APB2ENR), &(RCC->APB2ENR)};

volatile uint32_t* RCC_APB_ENR_USART_array[] = {NULL, &(RCC->APB2ENR), &(RCC->APB1ENR),
	NULL, NULL, NULL, &(RCC->APB2ENR)};

uint8_t RCC_DMA_EN_Pos_array[] = {0, RCC_AHB1ENR_DMA1EN_Pos, RCC_AHB1ENR_DMA2EN_Pos};

uint8_t RCC_APB_EN_I2C_Pos_array[] = {0, RCC_APB1ENR_I2C1EN_Pos, RCC_APB1ENR_I2C2EN_Pos,
	RCC_APB1ENR_I2C3EN_Pos};

uint8_t RCC_APB_EN_SPI_Pos_array[] = {0, RCC_APB2ENR_SPI1EN_Pos, RCC_APB1ENR_SPI2EN_Pos,
	RCC_APB1ENR_SPI3EN_Pos, RCC_APB2ENR_SPI4EN_Pos, RCC_APB2ENR_SPI5EN_Pos};

uint8_t RCC_APB_EN_USART_Pos_array[] = {0, RCC_APB2ENR_USART1EN_Pos, RCC_APB1ENR_USART2EN_Pos,
	0, 0, 0, RCC_APB2ENR_USART6EN_Pos};

void rcc_enable_clock_dma(uint8_t dma) {
	/* Enable DMA clock */
	RCC->AHB1ENR |= 1UL << RCC_DMA_EN_Pos_array[dma];
	// do two dummy reads after enabling the clock, as per the errata
	volatile uint32_t dummy;
	dummy = RCC->AHB1ENR;
	dummy = RCC->AHB1ENR;
}

void rcc_enable_clock_gpio(uint8_t portIdx) {
	RCC->AHB1ENR |= (1UL << RCC_AHB_GPIO_Pos_array[portIdx]);
	// do two dummy reads after enabling the peripheral clock, as per the errata
	volatile uint32_t dummy;
	dummy = RCC->AHB1ENR;
	dummy = RCC->AHB1ENR;
}

void rcc_enable_clock_i2c(uint8_t channel) {
	/* Enable I2C1 clock */
	*RCC_APB_ENR_I2C_array[channel] |= (1 << RCC_APB_EN_I2C_Pos_array[channel]);
	// do two dummy reads after enabling the peripheral clock, as per the errata
	volatile uint32_t dummy;
	dummy = *RCC_APB_ENR_I2C_array[channel];
	dummy = *RCC_APB_ENR_I2C_array[channel];
}

void rcc_enable_clock_spi(uint8_t channel) {
	/* Enable SPI clock */
	*RCC_APB_ENR_SPI_array[channel] |= (1 << RCC_APB_EN_SPI_Pos_array[channel]);
	// do two dummy reads after enabling the peripheral clock, as per the errata
	volatile uint32_t dummy;
	dummy = *RCC_APB_ENR_SPI_array[channel];
	dummy = *RCC_APB_ENR_SPI_array[channel];
}

void rcc_enable_clock_usart(uint8_t channel) {
	/* Enable USART clock */
	*RCC_APB_ENR_USART_array[channel] |= (1 << RCC_APB_EN_USART_Pos_array[channel]);
	// do two dummy reads after enabling the peripheral clock, as per the errata
	volatile uint32_t dummy;
	dummy = *RCC_APB_ENR_USART_array[channel];
	dummy = *RCC_APB_ENR_USART_array[channel];
}