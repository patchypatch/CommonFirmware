#include <stdint.h>
#include "stm32f4xx.h"

IRQn_Type IRQn_tim_array[] = {(IRQn_Type)0, (IRQn_Type)0, TIM2_IRQn, TIM3_IRQn, TIM4_IRQn,
							   TIM5_IRQn};
IRQn_Type IRQn_dma_array[][8] = { {(IRQn_Type)0, (IRQn_Type)0, (IRQn_Type)0, (IRQn_Type)0,
								   (IRQn_Type)0, (IRQn_Type)0, (IRQn_Type)0, (IRQn_Type)0},
								  {DMA1_Stream0_IRQn, DMA1_Stream1_IRQn, DMA1_Stream2_IRQn,
								   DMA1_Stream3_IRQn, DMA1_Stream4_IRQn, DMA1_Stream5_IRQn,
								   DMA1_Stream6_IRQn, DMA1_Stream7_IRQn},
								  {DMA2_Stream0_IRQn, DMA2_Stream1_IRQn, DMA2_Stream2_IRQn,
								   DMA2_Stream3_IRQn, DMA2_Stream4_IRQn, DMA2_Stream5_IRQn,
								   DMA2_Stream6_IRQn, DMA2_Stream7_IRQn} };

void nvic_mask_global(void) {
	__disable_irq(); // disable global interrupts
}

void nvic_unmask_global(void) {
	__enable_irq(); // enable global interrupts
}

void nvic_enable_tim(uint8_t timNum) {
	/* NVIC Setup */
	uint32_t priority = 1; // 0-255 with 0 being highest priority
	__NVIC_SetPriority(IRQn_tim_array[timNum], priority);
	NVIC_EnableIRQ(IRQn_tim_array[timNum]);
}

void nvic_enable_dma(uint8_t dmaNum, uint8_t streamNum) {
	/* NVIC Setup */
	uint32_t priority = 20; // 0-255 with 0 being highest priority
	__NVIC_SetPriority(IRQn_dma_array[dmaNum][streamNum], priority);
	NVIC_EnableIRQ(IRQn_dma_array[dmaNum][streamNum]);
}