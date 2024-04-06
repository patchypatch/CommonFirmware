#include <stdio.h>
#include "interrupt.h"
#include "exti.h"
#include "tim.h"
#include "nvic.h"
#include "stm32f4xx.h" //move to lower layer

#define SYS_TICK_CLOCK_DIVIDER 100000

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef void(*FunctionPointer_t)(void);
FunctionPointer_t pf_handlers_exti[16];
FunctionPointer_t pf_handlers_tim[16];
/* SysTick */
void interrupt_init_systick(void) {
	SysTick_Config(SYS_TICK_CLOCK_DIVIDER);
}

/* NVIC */
void interrupt_mask_global(void) {
	nvic_mask_global();
}

void interrupt_unmask_global(void) {
	nvic_unmask_global();
}

/* EXTI */
void init_interrupt_exti(char port, uint8_t pin, bool isRisingTrigger, bool isFallingTrigger, void(*pf_handler)(void)) {
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