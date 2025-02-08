#include <stdio.h>
#include "interrupt.h"
#include "dma.h"
#include "exti.h"
#include "tim.h"
#include "nvic.h"
#include "usart.h"
#include "spi.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef void(*FunctionPointer_t)(void);
FunctionPointer_t pf_handlers_dma[2][8];
FunctionPointer_t pf_handlers_uart[16];
FunctionPointer_t pf_handlers_exti[16];
FunctionPointer_t pf_handlers_tim[16];

/* DMA */
void init_interrupt_dma_uart_Rx_8ByteMsgs(uint8_t dma, uint8_t stream, uint8_t channel,
										   uint8_t uartChannel, uint8_t* memPtr, void(*pf_handler)(void)) {
	/* Store pointer to app's interrupt handler */
	pf_handlers_dma[dma][stream] = pf_handler;

	/* DMA init */
	dma_config(dma, stream, channel, Peripheral2Memory, VeryHigh, true,
			  (uint32_t)dma_periph_data_by_usart_array[uartChannel], (uint32_t)memPtr, (uint16_t)8,
			  Byte, Byte, HalfFull, true, EightBeats, OneBeat, true, false, true);
	dma_enable(dma, stream, 8);

	/* Enable DMA mode for USART Rx */
	usart_enable_dma(uartChannel, false, true);
}

void init_interrupt_dma_spi_Rx_6HalfWordMsgs(uint8_t dma, uint8_t stream, uint8_t channel,
											 uint8_t spiChannel, uint8_t* memPtr, void(*pf_handler)(void)) {
	/* Store pointer to app's interrupt handler */
	pf_handlers_dma[dma][stream] = pf_handler;

	/* Init DMA */
	dma_config(dma, stream, channel, Peripheral2Memory, VeryHigh, true,
			  (uint32_t)dma_periph_data_by_spi_array[spiChannel], (uint32_t)memPtr, (uint16_t)13,
			  Byte, Byte, DirectMode, true, OneBeat, OneBeat, true, false, true);

	/* Enable DMA mode for SPI Rx */
	spi_enable_dma(spiChannel, false, true);
}

void interrupt_handler_dma(uint8_t dma, uint8_t stream) {
	pf_handlers_dma[dma][stream]();
}

/* UART */
void interrupt_handler_uart(uint8_t channel) {
	pf_handlers_uart[channel]();
}

/* NVIC */
void interrupt_mask_global(void) {
	nvic_mask_global();
}

void interrupt_unmask_global(void) {
	nvic_unmask_global();
}

/* EXTI */
void init_interrupt_exti(char port, uint8_t pin, bool isRisingTrigger, bool isFallingTrigger,
						 void(*pf_handler)(void)) {
	exti_init(port, pin, isRisingTrigger, isFallingTrigger);
	pf_handlers_exti[pin] = pf_handler;
}

void interrupt_handler_exti(uint8_t pin) {
	pf_handlers_exti[pin]();
}

void suspend_interrupt_exti(uint8_t pin) {
	exti_suspend(pin);
}

void resume_interrupt_exti(uint8_t pin) {
	exti_resume(pin);
}

/* TIM */
void init_interrupt_tim(uint8_t timNumber, uint32_t timDuration_ms, void(*pf_handler)(void)) {
	tim_init(timNumber, timDuration_ms);
	pf_handlers_tim[timNumber] =  pf_handler;
}

void interrupt_handler_tim(uint8_t timNumber) {
	pf_handlers_tim[timNumber]();
}

void reset_interrupt_tim(uint8_t timNumber) {
	tim_reset(timNumber);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */