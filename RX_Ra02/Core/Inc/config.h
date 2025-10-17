/*
 * config.h
 *
 *  Created on: Oct 10, 2025
 *      Author: centr
 */

#ifndef INC_CONFIG_H_
#define INC_CONFIG_H_


#define FRAM_i2c_addrr	0xA0	// FM24CL04
#define RTC_i2c_addr	0xD0    // DS3231
#define SHT30_i2c_addr	0x88	// SHT30


#define ADC_CHANNELS_NUM 3
#define ADC0_REF         2.515	// V; LM4040CIM3-2.5; Ref=2.5V±21mV;
#define ADC2_R_DEVIDER   1.995
#define R100k            100e3  // Ohm
#define BATT_LOW_VOLTAGE 3.2  	// V

#endif /* INC_CONFIG_H_ */
