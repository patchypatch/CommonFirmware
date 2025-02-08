#include <stddef.h>
#include <stdio.h>
#include "usart.h"
#include "gpio.h"
#include "rcc.h"
#include "stm32f4xx.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

USART_TypeDef* USART_array[] = {NULL, USART1, USART2, NULL, NULL, NULL, USART6};

void usart_init(uint8_t channel, char port, uint8_t pinTx, uint8_t pinRx, uint8_t idxAF, bool isOpenDrain) {
	/* Enables */
	rcc_enable_clock_usart(channel);
	gpio_enable_usart(port, pinTx, pinRx, idxAF, isOpenDrain);

	/* Configure and enable USART */
	if(channel == 1 || channel == 6) { // On APB2 (high speed peripheral bus)
		USART_array[channel]->BRR = (uint16_t) 0x1B2; // 115200 baud @ 50 MHz APB2 clock and 16x oversampling
	} else { // On APB1 (low speed peripheral bus)
		USART_array[channel]->BRR = (uint16_t) 0x1B2; // 115200 baud @ 50 MHz APB1 clock and 16x oversampling
	}
	USART_array[channel]->CR1 |= USART_CR1_UE; // USART enable
	USART_array[channel]->CR1 |= USART_CR1_TE | USART_CR1_RE; // transmitter and receiver enables

	// Dummy write, because the first byte is dropped
	USART_array[channel]->DR = 0;
	while (!(USART_array[channel]->SR & USART_SR_TC));
}

void usart_write_byte(uint8_t channel, char c) {
	USART_array[channel]->DR = c; // put character in output register
	while (!(USART_array[channel]->SR & USART_SR_TC)); // wait for transmission complete flag
}

void usart_write(uint8_t channel, uint8_t* dataPtr, uint8_t dataLenBytes) {
	for (uint8_t byteIdx = 0; byteIdx < dataLenBytes; byteIdx++) {
		usart_write_byte(channel, *(dataPtr + byteIdx));
	}
}

void usart_enable_onewire_mode(uint8_t channel) {
	/* Set half-duplex mode: HDSEL bit in USART_CR3 register
	 * --> TX and RX internally connected
	 * --> RX pin no longer used 
	 * --> TX pin released when no data is being transmitted (Must configure pin as Open-Drain) */
	USART_array[channel]->CR3 |= 1 << USART_CR3_HDSEL_Pos;
}

void usart_enable_Rx(uint8_t channel) {
	USART_array[channel]->CR1 |= USART_CR1_RE; // receiver enable
}

void usart_disable_Rx(uint8_t channel) {
	USART_array[channel]->CR1 &= ~USART_CR1_RE; // receiver disable
}

void usart_enable_Tx(uint8_t channel) {
	USART_array[channel]->CR1 |= USART_CR1_TE; // transmitter enable
}

void usart_disable_Tx(uint8_t channel) {
	USART_array[channel]->CR1 &= ~USART_CR1_TE; // transmitter disable
}

uint8_t usart_read_byte(uint8_t channel, uint8_t* dataPtr) {
	while(!(USART_array[channel]->SR & USART_SR_RXNE)); // wait for data register not empty
	*dataPtr = (uint8_t) USART_array[channel]->DR & USART_DR_DR;
	return 0;
}

uint8_t usart_read(uint8_t channel, uint8_t* dataPtr, uint8_t dataLenBytes) {
	for (uint8_t byteIdx = 0; byteIdx < dataLenBytes; byteIdx++) {
		usart_read_byte(channel, (dataPtr + byteIdx));
	}
	return 0;
}

void usart_enable_dma(uint8_t channel, bool isEnableTxDMA, bool isEnableRxDMA) {
	if(isEnableTxDMA) USART_array[channel]->CR3 |= USART_CR3_DMAT;
	if(isEnableRxDMA) USART_array[channel]->CR3 |= USART_CR3_DMAR;
}

void usart_enable_idle_isr(uint8_t channel) {
	USART_array[channel]->CR1 |= USART_CR1_IDLEIE;
}

void usart_disable_idle_isr(uint8_t channel) {
	USART_array[channel]->CR1 &= ~USART_CR1_IDLEIE;
}

void usart2_handler(void) {
	interrupt_handler_uart(2U);
	USART2->SR &= ~USART_SR_IDLE; // Clear idle line detected flag
}

#ifdef __cplusplus
}
#endif /* __cplusplus */