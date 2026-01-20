/*
 * SHT30.h
 *
 * Created: 26.10.2025 13:49:08
 *  Author: centr
 */ 


#ifndef SHT30_H_
#define SHT30_H_

#include <avr/io.h>


uint8_t calculate_crc8_nrsc5(const uint8_t* data);
void SHT30_heater (uint8_t onoff);
uint16_t SHT30_read_status_reg (void);
uint8_t measurement_t_h_SHT30 (uint8_t* Th);

#endif /* SHT30_H_ */