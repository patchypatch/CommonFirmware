#include "RPLIDAR_A1M8.h"
#include "usart.h"

RPLIDAR_A1M8::RPLIDAR_A1M8(uint8_t uartChannel, uint8_t uartPort, uint8_t uartPinTx, uint8_t uartPinRx, uint8_t uartAF,
						   uint8_t dmaController, uint8_t dmaTxStream, uint8_t dmaTxChannel,
						   uint8_t dmaRxStream, uint8_t dmaRxChannel, uint8_t* uartRxMemPtr)
						  : uartChannel(uartChannel), uartPort(uartPort), uartPinTx(uartPinTx), uartPinRx(uartPinRx),
							dmaController(dmaController), dmaTxStream(dmaTxStream), uartRxMemPtr(uartRxMemPtr) {
	/* Setup UART */
	usart_init(uartChannel, uartPort, uartPinTx, uartPinRx, uartAF, false);

	/* Setup DMA for UART Tx */

	/* Setup DMA for UART Rx */
}

void RPLIDAR_A1M8::configure();
void RPLIDAR_A1M8::data_request_get_lidar_conf();
void RPLIDAR_A1M8::data_receive_complete();
void RPLIDAR_A1M8::data_receive_failed();
uint8_t* RPLIDAR_A1M8::get_uart_Rx_ptr();
uint8_t RPLIDAR_A1M8::get_uart_channel();

void RPLIDAR_A1M8::write(uint8_t registerAddress, uint8_t data);
void RPLIDAR_A1M8::read_request(uint8_t registerAddress);
