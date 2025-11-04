/*
 * DS3231.c
 *
 *  Created on: Oct 20, 2025
 *      Author: centr
 */

#include "stm32f4xx_hal.h"
#include "DS3231.h"
#include "config.h"
#include "display.h"
#include <st7789.h>
#include "main.h"
#include "other functions.h"
#include "adc.h"
#include "fram.h"
#include <stdio.h>

extern I2C_HandleTypeDef hi2c1;
extern uint8_t MENU_update;
extern float humidity_on_board;
uint8_t flag_once_wr_count_init = 0;
uint32_t sec_time = 0;

void displayed_data_time_DS3231 (uint16_t background_color, uint8_t displayed){

	uint8_t pData[19] = {0};
	uint8_t s = 0;
	uint8_t m = 0;
	uint8_t H = 0;
	uint8_t w = 0;
	uint8_t d = 0;
	uint8_t M = 0;
	uint8_t Y = 0;

	HAL_I2C_Mem_Read(& hi2c1, RTC_i2c_addr, 0x00, 1, pData, 7, 100);
	Y = ((pData[6] & 0xF0)>>4)*10 + (pData[6] & 0x0F);

	s = ((pData[0] & 0xF0)>>4)*10 + (pData[0] & 0x0F);
	m = ((pData[1] & 0xF0)>>4)*10 + (pData[1] & 0x0F);
	H = ((pData[2] & 0x30)>>4)*10 + (pData[2] & 0x0F);
	w =  pData[3];
	d = ((pData[4] & 0xF0)>>4)*10 + (pData[4] & 0x0F);
	M = ((pData[5] & 0x10)>>4)*10 + (pData[5] & 0x0F);
	Y = ((pData[6] & 0xF0)>>4)*10 + (pData[6] & 0x0F);

	sec_time = convert_time_to_sec (H, m, s);


	char buff  [8] = {0};

	if (flag_once_wr_count_init == 0){
		HAL_Delay(10);
		write_fram_count_init ();
		flag_once_wr_count_init = 1;
		HAL_Delay(10);
		MENU_update = 1;
	}

	if (s == 0) {
		write_fram_count_time_on();
	}

	if (s%10 == 0){
		convert_adc_3ch ();
		float temperature;
		mesurement_t_h_SHT30 (& temperature, & humidity_on_board);
		MENU_update = 1;
	}



	if (displayed == 1){
		sprintf (buff, "%02d-%02d-%02d  %02d:%02d:%02d ", d, M, Y, H, m, s);
		ST7789_DrawString_10x16_background(50, 2, buff, YELLOW, background_color);

		switch (w) {
			case 1: ST7789_DrawString_10x16_background(15, 2, "Mo", YELLOW, background_color);
				break;
			case 2: ST7789_DrawString_10x16_background(15, 2, "Tu", YELLOW, background_color);
				break;
			case 3: ST7789_DrawString_10x16_background(15, 2, "We", YELLOW, background_color);
				break;
			case 4: ST7789_DrawString_10x16_background(15, 2, "Th", YELLOW, background_color);
				break;
			case 5: ST7789_DrawString_10x16_background(15, 2, "Fr", YELLOW, background_color);
				break;
			case 6: ST7789_DrawString_10x16_background(15, 2, "Sa", YELLOW, background_color);
				break;
			case 7: ST7789_DrawString_10x16_background(15, 2, "Su", RED, background_color);
				break;
		}
	}
}


uint32_t convert_time_to_sec (uint8_t H, uint8_t m, uint8_t s){
	uint32_t sec = (H*3600) + (m*60) + s;
	return sec;
}

void convert_time_sec_to_H_m_s (uint32_t sec, uint8_t* arr){
	//sec = 83553;    //test

	uint32_t HOURS     = sec/3600;
	uint32_t min       = (sec-(HOURS*3600))/60;
	uint32_t seconds   = sec%60;

	arr[0] = (uint8_t) HOURS;
	arr[1] = (uint8_t) min;
	arr[2] = (uint8_t) seconds;
}

void read_data_time_DS3231 (uint8_t* data){
	uint8_t pData[19] = {0};
	HAL_I2C_Mem_Read(& hi2c1, RTC_i2c_addr, 0x00, 1, pData, 7, 100);
	data [6] = ((pData[0] & 0xF0)>>4)*10 + (pData[0] & 0x0F);    // s
	data [5]  = ((pData[1] & 0xF0)>>4)*10 + (pData[1] & 0x0F);    // m
	data [4]  = ((pData[2] & 0x30)>>4)*10 + (pData[2] & 0x0F);    // H
	data [3]  =  pData[3];                                        // DW
	data [2]  = ((pData[4] & 0xF0)>>4)*10 + (pData[4] & 0x0F);    // D
	data [1]  = ((pData[5] & 0x10)>>4)*10 + (pData[5] & 0x0F);    // M
	data [0]  = ((pData[6] & 0xF0)>>4)*10 + (pData[6] & 0x0F);    // Y

}


void Write_time_to_RTC (uint8_t DayWeek, uint8_t Day, uint8_t Month, uint8_t Year, uint8_t Hours, uint8_t min, uint8_t sec ){

	uint8_t data[8] = {0};

	data[0] = 0x00;		// num register
	data[1] = sec;
	data[2] = min;
	data[3] = Hours;
	data[4] = DayWeek;
	data[5] = Day;
	data[6] = Month;
	data[7] = Year;

	HAL_I2C_Master_Transmit(& hi2c1, RTC_i2c_addr, data, 8, 100);

}
