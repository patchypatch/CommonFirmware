#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void tim_init(uint8_t timNumber, uint32_t timDuration_ms);
void tim_reset(uint8_t timNumber);

void tim2_handler(void);
void tim3_handler(void);
void tim4_handler(void);
void tim5_handler(void);

extern void interrupt_handler_tim(uint8_t timNumber);

#ifdef __cplusplus
}
#endif /* __cplusplus */