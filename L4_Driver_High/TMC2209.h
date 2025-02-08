#include <stdint.h>

typedef enum {
	M256STEP,
	M128STEP,
	M64STEP,
	M32STEP,
	M16STEP,
	M8STEP,
	M4STEP,
	M2STEP,
	M1STEP
} MRES_Enum;

class TMC2209 {
	public:
	/* public data */
	uint8_t dmaController;
	uint8_t dmaRxStream;
	uint8_t dmaRxChannel;
	uint8_t uartChannel;

	/* public member functions */
	TMC2209(uint8_t nodeAddress, uint8_t uartChannel, char uartOnewirePort, uint8_t uartOnewirePin,
			uint8_t uartUnusedRxPin, uint8_t uartAF, uint8_t dmaController, uint8_t dmaRxStream,
			uint8_t dmaRxChannel, uint8_t* uartRxMemPtr); // constructor
	void uart_set_velocity(float rpm);
	void configure();
	uint8_t* uart_get_Rx_ptr();
	void uart_set_Rx_ptr(uint8_t* memPtr);
	void uart_req_write_count();
	void data_receive_complete();
	void data_receive_failed();
	uint8_t get_general_status();
	uint32_t get_drive_status();
	uint32_t get_256microstep_time();
	void clear_status();
	void set_microstep_res(MRES_Enum res);
	uint16_t get_input_pin_state();

	private:
	/* private data */
	uint8_t nodeAddress;
	uint8_t uartOnewirePort;
	uint8_t uartOnewirePin;
	uint8_t* uartRxMemPtr;
	uint8_t uartRxMsgCount;
	uint16_t microStepRes;

	/* private member functions */
	void write(uint8_t registerAddress, uint32_t data);
	uint8_t read_request(uint8_t registerAddress);
	uint8_t read(uint8_t registerAddress, uint32_t* data);
	void calc_CRC(uint8_t* datagram, uint8_t length);
	void handle_stepper_uart_Rx(void);
	uint16_t mstep_enum_2_msteps_per_step(MRES_Enum res);
};
