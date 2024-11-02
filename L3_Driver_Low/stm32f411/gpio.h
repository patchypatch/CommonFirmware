#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void gpio_enable(char port, uint8_t pin, bool isOutput, bool isPullUp, bool isPullDn);
void gpio_toggle(char port, uint8_t pin);
void gpio_output_high(char port, uint8_t pin);
void gpio_output_low(char port, uint8_t pin);
void gpio_enable_i2c();
void gpio_enable_usart(char port, uint8_t pinTx, uint8_t pinRx, uint8_t idxAF);
bool gpio_input_read(char port, uint8_t pin);

#ifdef __cplusplus
}
#endif /* __cplusplus */
