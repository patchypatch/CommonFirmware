#include <stddef.h>
#include "usart.h"
#include "gpio.h"
#include "stm32f4xx.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

// Arrays are ordered by usart channel number
volatile uint32_t* RCC_APB_ENR_USART_array[] = {NULL, &(RCC->APB2ENR), &(RCC->APB1ENR),
  NULL, NULL, NULL, &(RCC->APB2ENR)};

uint8_t RCC_APB_EN_USART_Pos_array[] = {0, RCC_APB2ENR_USART1EN_Pos, RCC_APB1ENR_USART2EN_Pos,
  0, 0, 0, RCC_APB2ENR_USART6EN_Pos};

USART_TypeDef* USART_array[] = {NULL, USART1, USART2, NULL, NULL, NULL, USART6};

void usart_init(uint8_t channel, char port, uint8_t pinTx, uint8_t pinRx, uint8_t idxAF) {
  /* Enable USART clock */
  *RCC_APB_ENR_USART_array[channel] |= (1 << RCC_APB_EN_USART_Pos_array[channel]);
  // do two dummy reads after enabling the peripheral clock, as per the errata
  volatile uint32_t dummy;
  dummy = *RCC_APB_ENR_USART_array[channel];
  dummy = *RCC_APB_ENR_USART_array[channel];

  /* Enable GPIO */
  gpio_enable_usart(port, pinTx, pinRx, idxAF);
  
  /* Configure and enable USART */
  if(channel == 1) { // On APB2 (high speed peripheral bus)
    USART_array[channel]->BRR = (uint16_t) 0x364; // 115200 baud @ 100 MHz APB2 clock and 16x oversampling
  } else { // On APB1 (low speed peripheral bus)
    USART_array[channel]->BRR = (uint16_t) 0x1B2; // 115200 baud @ 50 MHz APB1 clock and 16x oversampling
  }
  USART_array[channel]->CR1 |= USART_CR1_UE | USART_CR1_TE; // USART enable and transmitter enable

  // Dummy write, because the first byte is dropped
  USART_array[channel]->DR = 0;
  while (!(USART_array[channel]->SR & USART_SR_TC));
}

void usart_write(uint8_t channel, char c) {
    USART_array[channel]->DR = c; // put character in output register
    while (!(USART_array[channel]->SR & USART_SR_TC)); // wait for transmission complete flag
}

#ifdef __cplusplus
}
#endif /* __cplusplus */