/*
 * fram.c
 *
 *  Created on: Oct 13, 2025
 *      Author: centr
 */

#include "fram.h"
#include "stm32f4xx_hal.h"
#include "config.h"
#include "display.h"
#include <st7789.h>
#include "main.h"

extern I2C_HandleTypeDef hi2c1;

uint8_t dev_i2c_presence (void){
	uint8_t state = 0;
	state = HAL_I2C_IsDeviceReady (& hi2c1, FRAM_i2c_addrr, 2, 100) == 0  ?  (state | 0x01) : state ;
	state = HAL_I2C_IsDeviceReady (& hi2c1,   RTC_i2c_addr, 2, 100) == 0  ?  (state | 0x02) : state ;
	state = HAL_I2C_IsDeviceReady (& hi2c1, STH30_i2c_addr, 2, 100) == 0  ?  (state | 0x04) : state ;
	return state;
}


void read_fram_into_terminal (void){


    char buf[]="Test\r";
    CDC_Transmit_FS(buf, 5);


}

