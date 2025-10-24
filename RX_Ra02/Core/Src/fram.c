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
#include "usbd_cdc_if.h"
extern I2C_HandleTypeDef hi2c1;
extern CRC_HandleTypeDef hcrc;

//--------------------------------------------------------------------
// 00 01 02 03  |  04 05 06 07   |
// count init   |  count time ON |
//--------------------------------
#define addr_fram_count_init	0x00
#define addr_fram_count_time_on	0x10

uint32_t FRAM_data_verification (uint8_t* DATA){

	uint32_t count[3] = {0};
	uint8_t c=0;

	for (uint8_t i = 0;  i<3;  i++){
		count[i] |= DATA[c]<<24 | DATA[c+1]<<16 | DATA[c+2]<<8 | DATA[c+3];
		c+=4;
	}

	if (count[0] == count[1]) {
		return count[0];
	}

	if (count[0] == count[2]) {
		return count[0];
	}

	if (count[1] == count[2]) {
		return count[1];
	}
	else {
		return 0;
	}
}


uint32_t read_fram_count_init (void){
	uint32_t count = 0;
	uint8_t pData[12] = {0};
	HAL_I2C_Mem_Read (& hi2c1, FRAM_i2c_addrr, addr_fram_count_init, 1, pData, 12, 100);
	count = FRAM_data_verification (&pData);
	return count;
}

void write_fram_count_init (void){
	uint8_t pData[12] = {0};
	uint32_t count = read_fram_count_init ();
	count ++;
	pData[0] = (uint8_t)((count & 0xFF000000) >> 24);
	pData[1] = (uint8_t)((count & 0x00FF0000) >> 16);
	pData[2] = (uint8_t)((count & 0x0000FF00) >> 8);
	pData[3] = (uint8_t)( count & 0x000000FF);
	pData[4] = pData[0];
	pData[5] = pData[1];
	pData[6] = pData[2];
	pData[7] = pData[3];
	pData[8] = pData[0];
	pData[9] = pData[1];
	pData[10] =pData[2];
	pData[11] =pData[3];
	HAL_I2C_Mem_Write(& hi2c1, FRAM_i2c_addrr, addr_fram_count_init, 1, pData, 12, 100);
}

//----------------------------------------------------------------------------
uint32_t read_fram_count_time_on (void){
	uint32_t count = 0;
	uint8_t pData[12] = {0};
	HAL_I2C_Mem_Read (& hi2c1, FRAM_i2c_addrr, addr_fram_count_time_on, 1, pData, 12, 100);
	count = FRAM_data_verification (&pData);
	return count;
}

void write_fram_count_time_on(void){
	uint8_t pData[12] = {0};
	uint32_t count = read_fram_count_time_on ();
	count ++;
	pData[0] = (uint8_t)((count & 0xFF000000) >> 24);
	pData[1] = (uint8_t)((count & 0x00FF0000) >> 16);
	pData[2] = (uint8_t)((count & 0x0000FF00) >> 8);
	pData[3] = (uint8_t)( count & 0x000000FF);
	pData[4] = pData[0];
	pData[5] = pData[1];
	pData[6] = pData[2];
	pData[7] = pData[3];
	pData[8] = pData[0];
	pData[9] = pData[1];
	pData[10] =pData[2];
	pData[11] =pData[3];
	HAL_I2C_Mem_Write(& hi2c1, FRAM_i2c_addrr, addr_fram_count_time_on, 1, pData, 12, 100);
}

//---------------------------------------------------------------------------

uint8_t dev_i2c_presence (void){
	uint8_t state = 0;
	state |= HAL_I2C_IsDeviceReady (& hi2c1, FRAM_i2c_addrr, 2, 100) == 0  ?  (state | 0x01) : state ;
	state |= HAL_I2C_IsDeviceReady (& hi2c1,   RTC_i2c_addr, 2, 100) == 0  ?  (state | 0x02) : state ;
	state |= HAL_I2C_IsDeviceReady (& hi2c1, SHT30_i2c_addr, 2, 100) == 0  ?  (state | 0x04) : state ;
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


