#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void usart_init(uint8_t channel, char port, uint8_t pinTx, uint8_t pinRx, uint8_t idxAF);
void usart_write(uint8_t channel, char c);

#ifdef __cplusplus
}
#endif /* __cplusplus */
