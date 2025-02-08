#include "BinaryIO.h"
#include "gpio.h"

BinaryIO::BinaryIO(char port, uint8_t pin, bool isOutput, bool isPullUp, bool isPullDn, bool isOpenDrain)
				: port(port), pin(pin), isOutput(isOutput), isPullUp(isPullUp), isPullDn(isPullDn),
				  isOpenDrain(isOpenDrain) {
	/* enable port/pin as input/output */
	this->enable();
}

void BinaryIO::enable() {
	gpio_enable(this->port, this->pin, this->isOutput, this->isPullUp, this->isPullDn, this->isOpenDrain);
}

void BinaryIO::toggle() {
	gpio_toggle(this->port, this->pin); 
}

void BinaryIO::output_high() {
	gpio_output_high(this->port, this->pin);
}

void BinaryIO::output_low() {
	gpio_output_low(this->port, this->pin);
}

bool BinaryIO::getState() {
	return gpio_input_read(this->port, this->pin);
}