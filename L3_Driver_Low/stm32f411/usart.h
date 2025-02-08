#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void usart_init(uint8_t channel, char port, uint8_t pinTx, uint8_t pinRx, uint8_t idxAF, bool isOpenDrain);
void usart_write_byte(uint8_t channel, char c);
void usart_write(uint8_t channel, uint8_t* dataPtr, uint8_t dataLenBytes);
void usart_enable_onewire_mode(uint8_t channel);
void usart_enable_Rx(uint8_t channel);
void usart_disable_Rx(uint8_t channel);
void usart_enable_Tx(uint8_t channel);
void usart_disable_Tx(uint8_t channel);
uint8_t usart_read_byte(uint8_t channel, uint8_t* dataPtr);
uint8_t usart_read(uint8_t channel, uint8_t* dataPtr, uint8_t dataLenBytes);
void usart_enable_dma(uint8_t channel, bool isEnableTxDMA, bool isEnableRxDMA);
void usart_enable_idle_isr(uint8_t channel);
void usart_disable_idle_isr(uint8_t channel);

void usart2_handler(void);

extern void interrupt_handler_uart(uint8_t channel);

#ifdef __cplusplus
}
#endif /* __cplusplus */
