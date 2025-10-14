/*
 * fram.h
 *
 *  Created on: Oct 13, 2025
 *      Author: centr
 */

#ifndef INC_FRAM_H_
#define INC_FRAM_H_


#include "stm32f4xx_hal.h"

uint8_t fm24cl04_presence (void);
void read_fram_into_terminal (void);

#endif /* INC_FRAM_H_ */
