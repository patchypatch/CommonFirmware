#include <stdint.h>
#pragma once
class BinaryIO {
	public:
	/* public data */
	
	/* public member functions */
	BinaryIO(char port, uint8_t pin, bool isOutput, bool isPullUp, bool isPullDn); // constructor
	void toggle();
	void on();
	void off();
	bool getState();

	private:
	/* private data */
	char port;
	uint8_t pin;
	bool isOutput;
	bool isPullUp;
	bool isPullDn;

	/* private member functions */
	void enable();
};

