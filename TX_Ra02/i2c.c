/*
 * i2c.c
 *
 * Created: 26.10.2025 11:48:31
 *  Author: centr
 */ 
//#define F_CPU               1000000UL	// Define CPU frequency for _delay_ms()
//#define SCL_CLOCK           100000UL	// I2C clock frequency (100kHz standard)
//#define SLAVE_ADDRESS_WRITE 0x50		// 8b-addr = 0xA0


#include "i2c.h"
#include "init.h"
#include <util/delay.h>


void i2c_init(void) {
	// Set SCL frequency
	// TWBR = ((F_CPU / SCL_CLOCK) - 16) / 2;
	// For 16MHz F_CPU and 100kHz SCL_CLOCK, TWBR = 72
	TWBR = 0;
	// Set TWPS (prescaler) to 0 (no prescaling)
	TWSR = 0;
	// Enable TWI
	TWCR = (1 << TWEN);
}

void i2c_deinit(void) {
	// Enable TWI
	TWCR = 0;
}

uint8_t i2c_start(uint8_t address) {
	// Send START condition
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	// Wait for TWINT flag set, indicating completion
	while (!(TWCR & (1 << TWINT)));

	// Check status register for START condition success
	if ((TWSR & 0xF8) != 0x08) return 1; // Return error if not START transmitted

	// Send slave address with R/W bit
	TWDR = address;
	TWCR = (1 << TWINT) | (1 << TWEN);
	// Wait for TWINT flag set
	while (!(TWCR & (1 << TWINT)));

	// Check status register for SLA+W transmitted, ACK received
	if ((TWSR & 0xF8) != 0x18) return 1; // Return error if not SLA+W ACK
	return 0; // Success
}

void i2c_stop(void) {
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN); // Send STOP condition
	while (TWCR & (1 << TWSTO)); // Wait for STOP condition to be executed
}

uint8_t i2c_write(uint8_t data) {
	TWDR = data; // Load data into TWDR register
	TWCR = (1 << TWINT) | (1 << TWEN); // Start transmission
	while (!(TWCR & (1 << TWINT))); // Wait for TWINT flag set

	// Check status register for data transmitted, ACK received
	if ((TWSR & 0xF8) != 0x28) return 1; // Return error if not Data ACK
	return 0; // Success
}

uint8_t i2c_read_ack(void) {
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA); // Read with ACK
	while (!(TWCR & (1 << TWINT))){};
	return TWDR;
}

uint8_t i2c_read_nack(void) {
	TWCR = (1 << TWINT) | (1 << TWEN); // Read with NACK
	while (!(TWCR & (1 << TWINT))){};
	return TWDR;
}

void WRITE_DATA_I2C (uint8_t slave_addr, uint8_t* data, uint8_t lenght_data){
	if (i2c_start(slave_addr) == 0) { // 0x50 << 1 for write (to set register address)
		for(uint8_t i = 0; i<lenght_data; ++i){
			i2c_write(data[i]); // Register address
		}	
	}
	i2c_stop();
}

void READ_DATA_I2C (uint8_t slave_addr, uint8_t* received_data, uint8_t lenght_data){
	uint8_t i = 0;
	i2c_start(slave_addr+1);
	if(lenght_data>1){
		while( i<(lenght_data-1) ){
			received_data[i] = i2c_read_ack(); // Read with ACK
			++i;
		}
	}
	received_data[i] = i2c_read_nack(); // Read with NACK for last byte
	i2c_stop();
}

	
	