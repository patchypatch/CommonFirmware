#include "gpio.h"
#include "stm32f4xx.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

// Arrays by port letter index with 'A' = 0, 'B' = 1, ... 
GPIO_TypeDef* GPIO_array[] = {GPIOA, GPIOB, GPIOC, GPIOD, GPIOE};

uint8_t RCC_AHB_GPIO_Pos_array[] = {RCC_AHB1ENR_GPIOAEN_Pos, RCC_AHB1ENR_GPIOBEN_Pos,
	RCC_AHB1ENR_GPIOCEN_Pos, RCC_AHB1ENR_GPIODEN_Pos, RCC_AHB1ENR_GPIOEEN_Pos};

// Arrays by pin number
uint8_t GPIO_MODER_Pos_array[] = {GPIO_MODER_MODER0_Pos, GPIO_MODER_MODER1_Pos, GPIO_MODER_MODER2_Pos, 
	GPIO_MODER_MODER3_Pos, GPIO_MODER_MODER4_Pos, GPIO_MODER_MODER5_Pos, GPIO_MODER_MODER6_Pos,
	GPIO_MODER_MODER7_Pos, GPIO_MODER_MODER8_Pos, GPIO_MODER_MODER9_Pos, GPIO_MODER_MODER10_Pos,
	GPIO_MODER_MODER11_Pos, GPIO_MODER_MODER12_Pos, GPIO_MODER_MODER13_Pos, GPIO_MODER_MODER14_Pos,
	GPIO_MODER_MODER15_Pos};

uint8_t GPIO_PUPDR_Pos_array[] = {GPIO_PUPDR_PUPD0_Pos, GPIO_PUPDR_PUPD1_Pos, GPIO_PUPDR_PUPD2_Pos,
	GPIO_PUPDR_PUPD3_Pos, GPIO_PUPDR_PUPD4_Pos, GPIO_PUPDR_PUPD5_Pos, GPIO_PUPDR_PUPD6_Pos,
	GPIO_PUPDR_PUPD7_Pos, GPIO_PUPDR_PUPD8_Pos, GPIO_PUPDR_PUPD9_Pos, GPIO_PUPDR_PUPD10_Pos,
	GPIO_PUPDR_PUPD11_Pos, GPIO_PUPDR_PUPD12_Pos, GPIO_PUPDR_PUPD13_Pos, GPIO_PUPDR_PUPD14_Pos,
	GPIO_PUPDR_PUPD15_Pos};

uint8_t GPIO_AFR_Pos_array[] = {GPIO_AFRL_AFSEL0_Pos, GPIO_AFRL_AFSEL1_Pos, GPIO_AFRL_AFSEL2_Pos,
	GPIO_AFRL_AFSEL3_Pos, GPIO_AFRL_AFSEL4_Pos, GPIO_AFRL_AFSEL5_Pos, GPIO_AFRL_AFSEL6_Pos,
	GPIO_AFRL_AFSEL7_Pos, GPIO_AFRH_AFSEL8_Pos, GPIO_AFRH_AFSEL9_Pos, GPIO_AFRH_AFSEL10_Pos,
	GPIO_AFRH_AFSEL11_Pos, GPIO_AFRH_AFSEL12_Pos, GPIO_AFRH_AFSEL13_Pos, GPIO_AFRH_AFSEL14_Pos,
	GPIO_AFRH_AFSEL15_Pos};

void gpio_enable(char port, uint8_t pin, bool isOutput, bool isPullUp, bool isPullDn) {
	/* Determine GPIO port index */
	uint8_t portIdx = port - 'A';

	/* Enable port clock */
	RCC->AHB1ENR |= (1 << RCC_AHB_GPIO_Pos_array[portIdx]);
	
	/* do two dummy reads after enabling the peripheral clock, as per the errata */
	volatile uint32_t dummy;
	dummy = RCC->AHB1ENR;
	dummy = RCC->AHB1ENR;
	
	/* Set to output or input */
	uint32_t* modeRegister = (uint32_t*) (AHB1PERIPH_BASE + 0x0400UL * ((uint32_t)port - (uint32_t)'A'));
	uint32_t pinShift = 2 * pin;
	if( isOutput ) {
		*modeRegister &= ~( 0b11UL << pinShift ); // clear mode bits
		*modeRegister |= ( 0b01UL << pinShift );
	} else {
		*modeRegister &= ~( 0b11UL << pinShift ); // clear mode bits
		//for(;;) printf("modeRegister = 0x%8x\nmodeRegisterValue = 0x%8x\n", modeRegister, *modeRegister);
	}

	/* Configure Pull Up/Dn Resistor */
	if(isPullUp) { // Pull-up
		GPIO_array[portIdx]->PUPDR |= (0b01UL << GPIO_PUPDR_Pos_array[pin]);
	} else if(isPullDn) { // Pull-down
		GPIO_array[portIdx]->PUPDR |= (0b10UL << GPIO_PUPDR_Pos_array[pin]);
	} else { // No Pull-up, Pull-down
		GPIO_array[portIdx]->PUPDR |= (0b00UL << GPIO_PUPDR_Pos_array[pin]);
	}
}

void gpio_toggle(char port, uint8_t pin) {
	uint8_t portIdx = port - 'A';
	GPIO_array[portIdx]->ODR ^= (1 << pin);
}

void gpio_output_high(char port, uint8_t pin) {
	uint8_t portIdx = port - 'A';
	GPIO_array[portIdx]->ODR |= (1 << pin);
}

void gpio_output_low(char port, uint8_t pin) {
	uint8_t portIdx = port - 'A';
	GPIO_array[portIdx]->ODR &= ~(1 << pin);
}

void gpio_enable_i2c() {
	/* Enable GPIOB clock */
	RCC->AHB1ENR |= (1 << RCC_AHB1ENR_GPIOBEN_Pos);
	// do two dummy reads after enabling the peripheral clock, as per the errata
	volatile uint32_t dummy;
	dummy = RCC->AHB1ENR;
	dummy = RCC->AHB1ENR;
	
	/* Set PB6 and PB7 mode to alternate function */
	GPIOB->MODER &= ~(GPIO_MODER_MODE6_Msk | GPIO_MODER_MODE7_Msk); // clear
	GPIOB->MODER |= (0b10 << GPIO_MODER_MODE6_Pos) | (0b10 << GPIO_MODER_MODE7_Pos);
		// set to AF

  	/* Attach I2C1 to PB6 and PB7 via AF4 (found in datasheet) */
	GPIOB->AFR[0] &= ~(GPIO_AFRL_AFRL6 | GPIO_AFRL_AFRL7); // clear alternate function
	GPIOB->AFR[0] |= (4 << GPIO_AFRL_AFSEL6_Pos) | (4 << GPIO_AFRL_AFSEL7_Pos);
		// set alternate functions to AF4
	
	/* Configure GPIO to for PB6 and PB7 to be open drain */
	GPIOB->OTYPER |= (1 << GPIO_OTYPER_OT6_Pos) | (1 << GPIO_OTYPER_OT7_Pos);
		// set to open drain
}

void gpio_enable_usart(char port, uint8_t pinTx, uint8_t pinRx, uint8_t idxAF) {
	/* Determing GPIO port index */
	uint8_t portIdx = port - 'A';

	/* Enable GPIO clock */
	RCC->AHB1ENR |= (1UL << RCC_AHB_GPIO_Pos_array[portIdx]);
	// do two dummy reads after enabling the peripheral clock, as per the errata
	volatile uint32_t dummy;
	dummy = RCC->AHB1ENR;
	dummy = RCC->AHB1ENR;

	/* Set alternate function */
	if(pinTx <= 15) {
		GPIO_array[portIdx]->MODER |= (1UL << (GPIO_MODER_Pos_array[pinTx] + 1)); // 1st bit = 1
		GPIO_array[portIdx]->MODER &= ~(1UL << GPIO_MODER_Pos_array[pinTx]); // 2nd bit = 0
		
		uint8_t afrHighLowTx = 0;
		if(pinTx > 7) afrHighLowTx = 1;
		GPIO_array[portIdx]->AFR[afrHighLowTx] &= ~(0b1111UL << GPIO_AFR_Pos_array[pinTx]); // clear
		GPIO_array[portIdx]->AFR[afrHighLowTx] |= ((uint32_t)idxAF << GPIO_AFR_Pos_array[pinTx]); // set		
	}
	if(pinRx <= 15) {
		GPIO_array[portIdx]->MODER |= (1UL << (GPIO_MODER_Pos_array[pinRx] + 1)); // 1st bit = 1
		GPIO_array[portIdx]->MODER &= ~(1UL << GPIO_MODER_Pos_array[pinRx]); // 2nd bit = 0
		
		uint8_t afrHighLowRx = 0;
		if(pinRx > 7) afrHighLowRx = 1;
		GPIO_array[portIdx]->AFR[afrHighLowRx] &= ~(0b1111UL << GPIO_AFR_Pos_array[pinRx]); // clear
		GPIO_array[portIdx]->AFR[afrHighLowRx] |= ((uint32_t)idxAF << GPIO_AFR_Pos_array[pinRx]); // set		
	}
}

bool gpio_input_read(char port, uint8_t pin) {
	return (GPIOA->IDR & GPIO_IDR_ID8_Msk);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
