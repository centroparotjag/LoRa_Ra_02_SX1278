/*
 * UART.h
 *
 * Created: 26.10.2025 12:25:57
 *  Author: centr
 */ 


#ifndef UART_H_
#define UART_H_

#include <avr/io.h>

#define F_CPU 1000000UL // Define CPU frequency for _delay_ms()
#define BAUD 9200
#define UBRR_VALUE ((F_CPU / (16UL * BAUD)) - 1)

void uart_init(void) ;
void uart_transmit(unsigned char data) ;


#endif /* UART_H_ */