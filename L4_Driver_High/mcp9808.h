#include <stdint.h>

#define MCP9808_CONFIG 0x01
#define MCP9808_T_UPPER 0x02
#define MCP9808_T_LOWER 0x03
#define MCP9808_T_CRIT 0x04
#define MCP9808_TEMP 0x05
#define MCP9808_TEMP_TEMP_MSK 0x0FFF
#define MCP9808_TEMP_SIGN_MSK 0x1000
#define MCP9808_MANF_ID 0x06
#define MCP9808_DEV_ID 0x07
#define MCP9808_RES 0x08

class MCP9808 {
	public:
	/* public data */

	/* public member functions */
	MCP9808(uint8_t sid, uint8_t i2c_channel, uint8_t i2c_pin_sda, uint8_t i2c_pin_scl); // constructor
	float getT();

	private:
	/* private data */
	uint8_t i2c_sid;
	uint8_t i2c_channel;
	uint8_t i2c_pin_scl;
	uint8_t i2c_pin_sda;
	float t; // temperature

	/* private member functions */
	void readT();
};	

