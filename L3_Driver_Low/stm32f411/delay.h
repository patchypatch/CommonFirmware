#include <stdint.h>
#include "globals.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void systick_handler();
void delay_ms(uint32_t ms);

#ifdef __cplusplus
}
#endif /* __cplusplus */
