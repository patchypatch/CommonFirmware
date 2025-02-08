#include <stdio.h>
#include "i2c.h"
#include "gpio.h"
#include "rcc.h"

// PB6 = SCL1
// PB7 = SDA1
/* Sm = 100kHz */
void i2c_init_sm(uint8_t i2c_channel, uint8_t i2c_pin_sda, uint8_t i2c_pin_scl) {	
	/* Enables */
	rcc_enable_clock_i2c(i2c_channel);
	gpio_enable_i2c();

	/* Reset I2C1 */
	I2C1->CR1 = I2C_CR1_SWRST; // reset state
	I2C1->CR1 &= ~I2C_CR1_SWRST_Msk; // clear reset bit
	
	/* Program the peripheral input clock in I2C_CR2 Register */
	I2C1->CR2 &= ~(I2C_CR2_FREQ_Msk); // clear I2C peripheral's clock FREQ
	I2C1->CR2 |= (0b110010 << I2C_CR2_FREQ_Pos); // Set to 50Mhz; >=4 MHz for Fm mode
		// Multiple of 10Mhz is required for 400 kHz max Fm mode speed
		// Must match the APB clock frequency 

	/* Configure the clock control registers while I2C1 is disabled (does SCL clock control) */
	I2C1->CCR &= ~(I2C_CCR_FS_Msk); // clear FS (Sm mode)
	I2C1->CCR &= ~(I2C_CCR_CCR_Msk); // clear CCR (SCL period control)
	I2C1->CCR |= (0xFA << I2C_CCR_CCR_Pos); // set Sm mode multiplier to 250
		 // T_high = T_low =  20ns clock * CCR = 5000ns

	/* Configure the rise time register, I2C_TRISE, while I2C1 is disabled
	   Controls max duration of SCL rise in clock cycles
	   TRISE = max SCL rise time + 1 */
	I2C1->TRISE &= ~(I2C_TRISE_TRISE_Msk); // clear TRISE
	I2C1->TRISE |= (0x33 << I2C_TRISE_TRISE_Pos); // TRISE = 51 = 50 + 1 -> 1ms max rise 
		// T_clock = 20ns

	/* Program the I2C_CR1 register to enable the peripheral */
	I2C1->CR1 &= ~(I2C_CR1_PE_Msk); // clear enable
	I2C1->CR1 |= (1 << I2C_CR1_PE_Pos); // enable I2C1
}

// PB6 = SCL1
// PB7 = SDA1
/* Sm = 400kHz */
void i2c_init_fm(uint8_t i2c_channel, uint8_t i2c_pin_sda, uint8_t i2c_pin_scl) {
	/* Enables */
	rcc_enable_clock_i2c(i2c_channel);
	gpio_enable_i2c();

	/* Reset I2C1 */
	I2C1->CR1 = I2C_CR1_SWRST; // reset state
	I2C1->CR1 &= ~I2C_CR1_SWRST_Msk; // clear reset bit

	/* Program the peripheral input clock in I2C_CR2 Register */
	I2C1->CR2 &= ~(I2C_CR2_FREQ_Msk); // clear I2C peripheral's clock FREQ
	I2C1->CR2 |= (0b110010 << I2C_CR2_FREQ_Pos); // Set to 50Mhz; >=4 MHz for Fm mode
		// Multiple of 10Mhz is required for 400 kHz max Fm mode speed
		// Must match the APB clock frequency 

	/* Configure the clock control registers while I2C1 is disabled (does SCL clock control) */
	I2C1->CCR &= ~(I2C_CCR_FS_Msk); // clear FS (Sm/Fm mode select)
	I2C1->CCR |= (1 << I2C_CCR_FS_Pos); // set to Fm (I2C fast standard) mode
	I2C1->CCR &= ~(I2C_CCR_DUTY_Msk); // clear Fm mode DUTY
	I2C1->CCR |= (1 << I2C_CCR_DUTY_Pos); // set DUTY mode to 1; T_low/T_high = 16/9
	I2C1->CCR &= ~(I2C_CCR_CCR_Msk); // clear CCR (SCL period control)
	I2C1->CCR |= (5 << I2C_CCR_CCR_Pos); // set Fm mode1 multiplier to 5
		// T_high = 20ns clock * 5 * 9 = 900ns
		// T_low = 16/9 * T_high = 1600ns

	/* Configure the rise time register, I2C_TRISE, while I2C1 is disabled
	   Controls max duration of SCL rise in clock cycles
	   TRISE = max SCL rise time + 1 */
	I2C1->TRISE &= ~(I2C_TRISE_TRISE_Msk); // clear TRISE
	I2C1->TRISE |= (4 << I2C_TRISE_TRISE_Pos); // set to 4 = 3 + 1 for 60ns max rise time
		// T_clock = 20ns

	/* Program the I2C_CR1 register to enable the peripheral */
	I2C1->CR1 &= ~(I2C_CR1_PE_Msk); // clear enable
	I2C1->CR1 |= (1 << I2C_CR1_PE_Pos); // enable I2C1
}
/* Refer to Figure 164. Transfer sequence diagram for master transmitter
 *  in stm32f411xc reference manual */
// TODO: Add acknowledge checking and collision checking
void i2c_write8(uint8_t i2c_channel, uint8_t sid, uint8_t addr, uint8_t data) {
	// Wait until BUSY bit of I2C_SR2 is cleared (bus is idle)
	while( I2C1->SR2 & (1 << I2C_SR2_BUSY_Msk) );

	// S: Write START bit of I2C_CR1 (generates start condition on bus)	
	I2C1->CR1 |= (1 << I2C_CR1_START_Pos); // makes this chip a master
	
	// EV5: Wait until START is complete (SB in I2C_SR1 is set)
	while( !(I2C1->SR1 & I2C_SR1_SB_Msk) ); // wait for hardware to set the SB (start bit)

	// Address: Write the slave address (bits 1-7) and the R/W bit as write (bit 0 to 0)
	I2C1->DR = (sid << 1); // left shift also puts a 0 in bit 0

	// A: Wait until ADDR bit of I2C_SR1 register is set (slave address sent)
	while( !(I2C1->SR1 & I2C_SR1_ADDR_Msk) );

	// EV6: Clearing Sequence -- Read I2C_SR1 and I2C_SR2 to clear the ADDR bit
	I2C1->SR1;
	I2C1->SR2;

	// EV8_1: Wait until transmit data register is empty (TxE bit of I2C_SR1 register is set)
	while( !(I2C1->SR1 & I2C_SR1_TXE_Msk) );

	// Data1: Write the slave register or memory adress
	I2C1->DR = addr;

	// EV8: Wait until transmit data register is empty (TxE bit of I2C_SR1 register is set)
	while( !(I2C1->SR1 & I2C_SR1_TXE_Msk) );

 	// DataN: Write the data
	I2C1->DR = data;

	// EV8_2:  Wait until transmit data register is empty (TxE bit of I2C_SR1 register is set)
	//         Wait until BTF (byte transfer finished) register of I2C_SR1 is set
	while( !(I2C1->SR1 & I2C_SR1_TXE_Msk) );
	while( !(I2C1->SR1 & I2C_SR1_BTF_Msk) );

	// P: Write STOP bit of I2C_CR1 (generate stop condition on bus)
	I2C1->CR1 |= (1 << I2C_CR1_STOP_Pos); // frees the bus for other transmissions
}

/* Master, write register address to be read */
/* Refer to Figure 164. Transfer sequence diagram for master transmitter
 *  in stm32f411xc reference manual */
void i2c_writeAddr(uint8_t i2c_channel, uint8_t sid, uint8_t addr) {
	// Wait until BUSY bit of I2C_SR2 is cleared (bus is idle)
	while( I2C1->SR2 & (1 << I2C_SR2_BUSY_Msk) );

	// S: Write START bit of I2C_CR1 (generates start condition on bus)	
	I2C1->CR1 |= (1 << I2C_CR1_START_Pos); // makes this chip a master
	
	// EV5: Wait until START is complete (SB in I2C_SR1 is set)
	while( !(I2C1->SR1 & I2C_SR1_SB_Msk) ); // wait for hardware to set the SB (start bit)

	// Address: Write the slave address (bits 1-7) and the R/W bit as write (bit 0 to 0)
	I2C1->DR = (sid << 1); // left shift also puts a 0 in bit 0

	// A: Wait until ADDR bit of I2C_SR1 register is set (slave address sent)
	while( !(I2C1->SR1 & I2C_SR1_ADDR_Msk) );

	// EV6: Clearing Sequence -- Read I2C_SR1 and I2C_SR2 to clear the ADDR bit
	I2C1->SR1;
	I2C1->SR2;

	// EV8_1: Wait until transmit data register is empty (TxE bit of I2C_SR1 register is set)
	while( !(I2C1->SR1 & I2C_SR1_TXE_Msk) );

	// Data1: Write the slave register or memory adress
	I2C1->DR = addr;

	// EV8_2:  Wait until transmit data register is empty (TxE bit of I2C_SR1 register is set)
	//         Wait until BTF (byte transfer finished) register of I2C_SR1 is set
	while( !(I2C1->SR1 & I2C_SR1_TXE_Msk) );
	while( !(I2C1->SR1 & I2C_SR1_BTF_Msk) );

	// P: Write STOP bit of I2C_CR1 (generate stop condition on bus)
	I2C1->CR1 |= (1 << I2C_CR1_STOP_Pos); // frees the bus for other transmissions
}

/* Master receiver, fig. 165 in stm32f411xce reference manual */
uint16_t i2c_read16(uint8_t i2c_channel, uint8_t sid) {
	uint16_t data = 0;
	// Wait until BUSY bit of I2C_SR2 is cleared (bus is idle)
	while( I2C1->SR2 & (1 << I2C_SR2_BUSY_Msk) );

	// S: Write START bit of I2C_CR1 (generates start condition on bus)	
	I2C1->CR1 |= (1 << I2C_CR1_START_Pos); // makes this chip a master
	
	// EV5: Wait until START is complete (SB in I2C_SR1 is set)
	while( !(I2C1->SR1 & I2C_SR1_SB_Msk) ); // wait for hardware to set the SB (start bit)

	// Address: Write the slave address (bits 1-7) and the R/W bit as read (bit 0 to 1)
	I2C1->DR = (sid << 1) + 1; // +1 after shift puts a 1 in bit 0

	// A: Wait until ADDR bit of I2C_SR1 register is set (slave address sent)
	while( !(I2C1->SR1 & I2C_SR1_ADDR_Msk) );

	// Set ACK low, set POS high (2-byte reception config)
	I2C1->CR1 &= ~I2C_CR1_ACK_Msk; // clear ACK
	I2C1->CR1 |= I2C_CR1_POS_Msk; // set POS
	
	// EV6: Clearing Sequence -- Read I2C_SR1 and I2C_SR2 to clear the ADDR bit
	I2C1->SR1;
	I2C1->SR2;

	// EV7: Wait until data is read (BTF=1) -- Data 1 in DR, Data 2 in shift register
	// Not set after NACK reception
	while( !(I2C1->SR1 & I2C_SR1_BTF_Msk) );

	// P: Write STOP bit of I2C_CR1 (generate stop condition on bus)
	I2C1->CR1 |= (1 << I2C_CR1_STOP_Pos); // frees the bus for other transmissions

 	// Data 1: Read MSB
	data = (I2C1->DR & I2C_DR_DR_Msk) << 8;
	// Data 2: Read LSB
	data |= (I2C1->DR & I2C_DR_DR_Msk);

	// NA: NACK tells slave the master is done reading
	//I2C1->CR1 &= !I2C_CR1_ACK_Msk; // clears ACK register for NACK returned	
	
	return data;
}
