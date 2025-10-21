/*
 * adc.c
 *
 *  Created on: Oct 17, 2025
 *      Author: centr
 */


#include "adc.h"
#include "main.h"
#include <stdint.h> // For uint8_t
#include "stm32f4xx_hal.h"
#include "config.h"
#include "display.h"
#include <stdio.h>
#include <math.h>
#include "st7789.h"


extern ADC_HandleTypeDef hadc1;
uint16_t adcData[ADC_CHANNELS_NUM];
float U33  = 3.3;
float Ubat = 4.2;
extern uint8_t MENU_update;
extern uint8_t RTC_view;
extern uint8_t MENU_stage;
extern uint16_t background_color;
void convert_adc_3ch (void){
		HAL_GPIO_WritePin(adc_cir_0V_GPIO_Port, adc_cir_0V_Pin, GPIO_PIN_RESET);		// enable 0V on circuit ADC
		HAL_Delay(25);
		HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adcData, ADC_CHANNELS_NUM);
}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	HAL_GPIO_WritePin(adc_cir_0V_GPIO_Port, adc_cir_0V_Pin, GPIO_PIN_SET);		// disable 0V on circuit ADC
	U33  = (ADC0_REF*4096)/adcData[0];
	Ubat = (U33*adcData[2]*ADC2_R_DEVIDER)/4096;
}


uint8_t displayed_adc_measurement_full (void){
	HAL_GPIO_WritePin(pow_hold_GPIO_Port, pow_hold_Pin, GPIO_PIN_RESET);		// power on
	convert_adc_3ch ();
	HAL_Delay(5);
	background_color = RGB565(105,105,105);
	//-------- NTC 100k-+1% B3950 ----------------
	float Ur100k = (U33 * adcData[1])/4096;
	float rntc   = ((U33/Ur100k)-1)*R100k;

	float temp;
	temp = rntc / (float)R100k;
	temp = logf(temp);
	temp /= (float)3950;
	temp += 1.0f / ((float)25 + 273.15f);
	temp = 1.0f / temp;
	temp -= 273.15f;
	//-------------------------------------------
	char pBuff [32];

	//----------------------------------------

	if(MENU_stage == 0){
		MENU_stage = 1;
		ST7789_FillScreen(background_color);
		ST7789_DrawLine(0, 25, 239, 25, YELLOW);
	}


	ST7789_DrawRectangle(0, 65, 239, 90, YELLOW);
	sprintf(pBuff, "U(bat) = %.3f V", Ubat);
	if(Ubat<=BATT_LOW_VOLTAGE+0.1){
		ST7789_DrawRectangle(0, 30, 239, 60, RED);
		ST7789_DrawString_10x16 (10, 35, "Low battery voltage!!!", RED);

		ST7789_DrawString_10x16_background  (5, 70, pBuff, RED, background_color);
	}
	else{
		ST7789_DrawString_10x16_background (5, 70, pBuff, GREEN, background_color);
		ST7789_DrawRectangleFilled(0, 30, 240, 61, background_color);
	}

	//----------------------------------------
	ST7789_DrawRectangle(0, 95, 239, 120, YELLOW);
	sprintf(pBuff, "U(3v3) = %.3f V", U33);
	ST7789_DrawString_10x16_background  (5, 100, pBuff, GREEN, background_color);

	//----------------------------------------
	ST7789_DrawRectangle(0, 125, 239, 170, YELLOW);
	sprintf(pBuff, "R NTC  = %.1f Ohm", rntc);
	ST7789_DrawString_10x16_background (5, 130, pBuff, CYAN, background_color);
	sprintf(pBuff, "T NTC  = %.1f C", temp);
	ST7789_DrawString_10x16_background (5, 150, pBuff, CYAN, background_color);

	//-----------------------------------------------
	ST7789_DrawRectangle(0, 175, 239, 239, YELLOW);
	ST7789_DrawString_10x16_background  (5, 180, "ADC meas", WHITE, background_color);
	ST7789_DrawString_10x16_background  (95, 180, "AN0 Ref", WHITE, background_color);
	ST7789_DrawString_10x16_background  (95, 200, "AN1 NTC", WHITE, background_color);
	ST7789_DrawString_10x16_background  (95, 220, "AN2 BAT", WHITE, background_color);

	for (uint8_t i = 0; i<3; ++i){
		sprintf(pBuff, "0x%04X", adcData[i]);
		ST7789_DrawString_10x16_background (172, 180 + (i*20), pBuff, WHITE, background_color);
	}
	//----------------------------------------------------
	return 0;
}

uint8_t battery_check_low_voltage (void){
	if(Ubat < BATT_LOW_VOLTAGE-0.1){
		RTC_view = 0;
		HAL_GPIO_WritePin(pow_hold_GPIO_Port, pow_hold_Pin, GPIO_PIN_RESET);		// power on
		char pBuff [32];
		ST7789_FillScreen(RED);

		uint8_t s = 30;
		uint8_t flag = 0;
		for (uint8_t i = 0; i < s; ++i) {
			sprintf(pBuff, "LOW BATTERY = %.3fV", Ubat);
			ST7789_DrawString_10x16_background  (20, 70, pBuff, YELLOW, RED);
			ST7789_DrawString_10x16 (50, 110, "SHUTDOWN AFTER", YELLOW);
			convert_adc_3ch ();
			sprintf(pBuff, "%02d ", s-i);
			ST7789_DrawString_10x16_background (115, 150, pBuff, YELLOW, RED);
			HAL_Delay(850);
			flag = ~flag;
			HAL_GPIO_WritePin(led_button_GPIO_Port, led_button_Pin, (flag == 0 ? 1 : 0) ); // LED - OFF/on
			HAL_Delay(100);

			if (Ubat > BATT_LOW_VOLTAGE){
				ST7789_FillScreen(GREEN);
				sprintf(pBuff, "BATTERY OK = %.3fV", Ubat);
				ST7789_DrawString_10x16 (20, 110, pBuff, BLUE);
				HAL_Delay(3000);
				ST7789_FillScreen(BLACK);
				MENU_stage = 0;
				MENU_update = 1;
				return 1;
			}
		}

		HAL_GPIO_WritePin(pow_hold_GPIO_Port, pow_hold_Pin, GPIO_PIN_SET); // power hold - disabled
		ST7789_FillScreen(BLACK);
		while (1);
		return 0;
	}
}
