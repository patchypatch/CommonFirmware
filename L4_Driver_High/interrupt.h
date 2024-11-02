#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void interrupt_mask_global(void);
void interrupt_unmask_global(void);

void init_interrupt_exti(char port, uint8_t pin, bool isRisingTrigger, bool isFallingTrigger, void(*pf_handler)(void));
void interrupt_handler_exti(uint8_t pin);
void suspend_interrupt_exti(uint8_t pin);
void resume_interrupt_exti(uint8_t pin);

void init_interrupt_tim(uint8_t timNumber, uint32_t timDuration_ms, void(*pf_handler)(void));
void interrupt_handler_tim(uint8_t timNumber);
void reset_interrupt_tim(uint8_t timNumber);

#ifdef __cplusplus
}
#endif /* __cplusplus */