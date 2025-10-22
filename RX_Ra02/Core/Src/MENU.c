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

uint8_t MENU = 4;
uint8_t MENU_update = 1;
uint8_t MENU_stage = 0;
extern uint8_t state_but = 0;
extern uint16_t background_color;
extern uint8_t RTC_view;
extern uint8_t m_counter;

void MENU_SELEKTOR (void){


	//------------ return to MENU_SET (1) -------------
	if (state_but == 0x02 && MENU != 0 && MENU != 1) {
		MENU = 1;
		MENU_update = 1;
		MENU_stage = 0;
		state_but = 8;
	}

    if(MENU_update == 1){

    	if (MENU == 0){
    		RTC_view = 1;
    		MENU_O ();
    	}

    	if (MENU == 1){			// selektor - menu
    		RTC_view = 1;
    		MENU_SET ();
    	}

    	if (MENU == 2){			// ADC
    		RTC_view = 1;
    		displayed_adc_measurement_full ();
    	}
    	if (MENU == 3){        // SHT30
    		RTC_view = 1;
    		displayed_t_h ();
    	}
    	if (MENU == 4){			//STATISTICS
    		RTC_view = 1;
    		MENU_STAT ();
    	}
    	if (MENU == 5){			// TIMESET
    		RTC_view = 1;
    		//fram_erase_full ();			//test
    		MENU_RTC ();
    	}
    	if (MENU == 6){			// DISPLAY
    		RTC_view = 0;
    		displaying_images_from_flash ();
    	}

    	MENU_update = 0;
    }

}



//=========== SELEKTOR MENU 1  ===========================
uint8_t m_set = 0;
void MENU_SET (void){

	if (MENU_stage == 0){
		MENU_stage = 1;
		uint16_t color = RGB565(85,107,47);
		background_color = color;
		ST7789_FillScreen(color);
		ST7789_DrawLine(0, 22, 240, 22, YELLOW);
		ST7789_DrawString_10x16(30, 40, "BACK", WHITE);
		ST7789_DrawString_10x16(30, 65, "ADC", WHITE);
		ST7789_DrawString_10x16(30, 90, "SHT30", WHITE);
		ST7789_DrawString_10x16(30, 115, "STATISTICS", WHITE);
		ST7789_DrawString_10x16(30, 140, "TIME SET", WHITE);
		ST7789_DrawString_10x16(30, 165, "DISPLAY", WHITE);
	}



	//---------- buttons ---------------
	if (state_but == 0x01) {
		if (m_set == 0 ){
			m_set = 5;
		}
		else {
			m_set--;
		}

	}

	if (state_but == 0x03) {
		m_set++;
		if (m_set > 5){
			m_set = 0;
		}
	}

	if (state_but == 0x02) {
		if (m_set == 0){
			MENU = 0;
			m_counter = 100;
		}
		else {
			MENU = m_set+1;
		}

		MENU_update = 1;
		MENU_stage = 0;
		ST7789_DrawRectangleFilled(0, 25, 240, 240, BLACK);

	}
	else {
		//----  kursor ---------
		if (m_set == 0) {
			ST7789_DrawRectangleFilled(10, 45, 20, 55, RED);
			ST7789_DrawRectangleFilled(10, 56, 20, 180, background_color);
		}
		if (m_set == 1) {
			ST7789_DrawRectangleFilled(10, 45, 20, 69, background_color);
			ST7789_DrawRectangleFilled(10, 70, 20, 80, RED);
			ST7789_DrawRectangleFilled(10, 81, 20, 180, background_color);
		}
		if (m_set == 2) {
			ST7789_DrawRectangleFilled(10, 45, 20, 94, background_color);
			ST7789_DrawRectangleFilled(10, 95, 20, 105, RED);
			ST7789_DrawRectangleFilled(10, 106, 20, 180, background_color);
		}
		if (m_set == 3) {
			ST7789_DrawRectangleFilled(10, 45, 20, 119, background_color);
			ST7789_DrawRectangleFilled(10, 120, 20, 130, RED);
			ST7789_DrawRectangleFilled(10, 131, 20, 180, background_color);
		}
		if (m_set == 4) {
			ST7789_DrawRectangleFilled(10, 45, 20, 144, background_color);
			ST7789_DrawRectangleFilled(10, 145, 20, 155, RED);
			ST7789_DrawRectangleFilled(10, 156, 20, 180, background_color);
		}
		if (m_set == 5) {
			ST7789_DrawRectangleFilled(10, 45, 20, 169, background_color);
			ST7789_DrawRectangleFilled(10, 170, 20, 180, RED);
		}
	}
	RTC_view = 1;
}



void MENU_O (void){

	background_color = RGB565(34,139,34);

	if(MENU_stage == 0){
		MENU_stage = 1;
		ST7789_FillScreen(background_color);
		ST7789_DrawLine(0, 25, 239, 25, YELLOW);
	}

	ST7789_DrawString_10x16_background(50, 120, "MENU 0", WHITE, background_color);
}

void MENU_STAT (void){
	uint32_t h=0;
	uint32_t m=0;
	char buff [24];

	background_color = RGB565(34, 100, 80);
	uint16_t main_color = RGB565(32,178,170);
	uint16_t outline_color = RGB565(255,255,255);

	if(MENU_stage == 0){
		MENU_stage = 1;
		ST7789_FillScreen(background_color);

		//-------------CIRKLE GRAFIC -----------------
		uint16_t grafic_color = RGB565(60,179,113);
		ST7789_DrawCircleFilled(120, 126, 90, grafic_color);
		ST7789_DrawCircleFilled(120, 126, 80, background_color);
		ST7789_DrawCircleFilled(120, 126, 70, grafic_color);
		ST7789_DrawCircleFilled(120, 126, 60, background_color);
		ST7789_DrawCircleFilled(120, 126, 50, grafic_color);
		ST7789_DrawCircleFilled(120, 126, 40, background_color);
		//----------------- TIME ----------------------------
		ST7789_DrawLine(0, 24, 239, 24, outline_color);
		ST7789_DrawLine(0, 25, 239, 25, outline_color);
		ST7789_DrawLine(0, 26, 239, 26, outline_color);
		//-----------------------------------------------

		ST7789_DrawRectangleFilled(0, 35, 239, 85, main_color);
		ST7789_DrawRectangle(0, 35, 239, 85, outline_color);
		ST7789_DrawString_10x16_background(40, 40, "POWER-ON COUNTER", WHITE, main_color);

		ST7789_DrawRectangleFilled(0, 100, 239, 150, main_color);
		ST7789_DrawRectangle(0, 100, 239, 150, outline_color);
		ST7789_DrawString_10x16_background(50, 105, "WORKING TIMES", WHITE, main_color);

		ST7789_DrawRectangleFilled(0, 165, 239, 239, main_color);
		ST7789_DrawRectangle(0, 165, 239, 239, outline_color);
		ST7789_DrawString_10x16_background(100, 170, "FRAM", WHITE, main_color);
	}


	uint32_t power_on_counter = read_fram_count_init ();
	uint32_t working_times    = read_fram_count_time_on ();

	h = working_times / 60;
	m = working_times%60;

	//-------------------------------------------------------------------------
	sprintf (buff, "%d", power_on_counter);
	int8_t Xc=120;
	if(power_on_counter >= 10000 && power_on_counter < 100000 ){ Xc=80; }
	if(power_on_counter >= 100000 ){ Xc=50; }

	ST7789_DrawString_10x16_background(Xc, 60, buff, YELLOW, main_color);

	sprintf (buff, "%dh %02dm", h, m);
	int8_t Xh=90;
	if(h >= 1000  && h < 10000 ){ Xh=70; }
	if(h >= 10000 && h < 100000 ){ Xc=60; }
	if(h > 100000 ){ Xc=50; }

	ST7789_DrawString_10x16_background(Xh, 125, buff, YELLOW, main_color);
	//-------------------------------------------------------------------------
	sprintf (buff, "p.o.c. 0x%08X", power_on_counter);
	ST7789_DrawString_10x16_background(30, 190, buff, WHITE, main_color);

	sprintf (buff, "w.t.   0x%08X", working_times);
	ST7789_DrawString_10x16_background(30, 210, buff, WHITE, main_color);

}

void MENU_RTC (void){

	background_color = RGB565(80,110,34);

	if(MENU_stage == 0){
		//Write_time_to_RTC (0x02, 0x21, 0x10, 0x25, 0x22, 0x42, 0x10);		// test BCD format
		MENU_stage = 1;
		ST7789_FillScreen(background_color);
		ST7789_DrawLine(0, 25, 239, 25, YELLOW);
	}

	ST7789_DrawString_10x16_background(50, 120, "MENU TIME SET", WHITE, background_color);
}
