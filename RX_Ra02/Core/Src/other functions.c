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
#include <stdio.h>
#include <time.h>
#include "adc.h"

extern myLoRa;
extern float Ubat;

void display_of_device_presence_at_startup (void){
	HAL_GPIO_WritePin(pow_hold_GPIO_Port, pow_hold_Pin, GPIO_PIN_RESET); // power hold - enabled
	displaying_images_from_flash ();
	convert_adc_3ch ();

	uint8_t dev_LoRa = Ra_02_pressence (&myLoRa);
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


//---------------- buttons --------------------------
extern uint8_t MENU_update;
extern uint8_t state_but;
extern uint8_t MENU;
extern uint8_t MENU_stage;

uint16_t inactivity_counter = 0;

void buttons (void){
	uint8_t filter = 255;

	if (state_but>0){
		state_but = 0xFF;
	}

	inactivity_counter++;

	//------------ UP ------------------------------
	if (!HAL_GPIO_ReadPin(enc_up_GPIO_Port, enc_up_Pin) && state_but == 0){
		for (uint16_t i=0; i<filter; ++i) {
			if (HAL_GPIO_ReadPin(enc_up_GPIO_Port, enc_up_Pin)){
				break;
			}
			else {
				state_but = 1;
				MENU_update = 1;
				inactivity_counter = 0;
			}
		}

	}

	//------------ PUSH ------------------------------
	if (!HAL_GPIO_ReadPin(enc_button_GPIO_Port, enc_button_Pin) && state_but == 0){
		for (uint16_t i=0; i<filter; ++i) {
			if (HAL_GPIO_ReadPin(enc_button_GPIO_Port, enc_button_Pin)){
				break;
			}
			else {
				state_but = 2;
				MENU_update = 1;
				inactivity_counter = 0;
			}
		}
	}

	//------------ DOWN ------------------------------
	if (!HAL_GPIO_ReadPin(enc_down_GPIO_Port, enc_down_Pin) && state_but == 0){
		for (uint16_t i=0; i<filter; ++i) {
			if (HAL_GPIO_ReadPin(enc_down_GPIO_Port, enc_down_Pin)){
				break;
			}
			else {
				state_but = 3;
				MENU_update = 1;
				inactivity_counter = 0;
			}
		}
	}



	if (HAL_GPIO_ReadPin(enc_up_GPIO_Port, enc_up_Pin) && HAL_GPIO_ReadPin(enc_button_GPIO_Port, enc_button_Pin)  &&
		HAL_GPIO_ReadPin(enc_down_GPIO_Port, enc_down_Pin) ){
		state_but = 0;
	}

//	char b[34];
//	sprintf (b , "state_but%d", state_but);
//	ST7789_DrawString_10x16_background(10, 200, b, WHITE, BLACK);

//----auto-return to the main menu when inactive ----
	if (inactivity_counter >= 600){
		inactivity_counter = 0xFF00;
		if ( MENU == 1 || MENU == 5){
				MENU = 0;
				MENU_update = 1;
				MENU_stage = 0;
				state_but = 0x08;
			}
	}

	//---- return to MENU 0 ----
	if ( MENU == 0 && state_but == 2){
		MENU = 1;
		MENU_update = 1;
		MENU_stage = 0;
		state_but = 0x08;

	}

}
