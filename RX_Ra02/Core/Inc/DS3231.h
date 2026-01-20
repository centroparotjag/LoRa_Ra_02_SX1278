/*
 * DS3231.h
 *
 *  Created on: Oct 20, 2025
 *      Author: centr
 */

#ifndef INC_DS3234_H_
#define INC_DS3234_H_

#include "stm32f4xx_hal.h"

void displayed_data_time_DS3231 (uint16_t background_color, uint8_t displayed);
void Write_time_to_RTC (uint8_t DayWeek, uint8_t Day, uint8_t Month, uint8_t Year, uint8_t Hours, uint8_t min, uint8_t sec);
void read_data_time_DS3231 (uint8_t* data);
uint32_t convert_time_to_sec (uint8_t H, uint8_t m, uint8_t s);
void convert_time_sec_to_H_m_s (uint32_t sec, uint8_t* arr);

#endif /* INC_DS3231_H_ */
