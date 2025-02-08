#pragma once
#include <stdint.h>

class BinaryIO {
	public:
	/* public data */
	
	/* public member functions */
	BinaryIO(char port, uint8_t pin, bool isOutput, bool isPullUp, bool isPullDn, bool isOpenDrain);
	void toggle();
	void output_high();
	void output_low();
	bool getState();

	private:
	/* private data */
	char port;
	uint8_t pin;
	bool isOutput;
	bool isPullUp;
	bool isPullDn;
	bool isOpenDrain;

	/* private member functions */
	void enable();
};

