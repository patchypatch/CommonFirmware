#include <stdint.h>

class ISM330DHCX {
	public:
	/* public data */
	uint8_t dmaController;
	uint8_t dmaTxStream;
	uint8_t dmaTxChannel;
	uint8_t dmaRxStream;
	uint8_t dmaRxChannel;

	/* public member functions */
	ISM330DHCX(uint8_t spiChannel, uint8_t spiPort, uint8_t spiCS, uint8_t spiSCK, uint8_t spiMOSI,
			   uint8_t spiMISO, uint8_t spiAF, uint8_t dmaController, uint8_t dmaTxStream,
			   uint8_t dmaTxChannel, uint8_t dmaRxStream, uint8_t dmaRxChannel, uint8_t* spiRxMemPtr); // constructor
	void request_data_gyro_acc();
	void request_data_temp_gyro_acc();
	void configure();
	void data_receive_complete();
	void data_receive_failed();
	uint8_t* spi_get_Rx_ptr();
	uint8_t get_spi_channel();

	private:
	/* private data */
	uint8_t spiChannel;
	uint8_t spiPort;
	uint8_t spiCS;
	uint8_t spiSCK;
	uint8_t spiMOSI;
	uint8_t spiMISO;
	uint8_t* spiRxMemPtr;
	uint8_t spiRxMsgCount;
	uint8_t* spiTxPtr;

	/* private member functions */
	void write(uint8_t registerAddress, uint8_t data);
	void read_request(uint8_t registerAddress);
};