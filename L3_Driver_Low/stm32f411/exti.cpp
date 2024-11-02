#include "exti.h"
#include "stm32f4xx.h"
#include <stdio.h>

#define EXTI_OFFSET 0x08UL

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void exti_init(char port, uint8_t pin, bool isRisingTrigger, bool isFallingTrigger) {
    /* SYSCFG Multiplexor setup */
    // Enable SYSCFG clock
    RCC->APB2ENR |= (0x1UL << RCC_APB2ENR_SYSCFGEN_Pos); // set to enable
   printf("RCC_APB2ENR = 0x%8x\n", RCC->APB2ENR); 
    // Configure exti using sequential registers, each with 4 config nibbles
    uint32_t* extiConfigReg = (uint32_t*) ( SYSCFG_BASE + EXTI_OFFSET + (((uint32_t)pin / 4UL) * 4UL) );
    uint32_t extiShift = (uint32_t)((pin % 4) * 4);
    uint32_t portValue = (uint32_t)port - (uint32_t)'A';
    printf("SYSCFG_BASE=0x%8x\n", SYSCFG_BASE);
    printf("EXTI_OFFSET=0x%8x\n", EXTI_OFFSET);
    printf("SYSCFG_BASE+EXTI_OFFSET=0x%8x\n", SYSCFG_BASE + EXTI_OFFSET);
    printf("extiConfigReg=0x%8x\n", extiConfigReg);

    *extiConfigReg &= ~(0x0FUL << extiShift); // clear 4 exti config bits for this pin
    *extiConfigReg |= (portValue << extiShift); // set 4 bits to 0 for A, 1 for B, ... , 4 for E
    printf("extiConfigRegContents=0x%8x\n", SYSCFG->EXTICR[2]);
    /* EXTI Setup */
    // Enable mask in EXTI_IMR
    EXTI->IMR |= (0x1UL << pin); // set to unmask
    printf("EXTI_IMR = %8x\n", EXTI->IMR);
    // Select rising or falling trigger in EXTI_RTSR or EXTI_FTSR
    if(isRisingTrigger) EXTI->RTSR |= (0x1UL << pin);
    if(isFallingTrigger) EXTI->FTSR |= (0x1UL << pin);
    printf("EXTI_RTSR = %8x\n", EXTI->RTSR);
    printf("EXTI_FTSR = %8x\n", EXTI->FTSR);

    /* NVIC Setup */
    // Determine NVIC vector position
    uint8_t nvicVecPos;
    if(pin < 5) {
        nvicVecPos = (uint8_t)EXTI0_IRQn + pin;
    } else if (pin >= 5 && pin <= 9) {
        nvicVecPos = (uint8_t)EXTI9_5_IRQn;
    } else if (pin >=10 && pin <=15) {
        nvicVecPos = (uint8_t)EXTI15_10_IRQn;
    }
    printf("nvicVecPos = %d\n", nvicVecPos);
    // Set priority in NVIC_IPRx via core_cm4 function
    uint32_t priority = 1; // 0-255 with 0 being highest priority
    __NVIC_SetPriority((IRQn_Type)nvicVecPos, priority);
    // Enable interrupt in NVIC_ISERx via core_cm4 function
    NVIC_EnableIRQ((IRQn_Type)nvicVecPos);
}

void exti_suspend(uint8_t pin) {
    // Mask event in EXTI IMR (Interrupt Mask Register)
    EXTI->IMR &= ~(1UL << pin);
}

void exti_resume(uint8_t pin) {
    // Enable mask in EXTI_IMR
    EXTI->IMR |= (0x1UL << pin); // set to unmask
}

/* EXTI Handlers */
void exti0_handler(void) {
	printf("HandleExti0\n");
    interrupt_handler_exti(0U);
    EXTI->PR = (1UL << 0); // clear interrupt
}

void exti1_handler(void) {
	printf("HandleExti1\n");
    interrupt_handler_exti(1U);
    EXTI->PR = (1UL << 1); // clear interrupt
}

void exti2_handler(void) {
	printf("HandleExti2\n");
    interrupt_handler_exti(2U);
    EXTI->PR = (1UL << 2); // clear interrupt
}

void exti3_handler(void) {
	printf("HandleExti3\n");
    interrupt_handler_exti(3U);
    EXTI->PR = (1UL << 3); // clear interrupt
}

void exti4_handler(void) {
	printf("HandleExti4\n");
    interrupt_handler_exti(4U);
    EXTI->PR = (1UL << 4); // clear interrupt
}

void exti9_5_handler(void) {
	printf("HandleExti95\n");
    //EXTI->PR = (1UL << 0);
}

void exti15_10_handler(void) {
    printf("HandleExti1510\n");
    //EXTI->PR = (1UL << 10);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */