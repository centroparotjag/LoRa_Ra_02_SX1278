/*
 * MENU.c
 *
 *  Created on: Oct 20, 2025
 *      Author: centr
 */

#include "MENU.h"
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

uint8_t MENU = 1;
uint8_t MENU_update = 1;
uint8_t MENU_stage = 0;
extern uint8_t state_but = 0;
extern   uint16_t background_color;

void MENU_SELEKTOR (void){


    if(MENU_update == 1){

    	if (MENU == 0){

    	}

    	if (MENU == 1){			// selektor - menu
    		MENU_SET ();
    	}

    	if (MENU == 2){
    		background_color = BLACK;
    		displayed_adc_measurement_full ();
    	}
    	if (MENU == 3){
    		background_color = BLACK;
    		displayed_t_h ();
    	}

    	MENU_update = 0;
    }

}



//=========== SELEKTOR MENU 1  ===========================
uint8_t m_set = 1;
void MENU_SET (void){

	if (MENU_stage == 0){
		MENU_stage = 1;
		uint16_t color = RGB565(30,144,255);
		background_color = color;
		ST7789_FillScreen(color);
		ST7789_DrawLine(0, 22, 240, 22, YELLOW);
		ST7789_DrawString_10x16(30, 40, "ADC", WHITE);
		ST7789_DrawString_10x16(30, 65, "SHT30", WHITE);
		ST7789_DrawString_10x16(30, 90, "STATISTICS", WHITE);
		ST7789_DrawString_10x16(30, 115, "TIME SET", WHITE);
		ST7789_DrawString_10x16(30, 140, "DISPLAY", WHITE);
	}


	//---------- buttons ---------------
	if (state_but == 0x01) {
		m_set--;
		if (m_set == 0 ){
			m_set = 5;
		}
	}

	if (state_but == 0x03) {
		m_set++;
		if (m_set > 5){
			m_set = 1;
		}
	}

	if (state_but == 0x02) {
		MENU = m_set+1;
		MENU_update = 1;
		MENU_stage = 0;
		//ST7789_FillScreen(BLACK);
		ST7789_DrawRectangleFilled(0, 25, 240, 240, BLACK);
	}
	else {
		//----  kursor ---------
		if (m_set == 1) {
			ST7789_DrawRectangleFilled(10, 45, 20, 55, RED);
			ST7789_DrawRectangleFilled(10, 56, 20, 160, BLUE);
		}
		if (m_set == 2) {
			ST7789_DrawRectangleFilled(10, 40, 20, 69, BLUE);
			ST7789_DrawRectangleFilled(10, 70, 20, 80, RED);
			ST7789_DrawRectangleFilled(10, 81, 20, 160, BLUE);
		}
		if (m_set == 3) {
			ST7789_DrawRectangleFilled(10, 40, 20, 94, BLUE);
			ST7789_DrawRectangleFilled(10, 95, 20, 105, RED);
			ST7789_DrawRectangleFilled(10, 106, 20, 160, BLUE);
		}
		if (m_set == 4) {
			ST7789_DrawRectangleFilled(10, 40, 20, 119, BLUE);
			ST7789_DrawRectangleFilled(10, 120, 20, 130, RED);
			ST7789_DrawRectangleFilled(10, 131, 20, 160, BLUE);
		}
		if (m_set == 5) {
			ST7789_DrawRectangleFilled(10, 40, 20, 144, BLUE);
			ST7789_DrawRectangleFilled(10, 145, 20, 155, RED);
		}
	}

}


