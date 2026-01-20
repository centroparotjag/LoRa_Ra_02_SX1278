/*
 * fram.h
 *
 *  Created on: Oct 13, 2025
 *      Author: centr
 */

#ifndef INC_FRAM_H_
#define INC_FRAM_H_


#include "stm32f4xx_hal.h"

uint32_t read_fram_count_init (void);
void write_fram_count_init (void);
uint32_t read_fram_count_time_on (void);
void write_fram_count_time_on(void);
uint32_t read_fram_received_byte_counter (void);
void write_fram_received_byte_counter (void);

void read_fram_into_terminal (void);
void fram_erase_full (void);
uint32_t FRAM_data_verification (uint8_t* DATA);


#endif /* INC_FRAM_H_ */


