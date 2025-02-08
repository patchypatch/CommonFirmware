#include "stm32f4xx.h"

void nvic_mask_global(void);
void nvic_unmask_global(void);

void nvic_enable_tim(uint8_t timNum);
void nvic_enable_dma(uint8_t dmaNum, uint8_t streamNum);