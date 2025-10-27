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
	PORTB = PORTB| 0b00000100;		// CS=1.
	DDRB = DDRB | 0b00101111;		// PB5(CLK SPI)=output; PB3(MOSI SPI)=output; PB2(CS SPI)=output; PB1(RST Ra-02)=output; PB0-(3V3-ON)=output;
	// Enable SPI, Master, set clock rate fck/4
	SPCR = (1 << SPE) | (1 << MSTR);			// | (1 << SPR0);
}

void SPI_MasterDeInit(void) {	
	DDRB = DDRB & 0b00000001;		// PB0-(3V3-ON)=output;
	PORTB = PORTB & 0b11111011;		// CS=0.
	SPCR = 0;						// SPE=0
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