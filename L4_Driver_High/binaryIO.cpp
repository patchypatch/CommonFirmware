#include "binaryIO.h"
#include "gpio.h"

BinaryIO::BinaryIO(char port, uint8_t pin, bool isOutput, bool isPullUp, bool isPullDn) {
	/* Store data */
	this->port = port;
	this->pin = pin;
	this->isOutput = isOutput;
	this->isPullUp = isPullUp;
	this->isPullDn = isPullDn;
	/* enable port/pin as input/output */
	this->enable();
}

void BinaryIO::enable() {
	gpio_enable(this->port, this->pin, this->isOutput, this->isPullUp, this->isPullDn);
}

void BinaryIO::toggle() {
	gpio_toggle(this->port, this->pin); 
}

void BinaryIO::on() {
	gpio_output_high(this->port, this->pin);
}

void BinaryIO::off() {
	gpio_output_low(this->port, this->pin);
}

bool BinaryIO::getState() {
	return gpio_input_read(this->port, this->pin);
}