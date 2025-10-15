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

uint8_t fm24cl04_presence (void);
void read_fram_into_terminal (void);
void fram_erase_full (void);



#endif /* INC_FRAM_H_ */


