#include <stdio.h>
#include <cstring>
#include "ISM330DHCX.h"
#include "spi.h"
#include "dma.h"
#include "delay.h"

#define SPI_MODE 3U

/* Interrupt 1 register */
#define INT1_CTRL_ADDR 0x0DU
#define INT1_DRDY_XL_Pos 0U
#define INT1_DRDY_XL_Msk (1U << INT1_DRDY_XL_Pos)
#define INT1_DRDY_XL INT1_DRDY_XL_Msk
#define INT1_DRDY_G_Pos 1U
#define INT1_DRDY_G_Msk (1U << INT1_DRDY_G_Pos)
#define INT1_DRDY_G INT1_DRDY_G_Msk

/* Interrupt 2 register */
#define INT2_CTRL_ADDR 0x0EU
#define INT2_DRDY_XL_Pos 0U
#define INT2_DRDY_XL_Msk (1U << INT2_DRDY_XL_Pos)
#define INT2_DRDY_XL INT2_DRDY_XL_Msk
#define INT2_DRDY_G_Pos 1U
#define INT2_DRDY_G_Msk (1U << INT2_DRDY_G_Pos)
#define INT2_DRDY_G INT2_DRDY_G_Msk

/* Acc control register */
#define CTRL1_XL_ADDR 0x10U
#define CTRL1_LPF2_XL_EN_Pos 1U
#define CTRL1_LPF2_XL_EN_Msk (1U << CTRL1_LPF2_XL_EN_Pos)
#define CTRL1_LPF2_XL_EN CTRL1_LPF2_XL_EN_Msk
#define CTRL1_FS_XL_Pos 2U
#define CTRL1_FS_XL_Msk (0b11U << CTRL1_FS_XL_Pos)
#define CTRL1_FS_XL CTRL1_FS_XL_Msk
#define CTRL1_ODR_XL_Pos 4U
#define CTRL1_ODR_XL_Msk (0b1111U << CTRL1_ODR_XL_Pos)
#define CTRL1_ODR_XL CTRL1_ODR_XL_Msk

/* Gyro control register */
#define CTRL2_G_ADDR 0x11U
#define CTRL2_FS_4000_G_Pos 0U
#define CTRL2_FS_4000_G_Msk (1U << CTRL2_FS_4000_G_Pos)
#define CTRL2_FS_4000_G CTRL2_FS_4000_G_Msk
#define CTRL2_FS_125_G_Pos 1U
#define CTRL2_FS_125_G_Msk (1U << CTRL2_FS_125_G_Pos)
#define CTRL2_FS_125_G CTRL2_FS_125_G_Msk
#define CTRL2_FS_G_Pos 3U
#define CTRL2_FS_G_Msk (0b11U << CTRL2_FS_G_Pos)
#define CTRL2_FS_G CTRL2_FS_G_Msk
#define CTRL2_ODR_G_Pos 4U
#define CTRL2_ODR_G_Msk (0b1111U << CTRL2_ODR_G_Pos)
#define CTRL2_ODR_G CTRL2_ODR_G_Msk

/* Control register 3 */
#define CTRL3_C_ADDR 0x12U
#define CTRL3_IF_INC_Pos 2U
#define CTRL3_IF_INC_Msk (1U << CTRL3_IF_INC_Pos)
#define CTRL3_IF_INC CTRL3_IF_INC_Msk
#define CTRL3_SIM_Pos 3U
#define CTRL3_SIM_Msk (1U << CTRL3_SIM_Pos)
#define CTRL3_SIM CTRL3_SIM_Msk

ISM330DHCX::ISM330DHCX(uint8_t spiChannel, uint8_t spiPort, uint8_t spiCS, uint8_t spiSCK, uint8_t spiMOSI,
					   uint8_t spiMISO, uint8_t spiAF, uint8_t dmaController, uint8_t dmaTxStream,
					   uint8_t dmaTxChannel, uint8_t dmaRxStream, uint8_t dmaRxChannel, uint8_t* spiRxMemPtr)
					  : spiChannel(spiChannel), spiPort(spiPort), spiCS(spiCS), spiSCK(spiSCK), spiMOSI(spiMOSI), 
						spiMISO(spiMISO), spiRxMemPtr(spiRxMemPtr), spiRxMsgCount(0U),
						dmaController(dmaController), dmaTxStream(dmaTxStream), dmaTxChannel(dmaTxChannel),
						dmaRxStream(dmaRxStream), dmaRxChannel(dmaRxChannel) {
	/* Setup SPI */
	spi_init(spiChannel, spiPort, spiCS, spiSCK, spiMOSI, spiMISO, spiAF, SPI_MODE);
	/* Setup DMA for SPI Tx (Rx setup by app using Interrupt) */
	this->spiTxPtr = new uint8_t[13];
	memset(this->spiTxPtr, 0U, 13);
	dma_config(dmaController, dmaTxStream, dmaTxChannel, Memory2Peripheral, VeryHigh, false,
			   (uint32_t)dma_periph_data_by_spi_array[spiChannel], (uint32_t)spiTxPtr, (uint16_t)13,
			   Byte, Byte, DirectMode, true, OneBeat, OneBeat, true, false, false);
	spi_enable_dma(spiChannel, true, false);
}

uint8_t ISM330DHCX::get_spi_channel() {
	return this->spiChannel;
}

uint8_t* ISM330DHCX::spi_get_Rx_ptr() {
	return this->spiRxMemPtr;
}

void ISM330DHCX::write(uint8_t registerAddress, uint8_t data) {
	/* Arrange into allocated memory */
	// RW + 7 bit Register Address (0 is write)
	uint16_t dataPacked = ((uint16_t)0b01111111 & (uint16_t)registerAddress) << 8;
	// 8 bit Data
	dataPacked |= (uint16_t)data;

	/* Write */
	spi_write_half_word(this->spiChannel, dataPacked);
}

void ISM330DHCX::read_request(uint8_t registerAddress) {
	/* Put requested address and RW bit == R in first byte of Tx memory*/
	// RW + 7 bit Register Address (1 is read)
	uint8_t dataPacked = 0b10000000U | registerAddress;
	(this->spiTxPtr)[0] = dataPacked;

	/* Enable spi */
	spi_set_data_frame_format(this->spiChannel, 8U);
	spi_enable(this->spiChannel);

	/* Write data and dummy data bytes to keep spi clock active, allowing DMA to read in data */
	dma_enable(this->dmaController, this->dmaRxStream, 13);
	dma_enable(this->dmaController, this->dmaTxStream, 13);
}

void ISM330DHCX::data_receive_complete() {
	/* Disable dma streams */
	dma_disable(this->dmaController, this->dmaRxStream);
	dma_disable(this->dmaController, this->dmaTxStream);
	/* End the transmission */
	spi_disable(this->spiChannel);
	/* Increment counter */
	(this->spiRxMsgCount)++;
}

void ISM330DHCX::data_receive_failed() {
	/* Disable dma streams */
	dma_disable(this->dmaController, this->dmaRxStream);
	dma_disable(this->dmaController, this->dmaTxStream);
	/* End the transmission */
	spi_disable(this->spiChannel);
}

void ISM330DHCX::configure() {
	/* Sequential read mode and SPI wiring (4-wire) */
	write(CTRL3_C_ADDR, CTRL3_IF_INC);
	delay_ms(100);

	/* Sensor scales and sample rates */
	// Acc max == +-2g, sample rate == 104 Hz
	write(CTRL1_XL_ADDR, (0b00U << CTRL1_FS_XL_Pos) | (0b0100 << CTRL1_ODR_XL_Pos));
	delay_ms(100);
	// Gyro max == +-125dps, sample rate == 104 Hz
	write(CTRL2_G_ADDR, CTRL2_FS_125_G | (0b0100 << CTRL2_ODR_G_Pos));
	delay_ms(100);

	/* High performance mode */
	// XL_HM_MODE = 0 in CTRL6_C (0x15) // DEFAULT
	// G_HM_MODE = 0 in CTRL7_C (0x15) // DEFAULT

	/* Filters */

	/* Interrupt signals */
	//write(INT1_CTRL_ADDR, INT1_DRDY_XL);
	//delay_ms(100);
	//write(INT2_CTRL_ADDR, INT2_DRDY_G);
	//delay_ms(100);
}

/* Read: gyro (16 bit x3), accelerometer (16 bit x3) == 12 bytes */
void ISM330DHCX::request_data_gyro_acc() {
	read_request(0x22U);
}

/* Read: temp (16 bit), gyro (16 bit x3), accelerometer (16 bit x3) == 14 bytes */
void ISM330DHCX::request_data_temp_gyro_acc() {
	read_request(0x20U);
}