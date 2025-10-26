/*
 * SPI.c
 *
 * Created: 26.10.2025 12:07:56
 *  Author: centr
 */ 



#include <avr/io.h>
#include "SPI.h"

// Function to initialize SPI as Master
void SPI_MasterInit(void) {
	// Set MOSI, SCK, and SS as output
	// DDRB |= (1 << DDB5) | (1 << DDB7) | (1 << DDB4); // If you're using default pinout
	//DDRB |= (1 << MOSI) | (1 << SCK) | (1 << SS);
	// Enable SPI, Master, set clock rate fck/16
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

// Function to transmit and receive data
uint8_t SPI_MasterTransmit(uint8_t data) {
	// Start transmission
	SPDR = data;
	// Wait for transmission complete
	while (!(SPSR & (1 << SPIF)));
	// Return data from SPI data register
	return SPDR;
}