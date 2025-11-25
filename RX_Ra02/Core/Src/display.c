/*
 * display.c
 *
 *  Created on: Oct 12, 2025
 *      Author: centr
 */

#include <stdlib.h>
#include "display.h"
#include <st7789.h>
#include "main.h"
#include "fram.h"
#include "adc.h"
#include "Flash_W25Q64.h"
#include "config.h"

extern uint8_t RTC_view;
extern uint16_t background_color;
extern uint8_t MENU_update;
extern uint8_t MENU_stage;
extern uint8_t flag_status_read_T_DS18B20;
extern uint8_t LoRa_receive_data;
extern float T_ob_fix;
extern float H_ob_fix;

void power_on_displayed (void){
	uint16_t color;

	color = RGB565(0, 255, 0);
	ST7789_DrawString_10x16 (85, 25, "POWER ON", color);
	ST7789_DrawString_10x16 (100, 45, "check", color);

	ST7789_DrawString_5x8(25, 182, "*Checking against accidental ", WHITE);
	ST7789_DrawString_5x8(30, 195, "activation.", WHITE);

	HAL_Delay(300);
	color = RGB565(0, 100, 0);
	ST7789_DrawRectangleFilled(10, 100, 230, 140, color);
	color = RGB565(0, 255, 0);
	ST7789_DrawRectangle(10, 100, 230, 140, color);
	ST7789_DrawRectangle(11, 101, 229, 139, color);

	HAL_Delay(300);

	for (uint8_t i = 0; i < 217; ++ i) {
		color = RGB565(0, 38+i, 217-i);
		ST7789_DrawLine(i+12, 102, i+12, 138, color);
		HAL_Delay(4);

		if (HAL_GPIO_ReadPin(pow_button_GPIO_Port, pow_button_Pin)){			 // check power button state
			i = 254;
		}
	}

	if (!HAL_GPIO_ReadPin(pow_button_GPIO_Port, pow_button_Pin)){			 // check power button state
		HAL_GPIO_WritePin(pow_hold_GPIO_Port, pow_hold_Pin, GPIO_PIN_RESET); // power hold - enabled
		color = RGB565(0, 0, 0);
		ST7789_DrawRectangleFilled(8, 45, 200, 66, color);

		color = RGB565(0, 255, 0);
		ST7789_DrawString_10x16 (112, 45, "OK!", color);

		HAL_Delay(1000);

		ST7789_FillScreen(BLACK);
	}
	else {
		HAL_GPIO_WritePin(pow_hold_GPIO_Port, pow_hold_Pin, GPIO_PIN_RESET); // power hold - enabled
		ST7789_DrawRectangleFilled(100, 45, 160, 61, BLACK);
		ST7789_DrawString_10x16 (95, 45, "FAILED", RED);
		ST7789_DrawRectangleFilled(80, 110, 168, 132, BLACK);
		ST7789_DrawString_10x16 (85, 112, "SHUTDOWN", RED);
		HAL_Delay(1000);
		ST7789_FillScreen(BLACK);
		HAL_GPIO_WritePin(pow_hold_GPIO_Port, pow_hold_Pin, GPIO_PIN_SET);   // power hold - disabled
	}
}

void shutdown_displayed (void){
	uint16_t color;

	uint16_t i = 0;
	uint16_t filter = 200;

	battery_check_low_voltage ();

	while ( i <= filter) {
		if (HAL_GPIO_ReadPin(pow_button_GPIO_Port, pow_button_Pin)){			 // check power button state
			break;
		}
		HAL_Delay(1);
		++ i;
	}

	if (i > filter  &&  !HAL_GPIO_ReadPin(pow_button_GPIO_Port, pow_button_Pin) ){											// button power pressed and checked
		RTC_view = 0;
		ST7789_FillScreen(BLACK);
		color = RGB565(0, 255, 0);
		ST7789_DrawString_10x16 (85, 25, "SHUTDOWN", color);
		ST7789_DrawString_10x16 (100, 45, "check", color);


		ST7789_DrawString_5x8(25, 182, "*Checking against accidental ", WHITE);
		ST7789_DrawString_5x8(30, 195, "shutdown.", WHITE);

		color = RGB565(50, 0, 0);
		ST7789_DrawRectangleFilled(10, 100, 230, 140, color);
		color = RGB565(255, 0, 0);
		ST7789_DrawRectangle(10, 100, 230, 140, color);
		ST7789_DrawRectangle(11, 101, 229, 139, color);

		HAL_Delay(300);

		for (uint8_t i = 0; i < 217; ++ i) {
			color = RGB565(38+i, 217-i, 0);
			ST7789_DrawLine(i+12, 102, i+12, 138, color);
			HAL_Delay(4);

			if (HAL_GPIO_ReadPin(pow_button_GPIO_Port, pow_button_Pin)){			 // check power button state
				break;
			}
		}

		if (!HAL_GPIO_ReadPin(pow_button_GPIO_Port, pow_button_Pin)){			 // check power button state

			ST7789_DrawRectangleFilled(80, 25, 165, 41, BLACK);
			ST7789_DrawRectangleFilled(8, 45, 200, 66, BLACK);
			color = RGB565(0, 255, 0);
			ST7789_DrawString_10x16 (20, 45, "Shutdown is complete.", color);
			HAL_Delay(2000);
			HAL_GPIO_WritePin(pow_hold_GPIO_Port, pow_hold_Pin, GPIO_PIN_SET); // power hold - disabled
			while(1){
				ST7789_DrawRectangleFilled(25, 108, 218, 134, BLACK);
				HAL_Delay(1000);
				ST7789_DrawString_10x16 (30, 112, "Release the button.", RED);
				HAL_Delay(1000);
			}
		}
		else {
			HAL_GPIO_WritePin(pow_hold_GPIO_Port, pow_hold_Pin, GPIO_PIN_RESET);   // power hold - enabled
			ST7789_DrawRectangleFilled(100, 45, 160, 61, BLACK);
			ST7789_DrawString_10x16 (95, 45, "FAILED", RED);
			ST7789_DrawRectangleFilled(80, 108, 160, 134, BLACK);
			ST7789_DrawString_10x16 (90, 112, "Return.", RED);
			HAL_Delay(1000);
			ST7789_FillScreen(BLACK);
			LoRa_receive_data = 1;
			MENU_update = 1;
			MENU_stage = 0;
			T_ob_fix = 100;
			H_ob_fix = 100;
			//m_set = 0;
			//flag_status_read_T_DS18B20 = 0;
		}
	}
}


uint8_t displaying_images_from_flash (void){
	uint8_t data [72]={0};
	uint8_t x = 0;
	uint16_t color;
	uint8_t count_PX = 0;
	uint32_t addr_image_on_flash = 0x02A335;

	//------- Search for signatures and image headers .BMP on a flash drive ----------
	if(!signature_and_data_search ()){
		return 0;		// images not found
	}
	//---------- Displaying an image from a flash drive to the display ---------------
	for (uint8_t y = 0; y < 240; ++y){
		x=239;
		for (uint8_t i = 0; i < 10; ++i){

			read_data_flash_W25Q64 (addr_image_on_flash-72, data, 72);
			addr_image_on_flash-=72;
			count_PX = 71;

			for (uint8_t px = 0; px < 24; ++px){
				color = RGB565(data [count_PX-2], data [count_PX], data [count_PX-1]);
				ST7789_DrawPixel(x,  y, color);
				--x;
				count_PX-=3;
			}
		}

	}
	return 1;				// Everything is done well.
}


void battery_level_10_18 (uint16_t x0, uint16_t y0, float V){
	uint16_t LEVEL = 0;
	uint16_t percent = 0;
	float Vmax = 4.1;
	float  delta_V_max = Vmax - BATT_LOW_VOLTAGE;

	//V=3.2;
	if (V>Vmax){ V=Vmax;}

	float  delta_V = Vmax - V;

	if (delta_V > delta_V_max) {delta_V = delta_V_max;}

	if (V>=Vmax){
		percent = 100;
		LEVEL = 0;
	}
	else {
		percent = (100 * delta_V)/delta_V_max;
		LEVEL = (15*percent)/100;
	}

	//-------------------- BATT contour -------------------------------
	uint16_t color_BATT = RGB565(255, 255, 255);
	if (LEVEL>=14){
		color_BATT = RGB565(255, 0, 0);
	}
	ST7789_DrawRectangleFilled(x0+4,  y0,    x0+11, y0+3,  color_BATT);
	ST7789_DrawRectangleFilled(x0,    y0+3,  x0+15, y0+6,  color_BATT);
	ST7789_DrawRectangleFilled(x0,    y0+6,  x0+3,  y0+26, color_BATT);
	ST7789_DrawRectangleFilled(x0+3,  y0+23, x0+15, y0+26, color_BATT);
	ST7789_DrawRectangleFilled(x0+12, y0+6,  x0+15, y0+23, color_BATT);
	ST7789_DrawRectangle      (x0+3,  y0+6,  x0+11, y0+22, BLACK);
	//------------------------------------------------------------------


	uint8_t x1_l=x0+4;
	uint8_t y1_l=y0+7;
	uint8_t x2_l=x0+11;
	uint8_t y2_l=y0+22;


	uint16_t color_level = RGB565(0, 255, 0);
	if (LEVEL > 10 ){
		color_level = RGB565(255, 0, 0);
	}

	//--------------------- level ----------------------------------------------
	if (LEVEL != 15){
		ST7789_DrawRectangleFilled(x1_l, y1_l + LEVEL,  x2_l, y2_l, color_level);
	}

	//--------------------- fone ----------------------------------------------
	ST7789_DrawRectangleFilled(x1_l-1, y1_l-1,  x2_l, y2_l-(14-LEVEL), BLACK);



}




