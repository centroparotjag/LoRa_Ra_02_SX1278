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
		}
	}
}

//--------------- test -------------------
uint8_t triger = 1;
void encoder_test (void){

	if (!HAL_GPIO_ReadPin(enc_up_GPIO_Port, enc_up_Pin)){
		ST7789_FillScreen(BLACK);
		ST7789_DrawString_10x16 (95, 45, "UP", RED);
		while (!HAL_GPIO_ReadPin(enc_up_GPIO_Port, enc_up_Pin));
		triger = 1;

	}

	if (!HAL_GPIO_ReadPin(enc_button_GPIO_Port, enc_button_Pin)){
		ST7789_FillScreen(BLACK);
		ST7789_DrawString_10x16 (95, 45, "PRESSED", YELLOW);
		while (!HAL_GPIO_ReadPin(enc_button_GPIO_Port, enc_button_Pin));
		triger = 1;
	}

	if (!HAL_GPIO_ReadPin(enc_down_GPIO_Port, enc_down_Pin)){
		ST7789_FillScreen(BLACK);
		ST7789_DrawString_10x16 (95, 45, "DOWN", GREEN);
		while (!HAL_GPIO_ReadPin(enc_down_GPIO_Port, enc_down_Pin));
		triger = 1;
	}

	if (HAL_GPIO_ReadPin(enc_up_GPIO_Port, enc_up_Pin) && HAL_GPIO_ReadPin(enc_button_GPIO_Port, enc_button_Pin) &&
		HAL_GPIO_ReadPin(enc_down_GPIO_Port, enc_down_Pin) && triger == 1){

		ST7789_FillScreen(BLACK);
		ST7789_DrawString_10x16 (20, 45, "Encoder not engaged", WHITE);
		triger = 0;

		fm24cl04_presence ();

	}

}
