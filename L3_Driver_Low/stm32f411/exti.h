#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void exti_init(char port, uint8_t pin, bool isRisingTrigger, bool isFallingTrigger);
void exti_suspend(uint8_t pin);
void exti_resume(uint8_t pin);

void exti0_handler(void);
void exti1_handler(void);
void exti2_handler(void);
void exti3_handler(void);
void exti4_handler(void); 
void exti9_5_handler(void);
void exti15_10_handler(void);

extern void interrupt_handler_exti(uint8_t pin);

#ifdef __cplusplus
}
#endif /* __cplusplus */