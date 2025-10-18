/*
 * Flash_W25Q64.c
 *
 *  Created on: Oct 18, 2025
 *      Author: centr
 */

#include "stm32f4xx_hal.h"
#include <stdlib.h>
#include "main.h"
#include "Flash_W25Q64.h"


extern SPI_HandleTypeDef hspi2;


void read_data_flash_W25Q64 (uint32_t addr, uint8_t* data, uint8_t Size){

	HAL_GPIO_WritePin(cs_flash_GPIO_Port, cs_flash_Pin, GPIO_PIN_RESET); 	// SC W25Q64 - enabled

	uint8_t buff [4] = {0};

	buff [0] = 0x03;									// The Read Data (03h) instruction
	buff [1] = (uint8_t)((addr & 0x00FF0000) >> 16);	// MSB addr 24b addres W25Q64
	buff [2] = (uint8_t)((addr & 0x0000FF00) >> 8);		// addr
	buff [3] = (uint8_t)( addr & 0x000000FF);			// LSB addr 24b addres W25Q64

	HAL_SPI_Transmit(&hspi2, buff,  4, 100);	// MSB
	HAL_SPI_Receive (&hspi2, data, Size, 100);

	HAL_GPIO_WritePin(cs_flash_GPIO_Port, cs_flash_Pin, GPIO_PIN_SET); 	// SC W25Q64 - disabled
}


uint32_t signature_and_data_search (void){

	uint8_t buff [24] = {0};
	uint32_t addr=0;
	uint16_t signature_found = 0;

	read_data_flash_W25Q64 (0, buff, 24);

	if (buff[0x00]=='B'  && buff[0x01] =='M'  && buff[0x0A]==0x36 &&
		buff[0x0E]==0x28 && buff[0x12]==0xF0  && buff[0x16]==0xF0	)
	{
		return 1;
	}

	return 0;	// not found
}

