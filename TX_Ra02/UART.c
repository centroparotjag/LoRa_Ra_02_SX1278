/*
 * UART.c
 *
 * Created: 26.10.2025 12:25:42
 *  Author: centr
 */ 

#include "UART.h"
#include <avr/io.h>
#include <util/delay.h>



void uart_init(void) {
	// Set baud rate
	UBRR0H = (unsigned char)(UBRR_VALUE >> 8);
	UBRR0L = (unsigned char)UBRR_VALUE;

	// Enable receiver and transmitter
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);

	// Set frame format: 8 data bits, 1 stop bit, no parity
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8-bit data
}

void uart_deinit(void) {
	// Enable receiver and transmitter
	UCSR0B = 0;
}

void uart_transmit(unsigned char data) {
	// Wait for empty transmit buffer
	while (!(UCSR0A & (1 << UDRE0)));

	// Put data into buffer, sends the data
	UDR0 = data;
}

//unsigned char uart_receive(void) {
	//// Wait for data to be received
	//while (!(UCSR0A & (1 << RXC0)));
//
	//// Get and return received data from buffer
	//return UDR0;
//}

//int main(void) {
	//uart_init(); // Initialize UART
//
	//while (1) {
		//// Example: Echo received character
		//unsigned char received_char = uart_receive();
		//uart_transmit(received_char);
//
		//// Example: Transmit a string
		//_delay_ms(500); // Small delay to prevent continuous transmission
		//uart_transmit('H');
		//uart_transmit('e');
		//uart_transmit('l');
		//uart_transmit('l');
		//uart_transmit('o');
		//uart_transmit('\n');
	//}
	//return 0;
//}