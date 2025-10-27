/*
 * SPI.h
 *
 * Created: 26.10.2025 12:08:12
 *  Author: centr
 */ 


#ifndef SPI_H_
#define SPI_H_

#include <avr/io.h>

void SPI_MasterInit(void);
void SPI_MasterDeInit(void);
uint8_t SPI_MasterTransmit(uint8_t data);


#endif /* SPI_H_ */