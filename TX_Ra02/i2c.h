/*
 * i2c.h
 *
 * Created: 26.10.2025 11:56:10
 *  Author: centr
 */ 


#ifndef I2C_H_
#define I2C_H_

#include <avr/io.h>

#define SCL_CLOCK         100000UL	// I2C clock frequency (100kHz standard)
#define FRAM_ADDR_I2C     0x50		// 8b-addr = 0xA0
#define SHT30_ADDR_I2C    0x88		// 8b-addr = 0x88


void i2c_init(void);
void i2c_deinit(void);
uint8_t i2c_start(uint8_t address);
void i2c_stop(void);
uint8_t i2c_write(uint8_t data);
uint8_t i2c_read_ack(void);
uint8_t i2c_read_nack(void);

void WRITE_DATA_I2C (uint8_t slave_addr, uint8_t* data, uint8_t lenght_data);
void READ_DATA_I2C (uint8_t slave_addr, uint8_t* received_data, uint8_t lenght_data);


#endif /* I2C_H_ */