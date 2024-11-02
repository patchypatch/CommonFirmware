#include <stdio.h>
#include "tim.h"
#include "stm32f4xx.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

TIM_TypeDef* TIM_array[] = {NULL, TIM1, TIM2, TIM3, TIM4, TIM5};

uint8_t RCC_TIM_Pos_array[] = {0, 0, RCC_APB1ENR_TIM2EN_Pos,
    RCC_APB1ENR_TIM3EN_Pos, RCC_APB1ENR_TIM4EN_Pos, RCC_APB1ENR_TIM5EN_Pos};

IRQn_Type IRQn_array[] = {(IRQn_Type)0, (IRQn_Type)0, TIM2_IRQn, TIM3_IRQn, TIM4_IRQn, TIM5_IRQn};

void tim_init(uint8_t timNumber, uint32_t timDuration_ms) {
    // Enable clock to TIM
    RCC->APB1ENR |= (1UL << RCC_TIM_Pos_array[timNumber]);

    /* TIM Setup */
    // Set direction of TIM
    //TIM_array[timNumber]->CR1 |= (1UL << TIM_CR1_DIR_Pos); // set for countdown
    // Set to one-pulse mode (auto-disable at update event)
    TIM_array[timNumber]->CR1 |= (1UL << TIM_CR1_OPM_Pos);
    // Set prescaler -- 100 Mhz clock
    TIM_array[timNumber]->PSC = (uint16_t)(50000 - 1); // divides 100Mhz input give 0.5ms tick
    // Set auto-reload value
    TIM_array[timNumber]->ARR = 2 * timDuration_ms;
    // Update Generation (to update the clocking due to shadow register for clock frequency)
    TIM_array[timNumber]->EGR |= (1UL << TIM_EGR_UG_Pos);
    // Clear all interrupt flags
    TIM_array[timNumber]->SR = 0;
    // Set to interrupt at timer update
    TIM_array[timNumber]->DIER |= (1UL << TIM_DIER_UIE_Pos);
    printf("TIM4_CR1 = 0x%4x\n", TIM4->CR1); 
    // Disable counter
    TIM_array[timNumber]->CR1 &= ~(TIM_CR1_CEN_Msk); // clear for disable

    /* NVIC Setup */
    uint32_t priority = 1; // 0-255 with 0 being highest priority
    __NVIC_SetPriority(IRQn_array[timNumber], priority);
    NVIC_EnableIRQ(IRQn_array[timNumber]);

    printf("tim_init of TIM%d\n", timNumber);
}

void tim_reset(uint8_t timNumber) {
    printf("TIM%dreset\n", timNumber);
    // Reset counter
    // TIM_array[timNumber]->CNT = 0UL;
    // Clear all interrupt flags
    TIM_array[timNumber]->SR = 0;
    // Enable counter
    TIM_array[timNumber]->CR1 |= (1UL << TIM_CR1_CEN_Pos);
    printf("TIM4_CR1 = 0x%4x\n", TIM4->CR1); 
}

void tim2_handler(void) {
    printf("HandleTIM2\n");
    interrupt_handler_tim(2U);
    TIM2->SR = 0; // clear all interrupt flags
}

void tim3_handler(void) {
    printf("HandleTIM3\n");
    interrupt_handler_tim(3U);
    TIM3->SR = 0; // clear all interrupt flags
}

void tim4_handler(void) {
    printf("HandleTIM4\n");
    printf("TIM4_SR = 0x%4x\n", TIM4->SR);
    interrupt_handler_tim(4U);
    TIM4->SR = 0; // clear all interrupt flags
}

void tim5_handler(void) {
    printf("HandleTIM5\n");
    interrupt_handler_tim(5U);
    TIM5->SR = 0; // clear all interrupt flags
}

#ifdef __cplusplus
}
#endif /* __cplusplus */