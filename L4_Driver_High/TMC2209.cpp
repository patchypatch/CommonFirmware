#include <stdio.h>
#include <math.h>
#include "TMC2209.h"
#include "interrupt.h"
#include "usart.h"
#include "dma.h"
#include "delay.h"

#define SYNC_BYTE 0b00000101
#define WRITE_LENGTH 8U
#define READ_REQ_LENGTH 4U
#define READ_REPLY_LENGTH 8U

/* General Configuration Registers */
#define GCONF_ADDR 0x00
#define GSTAT_ADDR 0x01
#define IFCNT_ADDR 0x02
#define IOIN_ADDR 0x06

/* Velocity Dependent Control Registers */
#define IHOLD_IRUN_ADDR 0x10
#define TPOWERDOWN_ADDR 0x11
#define TSTEP_ADDR 0x12
#define TPWMTHRS_ADDR 0x13
#define VACTUAL_ADDR 0x22

/* Chopper Control Registers */
#define CHOPCONF_ADDR 0x6C
#define MRES_Pos (24U)
#define MRES_Msk (0b1111UL << MRES_Pos)
#define MRES MRES_Msk
#define DRV_STATUS_ADDR 0x6F

/* Creates TMC2209 object
 * @param node_address - The node address of the TMC2209 chip. Permissible values: 0, 1, 2, 3. */
TMC2209::TMC2209(uint8_t nodeAddress, uint8_t uartChannel, char uartOnewirePort, uint8_t uartOnewirePin,
				 uint8_t uartUnusedRxPin, uint8_t uartAF, uint8_t dmaController, uint8_t dmaRxStream,
				 uint8_t dmaRxChannel, uint8_t* uartRxMemPtr)
				: nodeAddress(nodeAddress), uartChannel(uartChannel), uartOnewirePort(uartOnewirePort),
				uartOnewirePin(uartOnewirePin), uartRxMemPtr(uartRxMemPtr), uartRxMsgCount(0U),
				dmaController(dmaController), dmaRxStream(dmaRxStream), dmaRxChannel(dmaRxChannel),
				microStepRes((uint16_t)256) {
	usart_init(uartChannel, uartOnewirePort, uartOnewirePin, uartUnusedRxPin, uartAF, true);
	usart_disable_Rx(uartChannel); // disable to avoid collisions (only enable after Rx request)
	usart_enable_onewire_mode(uartChannel);
}

void TMC2209::uart_set_Rx_ptr(uint8_t* memPtr) {
	this->uartRxMemPtr = memPtr;
}

uint8_t* TMC2209::uart_get_Rx_ptr() {
	return this->uartRxMemPtr;
}

void TMC2209::write(uint8_t registerAddress, uint32_t data) {
	/* Allocate memory */
	uint8_t* dataPtr = new uint8_t[WRITE_LENGTH];

	/* Arrange into allocated memory */
	// Sync Byte
	dataPtr[0] = SYNC_BYTE;
	// Node Address
	dataPtr[1] = this->nodeAddress;
	// RW + 7 bit Register Address
	dataPtr[2] = 0b10000000 | registerAddress;
	// 32 bit Data Word
	dataPtr[3] = (uint8_t) (data >> 24); // MSB
	dataPtr[4] = (uint8_t) (data >> 16);
	dataPtr[5] = (uint8_t) (data >> 8);
	dataPtr[6] = (uint8_t) data; // LSB
	// CRC
	calc_CRC(dataPtr, WRITE_LENGTH);

	/* Write */
	usart_write(this->uartChannel, dataPtr, WRITE_LENGTH);

	/* Free memory */
	delete dataPtr;
}

uint8_t TMC2209::read_request(uint8_t registerAddress) {
	/* Allocate memory */
	uint8_t* dataPtr = new uint8_t[READ_REQ_LENGTH];

	/* Arrange into allocated memory */
	// Sync Byte
	dataPtr[0] = SYNC_BYTE;
	// Node Address
	dataPtr[1] = this->nodeAddress;
	// RW + 7 bit Register Address
	dataPtr[2] = 0b01111111 & registerAddress;
	// CRC
	calc_CRC(dataPtr, READ_REQ_LENGTH);

	/* Write */
	usart_write(this->uartChannel, dataPtr, READ_REQ_LENGTH);

	/* Free memory */
	delete dataPtr;

	/* Enable Rx */
	usart_enable_Rx(this->uartChannel);

	//TODO: check CRC
	return 0;
}

uint8_t TMC2209::read(uint8_t registerAddress, uint32_t* data) {
	bool isMsgRxSuccessful = false;
	while (!isMsgRxSuccessful) {
		uint8_t prevRxMsgCount = this->uartRxMsgCount;
		/* Send read request */
		read_request(registerAddress);
		delay_ms(5);
		/* Ensure response was received */
		if (prevRxMsgCount == this->uartRxMsgCount) {
			// Message not received!
			data_receive_failed();
		} else { // Message received!
			// Save data
			*data = (uint32_t)(this->uartRxMemPtr[3]) << 24;
			*data |= (uint32_t)(this->uartRxMemPtr[4]) << 16;
			*data |= (uint32_t)(this->uartRxMemPtr[5]) << 8;
			*data |= (uint32_t)(this->uartRxMemPtr[6]);
			isMsgRxSuccessful = true;
		}
	}
	return 0;
}

void TMC2209::data_receive_complete() {
	/* Disable Rx */
	usart_disable_Rx(this->uartChannel);
	/* Increment counter */
	(this->uartRxMsgCount)++;
}

void TMC2209::data_receive_failed() {
	/* Disable Rx */
	usart_disable_Rx(this->uartChannel);
}

/* 8 bit CRC calculation with polynomial: x^8 + x^2 + x + 1
 * (from TMC2209 datasheet example)
 */
void TMC2209::calc_CRC(uint8_t* datagram, uint8_t length) {
	uint8_t* crc = datagram + (length - 1);
	uint8_t currentByte;
	*crc = 0;
	for (int i=0; i<(length-1); i++) {
		currentByte = datagram[i];
		for (int j=0; j<8; j++) {
			if ((*crc >> 7) ^ (currentByte&0x01)) {
				*crc = (*crc << 1) ^ 0x07;
			} else {
				*crc = (*crc << 1);
			}
			currentByte = currentByte >> 1;
		}
	}
}

/* Set velocity using internal step generator
 * @param cmd +-(2^23)-2 [usteps / t]
 */
void TMC2209::uart_set_velocity(float rpm) {
	float microSteps = (float)this->microStepRes;
	float stepsPerRev = 200;
	uint32_t velocitySetpoint = (uint32_t)(microSteps*stepsPerRev*abs(rpm)/60.0f/0.715f);
	if (rpm < 0) {
		velocitySetpoint = ~velocitySetpoint;
		velocitySetpoint++;
		velocitySetpoint &= ~0xFF000000;
	}
	write(VACTUAL_ADDR, velocitySetpoint);
}

void TMC2209::configure() {
	bool isConfigSuccessful = false;
	uint32_t config = 0b11000111UL;
	uint32_t data;
	while (!isConfigSuccessful) {
		/* Send config */
		write(GCONF_ADDR, config);
		delay_ms(10);

		/* Verify config */
		read(GCONF_ADDR, &data);
		if (data == config) isConfigSuccessful = true;
	}
}

void TMC2209::uart_req_write_count() {
	/* Send read request */
	read_request(IFCNT_ADDR);
}

uint8_t TMC2209::get_general_status() {
	uint32_t data;
	read(GSTAT_ADDR, &data);
	return (uint8_t)data;
}

uint32_t TMC2209::get_drive_status() {
	uint32_t data;
	read(DRV_STATUS_ADDR, &data);
	return data;
}

uint32_t TMC2209::get_256microstep_time() {
	uint32_t data;
	read(TSTEP_ADDR, &data);
	return data;
}

void TMC2209::clear_status() {
	/* Send clear flags message (flags will not clear if active) */
	write(GSTAT_ADDR, (uint32_t)0b111);
	delay_ms( 10 );
}

void TMC2209::set_microstep_res(MRES_Enum res) {
	bool isConfigSuccessful = false;
	uint32_t config;
	uint32_t prevConfig;
	uint32_t data;
	while (!isConfigSuccessful) {
		/* Get current config */
		read(CHOPCONF_ADDR, &prevConfig);

		/* Send modified config */
		config = prevConfig | ((uint32_t)res << MRES_Pos);
		write(CHOPCONF_ADDR, config);
		delay_ms(10);

		/* Verify config */
		read(CHOPCONF_ADDR, &data);
		if (data == config) isConfigSuccessful = true;
	}
	/* Save resolution */
	this->microStepRes = mstep_enum_2_msteps_per_step(res);
}

uint16_t TMC2209::get_input_pin_state() {
	uint32_t inputPinState;
	read(IOIN_ADDR, &inputPinState);
	return (uint16_t)inputPinState;
}

uint16_t TMC2209::mstep_enum_2_msteps_per_step(MRES_Enum res) {
	switch (res) {
		case M256STEP:
			return 256;
		case M128STEP:
			return 128;
		case M64STEP:
			return 64;
		case M32STEP:
			return 32;
		case M16STEP:
			return 16;
		case M8STEP:
			return 8;
		case M4STEP:
			return 4;
		case M2STEP:
			return 2;
		case M1STEP:
			return 1;
		default:
			return 0;
	}
}
