#include <stdint.h>

class RPLIDAR_A1M8 {
	public:
	/* public data */
	uint8_t dmaController;
	uint8_t dmaTxStream;
	uint8_t dmaTxChannel;
	uint8_t dmaRxStream;
	uint8_t dmaRxChannel;

	/* public member functions */
	RPLIDAR_A1M8(uint8_t uartChannel, uint8_t uartPort, uint8_t uartPinTx, uint8_t uartPinRx, uint8_t uartAF,
				 uint8_t dmaController, uint8_t dmaTxStream, uint8_t dmaTxChannel,
				 uint8_t dmaRxStream, uint8_t dmaRxChannel, uint8_t* uartRxMemPtr);
	void configure();
	void data_request_get_lidar_conf();
	void data_receive_complete();
	void data_receive_failed();
	uint8_t* get_uart_Rx_ptr();
	uint8_t get_uart_channel();

	private:
	/* private data */
	uint8_t uartChannel;
	uint8_t uartPort;
	uint8_t uartPinTx;
	uint8_t uartPinRx;
	uint8_t* uartTxMemPtr;
	uint8_t* uartRxMemPtr;
	uint8_t uartRxMsgCount;

	/* private member functions */
	void write(uint8_t registerAddress, uint8_t data);
	void read_request(uint8_t registerAddress);
};