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

extern uint8_t RTC_view;
extern uint16_t background_color;
extern uint8_t MENU_update;
extern uint8_t MENU_stage;

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
			MENU_update = 1;
			MENU_stage = 0;
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

	//ST7789_FillScreen(background_color);

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
	//--------------------------------------------------------------------------------
	//HAL_Delay(1000);
	//ST7789_FillScreen(BLACK);
	return 1;				// Everything is done well.
}

