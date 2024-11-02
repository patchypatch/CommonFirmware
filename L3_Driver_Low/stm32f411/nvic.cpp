#include "stm32f4xx.h"

void nvic_mask_global(void) {
	__disable_irq(); // disable global interrupts
}

void nvic_unmask_global(void) {
	__enable_irq(); // enable global interrupts
}