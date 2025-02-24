#include "delay.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

volatile uint32_t ticks;

void systick_handler() {
	ticks++;
}

void delay_ms(uint32_t ms) {
	uint32_t start = ticks;
	uint32_t end = start + ms;

	if (end < start) { // overflow
		while (ticks > start); // busy wait for ticks to wrap
	}
	while (ticks < end); // busy wait for ticks to get to end
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
