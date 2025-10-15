/*
 * STH30.h
 *
 *  Created on: Oct 15, 2025
 *      Author: centr
 */

#ifndef INC_SHT30_H_
#define INC_SHT30_H_

#include "stm32f4xx_hal.h"
uint8_t calculate_crc8_nrsc5(const uint8_t *data, size_t length);
uint8_t mesurement_t_h_SHT30 (float* temperature, float* humidity);
void displayed_t_h (void);


#endif /* INC_SHT30_H_ */
