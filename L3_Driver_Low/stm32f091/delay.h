#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern volatile uint32_t ticks;

void systick_handler();
void delay_ms(uint32_t ms);

#ifdef __cplusplus
}
#endif /* __cplusplus */
