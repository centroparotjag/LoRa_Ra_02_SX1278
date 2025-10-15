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


//--------------------------------------------------------------------
// 00 01 02 03  |  04 05 06 07   |
// count init   |  count time ON |
//--------------------------------

uint32_t read_fram_count_init (void){
	uint32_t count = 0;
	uint8_t pData[4] = {0};
	HAL_I2C_Mem_Read (& hi2c1, FRAM_i2c_addrr, 0x00, 0x01, pData, 4, 100);
	count |= pData[0] << 24 | pData[1] << 16 | pData[2] << 8 | pData[3];
	return count;
}

void write_fram_count_init (void){
	uint8_t pData[4] = {0};
	uint32_t count = read_fram_count_init ();
	count +=1;
	pData[0] = (uint8_t)((count & 0xFF000000) >> 24);
	pData[1] = (uint8_t)((count & 0x00FF0000) >> 16);
	pData[2] = (uint8_t)((count & 0x0000FF00) >> 8);
	pData[3] = (uint8_t)( count & 0x000000FF);

	HAL_I2C_Mem_Write(& hi2c1, FRAM_i2c_addrr, 0x00, 1, pData, 4, 100);
}

//----------------------------------------------------------------------------
uint32_t read_fram_count_time_on (void){
	uint32_t count = 0;
	uint8_t pData[4] = {0};
	HAL_I2C_Mem_Read (& hi2c1, FRAM_i2c_addrr, 0x04, 0x01, pData, 4, 100);
	count |= pData[0] << 24 | pData[1] << 16 | pData[2] << 8 | pData[3];
	return count;
}

void write_fram_count_time_on(void){
	uint8_t pData[4] = {0};
	uint32_t count = read_fram_count_time_on ();
	count +=1;
	pData[0] = (uint8_t)((count & 0xFF000000) >> 24);
	pData[1] = (uint8_t)((count & 0x00FF0000) >> 16);
	pData[2] = (uint8_t)((count & 0x0000FF00) >> 8);
	pData[3] = (uint8_t)( count & 0x000000FF);

	HAL_I2C_Mem_Write(& hi2c1, FRAM_i2c_addrr, 0x04, 1, pData, 4, 100);
}

//---------------------------------------------------------------------------





uint8_t dev_i2c_presence (void){
	uint8_t state = 0;
	state = HAL_I2C_IsDeviceReady (& hi2c1, FRAM_i2c_addrr, 2, 100) == 0  ?  (state | 0x01) : state ;
	state = HAL_I2C_IsDeviceReady (& hi2c1,   RTC_i2c_addr, 2, 100) == 0  ?  (state | 0x02) : state ;
	state = HAL_I2C_IsDeviceReady (& hi2c1, SHT30_i2c_addr, 2, 100) == 0  ?  (state | 0x04) : state ;
	return state;
}




void read_fram_into_terminal (void){
    char buf[16] = "\rDUMP FM24CL04:\r";
    CDC_Transmit_FS(buf, 16);

    uint8_t pData[16] = {0};

    for (uint16_t k = 0; k < 16; ++k) {
		HAL_I2C_Mem_Read (& hi2c1, FRAM_i2c_addrr, k*16, 0x01, pData, 16, 100);
		for (uint16_t i = 0; i < 16; ++i) {
			sprintf(buf, "%02X ", pData[i]);
			CDC_Transmit_FS(buf, 3);
			HAL_Delay(2);
		}
		char b[]= "\r";
		CDC_Transmit_FS(b, 1);
    }

    for (uint16_t k = 0; k < 16; ++k) {
		HAL_I2C_Mem_Read (& hi2c1, FRAM_i2c_addrr | 0x02, k*16, 0x01, pData, 16, 100);
		for (uint16_t i = 0; i < 16; ++i) {
			sprintf(buf, "%02X ", pData[i]);
			CDC_Transmit_FS(buf, 3);
			HAL_Delay(2);
		}
		char b[]= "\r";
		CDC_Transmit_FS(b, 1);
    }
}



void fram_erase_full (void){
	uint8_t zero_byte[1] = {0x00}; // Buffer with a zero byte
	for (uint16_t i = 0; i < 256; i++) {
	    // Write the zero byte to the current address
		HAL_I2C_Mem_Write(& hi2c1, FRAM_i2c_addrr, i, 1, zero_byte, 1, 100);
	}
	for (uint16_t i = 0; i < 256; i++) {
	    // Write the zero byte to the current address
		HAL_I2C_Mem_Write(& hi2c1, FRAM_i2c_addrr|0x02, i, 1, zero_byte, 1, 100);
	}
}


