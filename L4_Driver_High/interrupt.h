#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void interrupt_mask_global(void);
void interrupt_unmask_global(void);

void init_interrupt_dma_uart_Rx_8ByteMsgs(uint8_t dma, uint8_t stream, uint8_t channel,
										  uint8_t uartChannel, uint8_t* memPtr, void(*pf_handler)(void));
void init_interrupt_dma_spi_Rx_6HalfWordMsgs(uint8_t dma, uint8_t stream, uint8_t channel,
											 uint8_t spiChannel, uint8_t* memPtr, void(*pf_handler)(void));
void interrupt_handler_dma(uint8_t dma, uint8_t stream);

void interrupt_handler_uart(uint8_t channel);

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