/*
 * adc.h
 *
 *  Created on: Oct 17, 2025
 *      Author: centr
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_
#include "stm32f4xx_hal.h"

void convert_adc_3ch (void);
uint8_t displayed_adc_measurement_full (void);
uint8_t battery_check_low_voltage (void);

#endif /* INC_ADC_H_ */
