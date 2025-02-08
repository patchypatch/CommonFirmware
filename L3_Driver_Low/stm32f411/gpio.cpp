#include "gpio.h"
#include "rcc.h"
#include "stm32f4xx.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

// Arrays by port letter index with 'A' = 0, 'B' = 1, ... 
GPIO_TypeDef* GPIO_array[] = {GPIOA, GPIOB, GPIOC, GPIOD, GPIOE};

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

uint8_t GPIO_OTYPER_Pos_array[] = {GPIO_OTYPER_OT0_Pos, GPIO_OTYPER_OT1_Pos, GPIO_OTYPER_OT2_Pos,
	GPIO_OTYPER_OT3_Pos, GPIO_OTYPER_OT4_Pos, GPIO_OTYPER_OT5_Pos, GPIO_OTYPER_OT6_Pos,
	GPIO_OTYPER_OT7_Pos, GPIO_OTYPER_OT8_Pos, GPIO_OTYPER_OT9_Pos, GPIO_OTYPER_OT10_Pos,
	GPIO_OTYPER_OT11_Pos, GPIO_OTYPER_OT12_Pos, GPIO_OTYPER_OT13_Pos, GPIO_OTYPER_OT14_Pos,
	GPIO_OTYPER_OT15_Pos};

uint8_t GPIO_OSPEEDR_Pos_array[] = {GPIO_OSPEEDR_OSPEED0_Pos, GPIO_OSPEEDR_OSPEED1_Pos, GPIO_OSPEEDR_OSPEED2_Pos,
	GPIO_OSPEEDR_OSPEED3_Pos, GPIO_OSPEEDR_OSPEED4_Pos, GPIO_OSPEEDR_OSPEED5_Pos, GPIO_OSPEEDR_OSPEED6_Pos,
	GPIO_OSPEEDR_OSPEED7_Pos, GPIO_OSPEEDR_OSPEED8_Pos, GPIO_OSPEEDR_OSPEED9_Pos, GPIO_OSPEEDR_OSPEED10_Pos,
	GPIO_OSPEEDR_OSPEED11_Pos, GPIO_OSPEEDR_OSPEED12_Pos, GPIO_OSPEEDR_OSPEED13_Pos, GPIO_OSPEEDR_OSPEED14_Pos,
	GPIO_OSPEEDR_OSPEED15_Pos};

void gpio_set_alt_fcn(uint8_t portIdx, uint8_t pin, uint8_t idxAF) {
	if(pin <= 15) {
		GPIO_array[portIdx]->MODER |= (1UL << (GPIO_MODER_Pos_array[pin] + 1)); // 1st bit = 1
		GPIO_array[portIdx]->MODER &= ~(1UL << GPIO_MODER_Pos_array[pin]); // 2nd bit = 0

		uint8_t afrHighLow = 0;
		if(pin > 7) afrHighLow = 1;
		GPIO_array[portIdx]->AFR[afrHighLow] &= ~(0b1111UL << GPIO_AFR_Pos_array[pin]); // clear
		GPIO_array[portIdx]->AFR[afrHighLow] |= ((uint32_t)idxAF << GPIO_AFR_Pos_array[pin]); // set
	}
}

void gpio_enable(char port, uint8_t pin, bool isOutput, bool isPullUp, bool isPullDn, bool isOpenDrain) {
	/* Determine GPIO port index */
	uint8_t portIdx = port - 'A';

	/* Enable GPIO clock */
	rcc_enable_clock_gpio(portIdx);
	
	/* Set to output or input */
	uint32_t* modeRegister = (uint32_t*) (AHB1PERIPH_BASE + 0x0400UL * ((uint32_t)port - (uint32_t)'A'));
	uint32_t pinShift = 2 * pin;
	if( isOutput ) {
		*modeRegister &= ~( 0b11UL << pinShift ); // clear mode bits
		*modeRegister |= ( 0b01UL << pinShift );
	} else {
		*modeRegister &= ~( 0b11UL << pinShift ); // clear mode bits
	}

	/* Configure Pull Up/Dn Resistor */
	if(isPullUp) { // Pull-up
		GPIO_array[portIdx]->PUPDR |= (0b01UL << GPIO_PUPDR_Pos_array[pin]);
	} else if(isPullDn) { // Pull-down
		GPIO_array[portIdx]->PUPDR |= (0b10UL << GPIO_PUPDR_Pos_array[pin]);
	} else { // No Pull-up, Pull-down
		GPIO_array[portIdx]->PUPDR |= (0b00UL << GPIO_PUPDR_Pos_array[pin]);
	}

	/* Push-pull or open-drain (reset state is push-pull) */
	if(isOpenDrain) {
		GPIO_array[portIdx]->OTYPER |= 1 << GPIO_OTYPER_Pos_array[pin];
	} else {
		GPIO_array[portIdx]->OTYPER &= ~(1 << GPIO_OTYPER_Pos_array[pin]);
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

 //TODO: Generalize this
void gpio_enable_i2c() {
	uint8_t portIdx = 'B' - 'A';

	/* Enable GPIO clock */
	rcc_enable_clock_gpio(portIdx);
	
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

void gpio_enable_spi(char port, uint8_t pinCS, uint8_t pinSCK, uint8_t pinMOSI, uint8_t pinMISO, uint8_t idxAF) {
	/* Determing GPIO port index */
	uint8_t portIdx = port - 'A';

	/* Enable GPIO clock */
	rcc_enable_clock_gpio(portIdx);

	/* Set alternate function */
	gpio_set_alt_fcn(portIdx, pinCS, idxAF);
	gpio_set_alt_fcn(portIdx, pinSCK, idxAF);
	gpio_set_alt_fcn(portIdx, pinMOSI, idxAF);
	gpio_set_alt_fcn(portIdx, pinMISO, idxAF);

	/* Set CS as open-drain */
	GPIO_array[portIdx]->OTYPER |= 1 << GPIO_OTYPER_Pos_array[pinCS];
	/* Set other pins as push-pull (reset state is push-pull) */
	GPIO_array[portIdx]->OTYPER &= ~(1 << GPIO_OTYPER_Pos_array[pinSCK]);
	GPIO_array[portIdx]->OTYPER &= ~(1 << GPIO_OTYPER_Pos_array[pinMOSI]);
	GPIO_array[portIdx]->OTYPER &= ~(1 << GPIO_OTYPER_Pos_array[pinMISO]);

	/* Set CS output speed to very high */
	GPIO_array[portIdx]->OSPEEDR |= 0b11 << GPIO_OSPEEDR_Pos_array[pinCS];
}

void gpio_enable_usart(char port, uint8_t pinTx, uint8_t pinRx, uint8_t idxAF, bool isOpenDrain) {
	/* Determing GPIO port index */
	uint8_t portIdx = port - 'A';

	/* Enable GPIO clock */
	rcc_enable_clock_gpio(portIdx);

	/* Set alternate function */
	gpio_set_alt_fcn(portIdx, pinTx, idxAF);
	gpio_set_alt_fcn(portIdx, pinRx, idxAF);

	/* Push-pull or open-drain (reset state is push-pull) */
	if(isOpenDrain) {
		GPIO_array[portIdx]->OTYPER |= 1 << GPIO_OTYPER_Pos_array[pinTx];
		GPIO_array[portIdx]->OTYPER |= 1 << GPIO_OTYPER_Pos_array[pinRx];
	} else {
		GPIO_array[portIdx]->OTYPER &= ~(1 << GPIO_OTYPER_Pos_array[pinTx]);
		GPIO_array[portIdx]->OTYPER &= ~(1 << GPIO_OTYPER_Pos_array[pinRx]);
	}
}

bool gpio_input_read(char port, uint8_t pin) {
	return (GPIOA->IDR & GPIO_IDR_ID8_Msk);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
