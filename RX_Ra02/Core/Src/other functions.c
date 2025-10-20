/*
 * other functions.c
 *
 *  Created on: Oct 20, 2025
 *      Author: centr
 */

#include "stm32f4xx_hal.h"
#include "other functions.h"
#include "fram.h"
#include "config.h"
#include "display.h"
#include <st7789.h>
#include "main.h"
#include "Ra_02_LORA.h"

extern myLoRa;
extern float Ubat;
extern uint8_t count_adc_limit;

void display_of_device_presence_at_startup (void){
	HAL_GPIO_WritePin(pow_hold_GPIO_Port, pow_hold_Pin, GPIO_PIN_RESET); // power hold - enabled
	displaying_images_from_flash ();
	count_adc_limit = 0;
	convert_adc_3ch ();

	uint8_t dev_LoRa = 1;		//Ra_02_pressence (&myLoRa);
	uint8_t flash = flash_W25Q64_pressence ();
	uint8_t dev_i2c = dev_i2c_presence ();

	ST7789_DrawString_10x16 (50, 20,  "FM24CL04 -", CYAN);
	ST7789_DrawString_10x16 (50, 40,  "W25Q64   -", CYAN);
	ST7789_DrawString_10x16 (50, 60,  "DS3231   -", CYAN);
	ST7789_DrawString_10x16 (50, 80,  "SHT30    -", CYAN);
	ST7789_DrawString_10x16 (50, 100, "Ra-02    -", CYAN);

	//---------------------------------------------------------------
	if ((dev_i2c & 0x01)==0x01) {			//fram
		ST7789_DrawString_10x16 (160, 20,  "OK", GREEN);
	}
	else {
		ST7789_DrawString_10x16 (160, 20,  "ERROR", RED);
	}
	if (flash) {							// flash
		ST7789_DrawString_10x16 (160, 40,  "OK", GREEN);
	}
	else {
		ST7789_DrawString_10x16 (160, 40,  "ERROR", RED);
	}
	if ((dev_i2c & 0x02)==0x02) {			// rtc
		ST7789_DrawString_10x16 (160, 60,  "OK", GREEN);
	}
	else {
		ST7789_DrawString_10x16 (160, 60,  "ERROR", RED);
	}
	if ((dev_i2c & 0x04)==0x04) {			//th
		ST7789_DrawString_10x16 (160, 80,  "OK", GREEN);
	}
	else {
		ST7789_DrawString_10x16 (160, 80,  "ERROR", RED);
	}
	if (dev_LoRa) {
		ST7789_DrawString_10x16 (160, 100,  "OK", GREEN);
	}
	else {
		ST7789_DrawString_10x16 (160, 100,  "ERROR", RED);
	}
	//---------------------------------------------------------------
	char buff [24] = {0};
	sprintf (buff, "V bat = %.3fV" , Ubat);

	if (Ubat >= BATT_LOW_VOLTAGE){
		ST7789_DrawString_10x16 (50, 140, buff, GREEN);
	}
	else {

		ST7789_DrawString_10x16 (50, 140, buff, RED);
	}

	//==================================================================
	if (flash==0 || dev_LoRa==0 || dev_i2c != 0b111 || Ubat < BATT_LOW_VOLTAGE){
		ST7789_DrawString_10x16 (30, 200,  "CRITICAL FAILURE !!!", RED);
		HAL_Delay(5000);
		HAL_GPIO_WritePin(pow_hold_GPIO_Port, pow_hold_Pin, GPIO_PIN_SET); // power hold - disabled
		while (1);
	}
	HAL_Delay(1000);
	ST7789_FillScreen(BLACK);
}
