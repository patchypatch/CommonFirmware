#include <stdio.h>
#include <math.h>
#include "mcp9808.h"
#include "i2c.h"

MCP9808::MCP9808(uint8_t i2c_sid, uint8_t i2c_channel, uint8_t i2c_pin_sda, uint8_t i2c_pin_scl) {
	this->i2c_sid = i2c_sid;
	this->i2c_channel = i2c_channel;
	this->i2c_pin_sda = i2c_pin_sda;
	this->i2c_pin_scl = i2c_pin_scl;
	i2c_init_sm(i2c_channel, i2c_pin_sda, i2c_pin_scl);
}

void MCP9808::readT() {
	uint16_t unparsedT;
	float parsedT = 0;
	
	i2c_writeAddr(this->i2c_channel, this->i2c_sid, MCP9808_TEMP);
	unparsedT = i2c_read16(this->i2c_channel, this->i2c_sid);
	
	printf("unparsedT=0x%04x\n", unparsedT);
	
	/* Convert to float based on manufacturer's encoding */
	for (int i = -4; i < 8; i++) {
		parsedT += (bool)( (1 << i+4) & unparsedT ) * pow((float)2,(float)i);
	}
	if (unparsedT & MCP9808_TEMP_SIGN_MSK) { // T < 0
		parsedT *= -1;
	}
	
	this->t = parsedT;
}

float MCP9808::getT() {
	readT();
	return t;
}
