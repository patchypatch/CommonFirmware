#include "sysclock.h"
#include "stm32f0xx.h"

#define SYS_TICK_CLOCK_DIVIDER 48000 // Fclk divided by 1000 to give an interrupt every ms

void clock_init() {
	/* By default HSI (8 MHz RC oscillator) is selected as system clock.
	 * Enable the PLL and set PPLMUL to get FCLK and PCLK to 48 Mhz
	 */

	/* Configure flash controler for 48Mhz */
	FLASH->ACR |= (0b001 << FLASH_ACR_LATENCY_Pos);

	/* Switch to HSI if PLL is in use */
	if((RCC->CFGR & RCC_CFGR_SWS) == RCC_CFGR_SWS_PLL) { // If PLL is system clock
		RCC->CFGR &= (uint32_t) (~RCC_CFGR_SW); // Set HSI as system clock
		while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI); // Wait for HSI switched
	}

	/* Disable PLL */
	RCC->CR &= (uint32_t)(~RCC_CR_PLLON);
	while((RCC->CR & RCC_CR_PLLRDY) != 0) // Wait for PLL off

	/* Set PPLMUL to 12 to get 48 Mhz since PLLDIV is 2 by default
	 * 8 Mhz * 12 / 2 = 48 Mhz
	*/
	RCC->CFGR &= ~RCC_CFGR_PLLMUL_Msk; // clear
	RCC->CFGR |= RCC_CFGR_PLLMUL12; // set

	/* Enable PLL */
	RCC->CR |= RCC_CR_PLLON_Msk;
	while((RCC->CR & RCC_CR_PLLRDY) == 0);

	/* Set SW (system clock switch) to use PLLCLK */
	RCC->CFGR |= RCC_CFGR_SW_PLL << RCC_CFGR_SW_Pos;
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);


//	/* Enable HSI48 and wait until HSI48 clock is stable */
//	RCC->CR2 |= RCC_CR2_HSI48ON_Msk;
//	while(!(RCC->CR2 & RCC_CR2_HSI48RDY_Msk == RCC_CR2_HSI48RDY_Msk));

//	/* Set SW (System clock switch) to use HSI48 and wait until in use */
//	RCC->CFGR |= RCC_CFGR_SW_HSI48;
//	while(!(RCC->CFGR & RCC_CFGR_SWS_HSI48 == RCC_CFGR_SWS_HSI48));

//	/* Set PREDIV to divide by 2 */
//	RCC->CFGR2 |= RCC_CFGR2_PREDIV_DIV2;

//	/* Enable the HSI48 clock as source to PLL */
//	RCC->CFGR |= 0b11UL << RCC_CFGR_PLLSRC_Pos;
}

void systick_init() {
	/* Turn on SysTick interrupt */
	SysTick_Config(SYS_TICK_CLOCK_DIVIDER);
}