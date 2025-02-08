#include "sysclock.h"
#include "stm32f4xx.h"

#define SYS_TICK_CLOCK_DIVIDER 100000 // Fclk divided by 1000 to give an interrupt every ms

void clock_init() {
	/* By default HSI (16 MHz RC oscillator) is selected as system clock.
	 * We want to use the HSE (25 MHz Osc on BlackPill)
	 * through the PLL to get 100 MHz (max) system clock.
	 */ 

	// Enable power controller and set voltage scale mode 1
	RCC->APB1ENR |= RCC_APB1ENR_PWREN_Msk;
	volatile uint32_t dummy;
	dummy = RCC->APB1ENR;
	dummy = RCC->APB1ENR;
	PWR->CR |= (0b11 << PWR_CR_VOS_Pos);

	// Configure flash controller for 3V3 supply and 100 MHz -> 3 wait states
	FLASH->ACR |= FLASH_ACR_LATENCY_3WS;

	// Enable HSE (external oscillator)
	RCC->CR |= RCC_CR_HSEON_Msk;
	while (!(RCC->CR & RCC_CR_HSERDY_Msk));

	// Configure PLL dividers and multiplier
	/* Input to PLL should be 1-2 MHz (preferably 2 MHz). Choosing M=15 gives
	 * us 25 MHz / 15 = 1.6667 MHz.
	 * The output of the PLL should be 100-438 MHz, so setting the feedback
	 * multiplier to 240 gives us 1.6667 MHz * 200 = 400 MHz.
	 * The system clock should be 100 MHz. Choosing P=4 gives us
	 * 400 MHz / 4 = 100 MHz
	 * 
	 * Since APB1 clock must not be more than 50 MHz, set the PPRE1 divider to 2.
	 * Since errata states that SPI may not work correctly at 100 MHz, use APB2 at 50 MHz. Set PPRE2 divider to 2.
	 */
	// Clear PLLM, PLLN and PLLP bits
	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLM_Msk | RCC_PLLCFGR_PLLN_Msk | RCC_PLLCFGR_PLLP_Msk);

	// Set PLLM, PLLN and PLLP, and select HSE as PLL source
	RCC->PLLCFGR |= ((15 << RCC_PLLCFGR_PLLM_Pos) | (240 << RCC_PLLCFGR_PLLN_Pos) | (1 << RCC_PLLCFGR_PLLP_Pos) | (1 << RCC_PLLCFGR_PLLSRC_Pos));

	// Set APB prescalers to 2
	RCC->CFGR |= (0b100 << RCC_CFGR_PPRE1_Pos);
	RCC->CFGR |= (0b100 << RCC_CFGR_PPRE2_Pos);

	// Enable PLL and wait for ready
	RCC->CR |= RCC_CR_PLLON_Msk;
	while (! (RCC->CR & RCC_CR_PLLRDY_Msk));

	// Select PLL output as system clock
	RCC->CFGR |= (RCC_CFGR_SW_PLL << RCC_CFGR_SW_Pos);
	while (! (RCC->CFGR & RCC_CFGR_SWS_PLL));
}

void systick_init() {
	/* Turn on SysTick interrupt */
	SysTick_Config(SYS_TICK_CLOCK_DIVIDER);
}