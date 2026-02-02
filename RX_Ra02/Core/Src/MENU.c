/*
 * MENU.c
 *
 *  Created on: Oct 20, 2025
 *      Author: centr
 */

#include "MENU.h"
#include "stm32f4xx_hal.h"
#include "DS3231.h"
#include "DS18B20.h"
#include "config.h"
#include "display.h"
#include <st7789.h>
#include "main.h"
#include "other functions.h"
#include "adc.h"
#include "fram.h"
#include <stdio.h>
#include "data_encoding.h"

# define DATA_LOST 	1800		//seconds 1800
# define RESTART_RX 300		//seconds 300

uint8_t MENU = 0;
uint8_t MENU_update = 1;
uint8_t MENU_stage = 0;
uint8_t state_but = 0;
uint8_t m_set = 0;
uint8_t set_pos = 8;
uint32_t sec;
uint8_t TX_presence = 0;
extern uint16_t background_color;
extern uint8_t RTC_view;
extern uint8_t m_counter;
extern uint8_t LoRa_RxBuffer[7];
extern uint8_t LoRa_receive_data;
extern int myLoRa;
extern uint32_t sec_time;
extern uint32_t RX_fix_time_sec;
float TMP_ds18b20 = 0;
extern float humidity_on_board;
float T_ob_fix = 1;
float H_ob_fix = 1;
extern float Ubat;

extern  defaultTaskHandle;

uint8_t flag_status_read_T_DS18B20 = 0;
void MENU_SELEKTOR (void){
	//------------ return to MENU_SET (1) -------------
	if (state_but == 0x02 && MENU != 0 && MENU != 1 && MENU != 5) {
		MENU = 1;
		MENU_update = 1;
		MENU_stage = 0;
		state_but = 8;
	}

	if (state_but == 0x02 && MENU == 5 && set_pos == 8) { // exit on MENU RTC CONFIG
		MENU = 1;
		MENU_update = 1;
		MENU_stage = 0;
		state_but = 8;
		m_set = 4;
	}

	if (state_but == 0x02 && MENU == 1 && m_set == 0) {
		MENU = 0;
		MENU_update = 1;
		MENU_stage = 0;
		state_but = 8;
		LoRa_receive_data = 1;
		T_ob_fix = 100;
		H_ob_fix = 100;
	}


	if (MENU == 0){
		RTC_view = 1;
		MENU_O ();
	}
    if(MENU_update == 1){

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
    		MENU_RTC ();
    	}
    	if (MENU == 6){			// DISPLAY
    		state_but = 8;
    		RTC_view = 0;
    		vTaskSuspend(defaultTaskHandle);
    		displaying_images_from_flash ();
    		vTaskResume(defaultTaskHandle);

    	}
    	MENU_update = 0;
    }


	//-------- read T DS18B20------------------------
    uint32_t count_sec_cicle = sec_time - RX_fix_time_sec;
	if (count_sec_cicle%60 == 2 || count_sec_cicle%60 == 28 ){
		Power_DS18B20 (1);
		flag_status_read_T_DS18B20 = 1;
	}
	if ((count_sec_cicle%60 == 3 || count_sec_cicle%60 == 29) && flag_status_read_T_DS18B20 == 1){
		START__DS18B20_CONVERT_TEMPERATURE ();
		flag_status_read_T_DS18B20 = 2;
	}
	if ((count_sec_cicle%60 == 4 || count_sec_cicle%60 == 30) && flag_status_read_T_DS18B20 == 2){
		uint16_t T_DS18B20 = READ_DS18B20_TEMPERATURE ();
		Power_DS18B20 (0);
		flag_status_read_T_DS18B20 = 0;
		if(T_DS18B20 <= 0x07D0){
			TMP_ds18b20 = T_DS18B20/16.0f;
		}
		else {
			TMP_ds18b20 = ((0x10000-T_DS18B20)/16.0f)*(-1);
		}
		MENU_update = 1;

	}
	if (count_sec_cicle%60 == 5 || count_sec_cicle%60 == 35 ){
		Power_DS18B20 (0);
		flag_status_read_T_DS18B20 = 0;
	}
	//-----------------------------------------------

}



//=========== SELEKTOR MENU 1  ===========================

void MENU_SET (void){

	if (MENU_stage == 0){
		vTaskSuspend(defaultTaskHandle);
		uint16_t color = RGB565(85,107,47);
		background_color = color;
		ST7789_FillScreen(color);
		//ST7789_FillScreen(color);
		ST7789_DrawLine(0, 22, 240, 22, YELLOW);
		ST7789_DrawString_10x16(30, 40, "BACK", WHITE);
		ST7789_DrawString_10x16(30, 65, "ADC", WHITE);
		ST7789_DrawString_10x16(30, 90, "SHT30", WHITE);
		ST7789_DrawString_10x16(30, 115, "STATISTICS", WHITE);
		ST7789_DrawString_10x16(30, 140, "TIME SET", WHITE);
		ST7789_DrawString_10x16(30, 165, "DISPLAY", WHITE);
		MENU_stage = 1;
	}

	//---------- buttons ---------------
	if (state_but == 0x01 && MENU != 5) {
		if (m_set == 0 ){
			m_set = 5;
		}
		else {
			m_set--;
		}
	}

	if (state_but == 0x03 && MENU != 5) {
		m_set++;
		if (m_set > 5){
			m_set = 0;
		}
	}

	if (state_but == 0x02 && MENU != 5) {
		if (m_set == 0){
			MENU = 0;
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
		ST7789_DrawRectangleFilled(10, 45, 20, 180, background_color);
		uint16_t cursor_color = RGB565(0,255,0);
		if (m_set == 0) {
			ST7789_DrawRectangleFilled(10, 45, 20, 55, cursor_color);
		}
		if (m_set == 1) {
			ST7789_DrawRectangleFilled(10, 70, 20, 80, cursor_color);
		}
		if (m_set == 2) {
			ST7789_DrawRectangleFilled(10, 95, 20, 105, cursor_color);
		}
		if (m_set == 3) {
			ST7789_DrawRectangleFilled(10, 120, 20, 130, cursor_color);
		}
		if (m_set == 4) {
			ST7789_DrawRectangleFilled(10, 145, 20, 155, cursor_color);
		}
		if (m_set == 5) {
			ST7789_DrawRectangleFilled(10, 170, 20, 180, cursor_color);
		}
	}
	RTC_view = 1;
	vTaskResume(defaultTaskHandle);
}




void MENU_O (void){
	char buff[24];
	background_color 		  	 = RGB565(0, 34, 51);

	uint16_t frame_color      	 = RGB565(170, 170, 85);
	uint16_t frame_fill_color 	 = RGB565(85, 85, 43);

	uint16_t frame_color_ob      = RGB565(170, 170, 85);
	uint16_t frame_fill_color_ob = RGB565(85, 85, 43);

	uint16_t col_t = GREEN;
	uint16_t col_h = GREEN;
	uint16_t col_V = GREEN;

	if(MENU_stage == 0){
		vTaskSuspend(defaultTaskHandle);
		ST7789_FillScreen(background_color);

		//----------- extern data -------
		ST7789_DrawRectangleFilled(0, 25, 239, 135, frame_fill_color);
		ST7789_DrawRectangle(0, 25, 239, 135, frame_color);
		ST7789_DrawRectangle(1, 26, 238, 134, frame_color);
		ST7789_DrawRectangle(2, 27, 237, 133, frame_color);

		ST7789_DrawLine(0, 65,  239, 65, frame_color);
		ST7789_DrawLine(0, 66,  239, 66, frame_color);

		ST7789_DrawLine(0, 105, 239, 105, frame_color);
		ST7789_DrawLine(0, 106, 239, 106, frame_color);

		ST7789_DrawLine(196, 65, 196, 105, frame_color);		// bat
		ST7789_DrawLine(197, 65, 197, 105, frame_color);

		ST7789_DrawLine(44, 105, 44, 135, frame_color);
		ST7789_DrawLine(45, 105, 45, 135, frame_color);

		ST7789_DrawLine(137, 105, 137, 135, frame_color);
		ST7789_DrawLine(138, 105, 138, 135, frame_color);

		if(sec>=DATA_LOST){			// data lost
			ST7789_DrawString_10x16_background(70, 37, "TX LOST  !!!", RGB565(255,0,0), frame_fill_color);
		}
		else {
			ST7789_DrawString_10x16_background(80, 37, "NO DATA", RGB565(150,150,150), frame_fill_color);
		}
		ST7789_DrawString_10x16_background(15, 77, "WAITING FOR DATA", RGB565(150,150,150), frame_fill_color);

		//----------- on board data -------
		ST7789_DrawRectangleFilled(0, 150, 239, 239, frame_fill_color_ob);
		ST7789_DrawRectangle(0, 150, 239, 239, frame_color_ob);
		ST7789_DrawRectangle(1, 151, 238, 238, frame_color_ob);
		ST7789_DrawRectangle(2, 152, 237, 237, frame_color_ob);

		ST7789_DrawLine(0, 194,  239, 194, frame_color_ob);
		ST7789_DrawLine(0, 195,  239, 195, frame_color_ob);

		ST7789_DrawLine(196, 194, 196, 237, frame_color);		// bat
		ST7789_DrawLine(197, 194, 197, 237, frame_color);

		MENU_stage = 1;
	}


	if (LoRa_receive_data == 1){
		//-------------------------------------------------------------------------------------------------
		ST7789_DrawRectangleFilled(4, 108, 43, 132, RED);
		ST7789_DrawString_10x16_background(15, 111, "RX", YELLOW, RED);

		//-------------------- decoding data ----------------------------
		uint8_t DecodedDataPacket[7]={0};
		uint8_t CRC_check = DecodingAndVerificationOfDataPacket (LoRa_RxBuffer, DecodedDataPacket);

		if (CRC_check == 1){
			TX_presence = 1;
			//------------- convert raw data to T, h----------------------------------
			uint16_t temp_raw = 0;
			uint16_t hum_raw  = 0;

			temp_raw |= DecodedDataPacket[0]<<8 | DecodedDataPacket[1];
			hum_raw  |= DecodedDataPacket[2]<<8 | DecodedDataPacket[3];

			float temperature = (175.0f * ((float)temp_raw / 65535.0f)) - 45.0f;
			float humidity = 100.0f * ((float)hum_raw / 65535.0f);
			float Vbat = (DecodedDataPacket[4]+250.0f)/100.0f;

			humidity = humidity_correction (humidity);

			//-------------------------------------------------------------------
			if(temperature>=28) 				{ col_t = YELLOW; }
			if(temperature<15)  				{ col_t = CYAN;   }
			if(humidity>=65)    				{ col_h = YELLOW;   }
			if(humidity<45)     				{ col_h = CYAN; }

			//---- T aligned -----------
			if(temperature <=-10 )	{sprintf (buff, "%.2f" , temperature);}
			if((temperature >-10 && temperature < 0) || temperature >= 10) {sprintf (buff, " %.2f" , temperature);}
			if(temperature >=0 && temperature < 10 ) {sprintf (buff, "  %.2f" , temperature);}

			ST7789_DrawString_26x30_background (10, 33, buff,  col_t, frame_fill_color);
			ST7789_DrawString_26x30_background (176, 33, "gC",  col_t, frame_fill_color);
			//-----------------------

			//---- h aligned -----------
			char b1[9]={0};
			if(humidity <10) {
				sprintf (b1, "  %.1f%% " , humidity);
			}
			else {
				sprintf (b1, " %.1f%%" , humidity);
			}
			ST7789_DrawString_26x30_background (10, 73, b1,  col_h, frame_fill_color);

			//----------- battery level ------------------
			battery_level_10_18 (210, 73, Vbat);

			//------ battery voltage -------------
			char b2[9]={0};
			if(Vbat < (BATT_LOW_VOLTAGE+0.2))  {
				sprintf (b2, "Low %.2fV" , Vbat);
				ST7789_DrawString_10x16_background(144, 111, b2, RED, frame_fill_color);
			 }
			else {
				sprintf (b2, "  %.2fV  " , Vbat);
				ST7789_DrawString_10x16_background(144, 111, b2, GREEN, frame_fill_color);
			}
			//-------------------------------------------------------------------------------------------------
			//ST7789_DrawString_10x16_background(15, 110, "  ", GREEN, frame_fill_color);
			ST7789_DrawRectangleFilled(3, 108, 44, 132, frame_fill_color);		// clear "RX"
			LoRa_receive_data = 0;

		}
		else {
//			ST7789_DrawRectangleFilled(4, 108, 43, 132, RED);
//			ST7789_DrawString_10x16_background(10, 111, "CRC", YELLOW, RED);
		}

	}

	//------- time after last data reception -----------------------------------------------------------
	if (RX_fix_time_sec == 0) {
		RX_fix_time_sec = sec_time;
	}

	if(sec_time < RX_fix_time_sec){
		RX_fix_time_sec = sec_time;
	}

	sec = sec_time - RX_fix_time_sec;
	uint8_t Hms[3]={0};
	char b[24]={0};
	convert_time_sec_to_H_m_s (sec, Hms);
	//for(uint8_t i=0; i<24; ++i) {buff[i]=0;}
	sprintf (b, "%02d:%02d:%02d", Hms[0], Hms[1], Hms[2]);
	ST7789_DrawString_10x16_background(53, 111, b, col_V, frame_fill_color);			//col_V

	//-------- ERROR LAST_RECEIVE_DATA ------------------------------------------------------------------

	if(sec%RESTART_RX == 0 && sec > 0 ) {
		LoRa_receive_data = 0;
		LoRa_reset(&myLoRa);
		LoRa_init(&myLoRa);
		LoRa_startReceiving(&myLoRa);

		if(TX_presence == 1){
			ST7789_DrawRectangleFilled(4, 108, 43, 132, RED);
			ST7789_DrawString_10x16_background(4, 111, "LOST", YELLOW, RED);

			if(sec==DATA_LOST){
				MENU_stage = 0;
				RX_fix_time_sec--;
				for(uint8_t i=0; i<7; i++){
					LoRa_RxBuffer[i] = 0;
				}

			}
		}
	}

	//====================== On board meteo ====================================
	if(MENU_update == 1){
		//-------------------------------------------------------------------
		uint16_t col_t_OB = GREEN;
		uint16_t col_h_OB = GREEN;

		if(TMP_ds18b20>=28) 				{ col_t_OB = YELLOW; }
		if(TMP_ds18b20<15)  				{ col_t_OB = CYAN;   }
		if(humidity_on_board>=65)    		{ col_h_OB = YELLOW;   }
		if(humidity_on_board<45)     		{ col_h_OB = CYAN; }

		//---- T aligned -----------
		for(uint8_t i=0; i<24; ++i){ buff[i]=0; }		// clear buffer
		if(T_ob_fix != TMP_ds18b20) {
			if(TMP_ds18b20 <=-10 )	{sprintf (buff, "%.2f" , TMP_ds18b20);}
			if((TMP_ds18b20 >-10 && TMP_ds18b20 < 0) || TMP_ds18b20 >= 10) {sprintf (buff, " %.2f" , TMP_ds18b20);}
			if(TMP_ds18b20 >=0 && TMP_ds18b20 < 10 ) {sprintf (buff, "  %.2f" , TMP_ds18b20);}

			ST7789_DrawString_26x30_background (10,  160, buff,  col_t_OB, frame_fill_color_ob);
			ST7789_DrawString_26x30_background (176, 160, "gC",  col_t_OB, frame_fill_color_ob);
			T_ob_fix = TMP_ds18b20;
		}

		//---- h aligned -----------
		for(uint8_t i=0; i<24; ++i){ buff[i]=0; }		// clear buffer
		if(H_ob_fix != humidity_on_board){
			if(humidity_on_board <10) {
				sprintf (buff, " %.1f%%" , humidity_on_board);
			}
			else {
				sprintf (buff, "%.1f%%" , humidity_on_board);
			}
			ST7789_DrawString_26x30_background (38, 204, buff,  col_h_OB, frame_fill_color_ob);
			H_ob_fix = humidity_on_board;
		}

		//----------- battery level ------------------
		battery_level_10_18 (210, 204, Ubat);



	}
	vTaskResume(defaultTaskHandle);
}

void MENU_STAT (void){
	uint32_t h=0;
	uint32_t m=0;
	char buff [24];

	background_color = RGB565(34, 100, 80);
	uint16_t main_color = RGB565(32,178,170);
	uint16_t outline_color = RGB565(255,255,255);

	if(MENU_stage == 0){
		vTaskSuspend(defaultTaskHandle);
		ST7789_FillScreen(background_color);
		//ST7789_FillScreen(background_color);
		//read_fram_into_terminal ();		// test!!!

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
		//ST7789_DrawLine(0, 25, 239, 25, outline_color);
		ST7789_DrawLine(0, 26, 239, 26, outline_color);
		//-----------------------------------------------

		ST7789_DrawRectangleFilled(0, 35, 239, 60, main_color);
		ST7789_DrawRectangle(0, 35, 239, 60, outline_color);
		ST7789_DrawString_10x16_background(5, 40, "p.o.c.", WHITE, main_color);

		ST7789_DrawRectangleFilled(0, 70, 239, 95, main_color);
		ST7789_DrawRectangle(0, 70, 239, 95, outline_color);
		ST7789_DrawString_10x16_background(5, 75, "w.t.", WHITE, main_color);

		ST7789_DrawRectangleFilled(0, 105, 239, 130, main_color);
		ST7789_DrawRectangle(0, 105, 239, 130, outline_color);
		ST7789_DrawString_10x16_background(5, 110, "rx.B.", WHITE, main_color);

		ST7789_DrawRectangleFilled(0, 165, 239, 239, main_color);
		ST7789_DrawRectangle(0, 165, 239, 239, outline_color);
		//ST7789_DrawString_10x16_background(100, 170, "FRAM", WHITE, main_color);
		MENU_stage = 1;
	}


	uint32_t power_on_counter = read_fram_count_init ();
	uint32_t working_times    = read_fram_count_time_on ();
	uint32_t received_byte    = read_fram_received_byte_counter ();

	h = working_times / 60;
	m = working_times%60;

	//-------------------------------------------------------------------------
	sprintf (buff, "%d", power_on_counter);
	ST7789_DrawString_10x16_background(70, 40, buff, YELLOW, main_color);

	sprintf (buff, "%dh %02dm", h, m);
	ST7789_DrawString_10x16_background(70, 75, buff, YELLOW, main_color);

	sprintf (buff, "%d", received_byte);
	ST7789_DrawString_10x16_background(70, 110, buff, YELLOW, main_color);


	//-------------------------------------------------------------------------
	sprintf (buff, "p.o.c. 0x%08X", power_on_counter);
	ST7789_DrawString_10x16_background(30, 175, buff, WHITE, main_color);

	sprintf (buff, "w.t.   0x%08X", working_times);
	ST7789_DrawString_10x16_background(30, 195, buff, WHITE, main_color);

	sprintf (buff, "rx.B.  0x%08X", received_byte);
	ST7789_DrawString_10x16_background(30, 215, buff, WHITE, main_color);

	vTaskResume(defaultTaskHandle);
}

uint8_t DWs, Ds, Ms, Ys, Hs, ms, ss = 0;


//------------- menu set rtc --------------------

void draw_position (uint8_t pos, uint16_t set_color, uint16_t background_color){
	uint8_t xp1, yp1, xp2, yp2 = 0;
	//----- clear pos -------
	for (uint8_t i = 0; i < 9; ++i) {
		switch (i) {
			case 0: xp1 = 8;   yp1 = 62;  xp2 = 40;  yp2 = 85;	break;
			case 1: xp1 = 58;  yp1 = 62;  xp2 = 90;  yp2 = 85;	break;
			case 2: xp1 = 127; yp1 = 62;  xp2 = 160; yp2 = 85;	break;
			case 3: xp1 = 198; yp1 = 62;  xp2 = 230; yp2 = 85;	break;
			case 4: xp1 = 58;  yp1 = 137; xp2 = 90;  yp2 = 160;	break;
			case 5: xp1 = 127; yp1 = 137; xp2 = 160; yp2 = 160;	break;
			case 6: xp1 = 198; yp1 = 137; xp2 = 230; yp2 = 160;	break;
			case 7: xp1 = 30;  yp1 = 200; xp2 = 98;  yp2 = 227;	break;
			case 8: xp1 = 150; yp1 = 200; xp2 = 205; yp2 = 227;	break;
		}
		ST7789_DrawRectangle(xp1, yp1, xp2, yp2, background_color);
		ST7789_DrawRectangle(xp1+1, yp1+1, xp2-1, yp2-1, background_color);
	}

	//---- draw pos ---------
	switch (pos) {
		case 0: xp1 = 8;   yp1 = 62;  xp2 = 40;  yp2 = 85;	break;
		case 1: xp1 = 58;  yp1 = 62;  xp2 = 90;  yp2 = 85;	break;
		case 2: xp1 = 127; yp1 = 62;  xp2 = 160; yp2 = 85;	break;
		case 3: xp1 = 198; yp1 = 62;  xp2 = 230; yp2 = 85;	break;
		case 4: xp1 = 58;  yp1 = 137; xp2 = 90;  yp2 = 160;	break;
		case 5: xp1 = 127; yp1 = 137; xp2 = 160; yp2 = 160;	break;
		case 6: xp1 = 198; yp1 = 137; xp2 = 230; yp2 = 160;	break;
		case 7: xp1 = 30;  yp1 = 200; xp2 = 98;  yp2 = 227;	break;
		case 8: xp1 = 150; yp1 = 200; xp2 = 205; yp2 = 227;	break;
	}

	ST7789_DrawRectangle(xp1, yp1, xp2, yp2, set_color);
	ST7789_DrawRectangle(xp1+1, yp1+1, xp2-1, yp2-1, set_color);
}

uint8_t convert2BCD(uint8_t hexData)
{
    uint8_t    bcdHI   = hexData/10;
    uint8_t    bcdLO   = hexData%10;
    uint8_t    bcdData = (bcdHI<<4)+bcdLO;

    return   bcdData;
  }


uint8_t lim_max_day_month_leap( uint8_t MONTH, uint8_t YEAR){
	uint8_t DAY = 31;
	if(MONTH == 2){ // February
		DAY = ((YEAR%4) == 0) ? 29 : 28;
	}
	if(MONTH == 4 || MONTH == 6 || MONTH == 9 || MONTH == 11){   // April, June, September, November
		DAY = 30;
	}
	return DAY;
}



void MENU_RTC (void){
	background_color = RGB565(80,110,34);
	uint16_t main_color = RGB565(60,90,24);
	uint16_t outline_color = RGB565(20,30,10);
	uint16_t text_color = RGB565(255,255,255);
	uint16_t legend_color = RGB565(110,140,54);

	uint8_t data[7]={0};
	char buff [24];

	if(MENU_stage == 0){
		vTaskSuspend(defaultTaskHandle);
		ST7789_FillScreen(background_color);
		//ST7789_FillScreen(background_color);
		//----------------- TIME ----------------------------

		for(uint8_t i =0; i<5; ++i){
			ST7789_DrawLine(0, 24+i*2, 239, 24+i*2, outline_color);
		}
		//-----------------------------------------------
		ST7789_DrawRectangleFilled(0, 40, 239, 90, main_color);
		ST7789_DrawRectangle      (0, 40, 239, 90, outline_color);
		ST7789_DrawRectangleFilled(0, 115, 239, 165, main_color);
		ST7789_DrawRectangle      (0, 115, 239, 165, outline_color);
		ST7789_DrawRectangleFilled(0, 189, 239, 239, main_color);
		ST7789_DrawRectangle      (0, 189, 239, 239, outline_color);
		read_data_time_DS3231 (data);
		DWs = data[3];	Ys = data[0];	Ms = data[1];	Ds= data[2];
						Hs = data[4];	ms = data[5];	ss = data[6];
		ST7789_DrawString_10x16_background(3,   42, "W.DAY DAY - MONTH -YEAR", legend_color, main_color);
		ST7789_DrawString_10x16_background(50, 117, "HOURS : MIN  : SEC",      legend_color, main_color);
		ST7789_DrawString_10x16_background(40, 205, "APPLY       EXIT",        text_color,   main_color);
		set_pos = 8;
		MENU_stage = 1;
		state_but = 8;
	}

	//------------------ set new data time -------------------------------
	if (set_pos == 0){		// DW
		if (state_but == 0x03) { DWs++; if (DWs >  7) {DWs = 1;} }
		if (state_but == 0x01) { DWs--; if (DWs == 0) {DWs = 7;} }
	}
	if (set_pos == 1){		// Ds
		uint8_t D_lim = lim_max_day_month_leap(Ms, Ys);
		if (state_but == 0x03) { Ds++; if (Ds >  D_lim) {Ds = 1;  } }
		if (state_but == 0x01) { Ds--; if (Ds == 0)  {Ds = D_lim; } }
	}
	if (set_pos == 2){		// Ms
		if (state_but == 0x03) { Ms++; if (Ms >  12) {Ms = 1;  } }
		if (state_but == 0x01) { Ms--; if (Ms == 0)  {Ms = 12; } }
	}
	if (set_pos == 3){		// Ys
		if (state_but == 0x03) { Ys++; if (Ys >  99) {Ys = 0;} }
		if (state_but == 0x01) { if (Ys == 0) {Ys = 99;} else {Ys--;} }
	}
	if (set_pos == 4){		// Hs
		if (state_but == 0x03) { Hs++; if (Hs >  23) {Hs = 0;} }
		if (state_but == 0x01) { if (Hs == 0) {Hs = 23;} else {Hs--;} }
	}
	if (set_pos == 5){		// ms
		if (state_but == 0x03) { ms++; if (ms >  59) {ms = 0;} }
		if (state_but == 0x01) { if (ms == 0) {ms = 59;} else {ms--;} }
	}
	if (set_pos == 6){		// ss
		if (state_but == 0x03) { ss++; if (ss >  59) {ss = 0;} }
		if (state_but == 0x01) { if (ss == 0) {ss = 59;} else {ss--;} }
	}
	//------------------- DAY WEEK-------------------------------------------
	uint8_t x_DW = 15; uint8_t y_DW = 65;
	switch (DWs) {
		case 1: ST7789_DrawString_10x16_background(x_DW, y_DW, "Mo", text_color, main_color);
			break;
		case 2: ST7789_DrawString_10x16_background(x_DW, y_DW, "Tu", text_color, main_color);
			break;
		case 3: ST7789_DrawString_10x16_background(x_DW, y_DW, "We", text_color, main_color);
			break;
		case 4: ST7789_DrawString_10x16_background(x_DW, y_DW, "Th", text_color, main_color);
			break;
		case 5: ST7789_DrawString_10x16_background(x_DW, y_DW, "Fr", text_color, main_color);
			break;
		case 6: ST7789_DrawString_10x16_background(x_DW, y_DW, "Sa", text_color, main_color);
			break;
		case 7: ST7789_DrawString_10x16_background(x_DW, y_DW, "Su", text_color, main_color);
			break;
	}

	//-------------------- DATA TIME  -----------------------------------------------
	sprintf(buff, "%02d  -  %02d  -  %02d", Ds, Ms, Ys);
	ST7789_DrawString_10x16_background(x_DW + 50, y_DW, buff, text_color, main_color);

	sprintf(buff, "%02d  :  %02d  :  %02d", Hs, ms, ss);
	ST7789_DrawString_10x16_background(x_DW + 50, 140, buff, text_color, main_color);

	//----- button - ----------------
	if (state_but == 0x01 && (set_pos == 7 || set_pos==8)) {
		if (set_pos == 0 ){
			set_pos = 8;
		}
		else {
			set_pos--;
		}
		MENU_update = 1;
	}

	//----- button + ----------------
	if (state_but == 0x03 && (set_pos == 7 || set_pos==8)) {
		set_pos++;
		if (set_pos > 8) {set_pos = 0;}
		MENU_update = 1;
	}
	//----- button push ---------------- uint8_t DWs, Ds, Ms, Ys, Hs, ms, ss = 0;
	if (state_but == 0x02 && set_pos == 7) {			// save time -APPLY
		uint8_t Day = convert2BCD(Ds);
		uint8_t Mon = convert2BCD(Ms);
		uint8_t Ye  = convert2BCD(Ys);
		uint8_t Ho  = convert2BCD(Hs);
		uint8_t min = convert2BCD(ms);
		uint8_t sec = convert2BCD(ss);
		Write_time_to_RTC (DWs, Day, Mon, Ye, Ho, min, sec);		// test BCD format
		HAL_Delay(100);
		set_pos = 8;
		MENU_update = 1;
		state_but = 8;
	}
	//----- button push -----
	if (state_but == 0x02 && set_pos == 6) {set_pos = 7; MENU_update = 1;}
	if (state_but == 0x02 && set_pos == 5) {set_pos = 6; MENU_update = 1;}
	if (state_but == 0x02 && set_pos == 4) {set_pos = 5; MENU_update = 1;}
	if (state_but == 0x02 && set_pos == 3) {set_pos = 4; MENU_update = 1;}
	if (state_but == 0x02 && set_pos == 2) {set_pos = 3; MENU_update = 1;}
	if (state_but == 0x02 && set_pos == 1) {set_pos = 2; MENU_update = 1;}
	if (state_but == 0x02 && set_pos == 0) {set_pos = 1; MENU_update = 1;}
	//---------------------- POSITION ---------------------------------------------------
	uint16_t set_color = RGB565(255,127,80);
	draw_position (set_pos, set_color, main_color);
	vTaskResume(defaultTaskHandle);
}

float humidity_correction (float measured_humidity){

	if (measured_humidity <= 0) {
		return 0;
	}

	float k = (100-measured_humidity)/2;
	float Hc = 0;

	if (measured_humidity <= k){
		Hc = 0;
	}
	else {
		Hc = measured_humidity-k;
	}

	return Hc;
}






