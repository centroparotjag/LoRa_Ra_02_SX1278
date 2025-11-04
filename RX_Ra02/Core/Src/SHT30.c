/*
 * STH30.c
 *
 *  Created on: Oct 15, 2025
 *      Author: centr
 */

#include "SHT30.h"
#include <stddef.h> // For size_t
#include <stdint.h> // For uint8_t
#include "stm32f4xx_hal.h"
#include "config.h"
#include "display.h"
#include <stdio.h>

#include "st7789.h"
extern  defaultTaskHandle;

float humidity_on_board = 0;

extern I2C_HandleTypeDef hi2c1;
extern uint8_t MENU_stage;
extern uint8_t RTC_view;




// Function to calculate CRC-8/NRSC-5
uint8_t calculate_crc8_nrsc5(const uint8_t *data, size_t length) {
    uint8_t crc = 0xFF; // Initial CRC value
    for (size_t i = 0; i < length; i++) {
        crc ^= data[i]; // XOR with current data byte

        for (int j = 0; j < 8; j++) {
            if ((crc & 0x80) != 0) { // Check if MSB is set
                crc = (uint8_t)((crc << 1) ^ 0x31); // Shift and XOR with polynomial
            } else {
                crc <<= 1; // Just shift
            }
        }
    }
    return crc; // No final XOR applied as it's 0x00
}



//float temperature = -45.0f + 175.0f * ((float)temp_raw / 65535.0f);
//float humidity = 100.0f * ((float)hum_raw / 65535.0f);


void SHT30_heater (uint8_t onoff){
	uint8_t heater_enable_cmd [2]	= {0x30, 0x6D};
	uint8_t heater_disable_cmd[2]	= {0x30, 0x66};

	if (onoff == 0){
		HAL_I2C_Master_Transmit(& hi2c1, SHT30_i2c_addr, heater_disable_cmd, 2, 500);
	}
	else {
		HAL_I2C_Master_Transmit(& hi2c1, SHT30_i2c_addr, heater_enable_cmd, 2, 500);
	}
}

uint16_t SHT30_read_status_reg (void){
	uint8_t read_status_reg_cmd[2]	= {0xF3, 0x2D};
	uint16_t status_reg = 0;

	HAL_I2C_Master_Transmit(& hi2c1, SHT30_i2c_addr, read_status_reg_cmd, 2, 500);
	HAL_Delay(16);

	uint8_t pData[3] = {0};
	HAL_I2C_Master_Receive(& hi2c1, SHT30_i2c_addr, pData, 3, 500);

	// CRC check
	uint8_t data_t_crc[2] = {pData[0], pData[1]};
	if (pData[2] !=  calculate_crc8_nrsc5(data_t_crc, 2) ){
		return 0;
	}

	status_reg |= (pData[0]<<8) | pData[1];
	return status_reg;
}

uint8_t mesurement_t_h_SHT30 (float* temperature, float* humidity){
	//-----------command for SHT30 ------------------------
	uint8_t soft_reset_cmd[2] 		= {0x30, 0xA2};
	uint8_t mesurement_cmd[2] 		= {0x24, 0x00};	// Repeatability (LSB) High 0x06, Medium 0x0D, Low 0x10
//	uint8_t heater_enable_cmd [2]	= {0x30, 0x6D};
//	uint8_t heater_disable_cmd[2]	= {0x30, 0x66};
//	uint8_t read_status_reg_cmd[2]	= {0xF3, 0x2D};
//	uint8_t clear_status_reg_cmd[2]	= {0x30, 0x41};

	//--------------------------------------------------
	uint16_t status = SHT30_read_status_reg ();
	if ((status & 0x2000) != 0x2000){
		HAL_I2C_Master_Transmit(& hi2c1, SHT30_i2c_addr, soft_reset_cmd, 2, 500);
		HAL_Delay(2);
	}

	HAL_I2C_Master_Transmit(& hi2c1, SHT30_i2c_addr, mesurement_cmd, 2, 500);
	HAL_Delay(16);

	uint8_t pData[6] = {0};
	HAL_I2C_Master_Receive(& hi2c1, SHT30_i2c_addr, pData, 6, 500);

	// CRC check
	uint8_t data_t_crc[2] = {pData[0], pData[1]};
	if (pData[2] !=  calculate_crc8_nrsc5(data_t_crc, 2) ){
		return 0;
	}

	uint8_t data_h_crc[2] = {pData[3], pData[4]};
	if (pData[5] !=  calculate_crc8_nrsc5(data_h_crc, 2) ){
		return 0;
	}

	//------------- convert raw data to T, h----------------------------------
	uint16_t temp_raw = 0;
	uint16_t hum_raw  = 0;

	temp_raw |= (pData[0] << 8) | pData[1];
	hum_raw  |= (pData[3] << 8) | pData[4];

	* temperature = (175.0f * ((float)temp_raw / 65535.0f)) - 45.0f;
	* humidity = 100.0f * ((float)hum_raw / 65535.0f);

	return 1;
}

extern uint16_t background_color;




void displayed_t_h (void){
	float temperature =0;

	uint8_t crc_status = mesurement_t_h_SHT30 (& temperature, & humidity_on_board);
	background_color = RGB565(95,158,160);

	if(MENU_stage == 0){
		vTaskSuspend(defaultTaskHandle);			//
		ST7789_FillScreen(background_color);
		//ST7789_FillScreen(background_color);
		ST7789_DrawLine(0, 25, 239, 25, YELLOW);
		MENU_stage = 1;
	}


	char buff [24] = {0};
	sprintf (buff, "T = %.2f  h = %.2f   " , temperature, humidity_on_board);
	ST7789_DrawString_10x16_background(5, 45, buff, GREEN, background_color);


	if (crc_status) {
		ST7789_DrawString_10x16_background (5, 65, "CRC OK     ", GREEN, background_color);
	}
	else {
		ST7789_DrawString_10x16_background (5, 65, "CRC - ERROR", RED, background_color);
	}

	//--------------- reg status --------------------------
	uint16_t status = SHT30_read_status_reg ();

	uint8_t Alert_pending_15r 	= (status & 0x8000) ? 1 : 0;
	uint8_t Heater_13r			= (status & 0x2000) ? 1 : 0;
	uint8_t RH_alert_10r		= (status & 0x0800) ? 1 : 0;
	uint8_t System_reset_4r		= (status & 0x0010) ? 1 : 0;
	uint8_t Command_status_1r	= (status & 0x0002) ? 1 : 0;
	uint8_t checksum_status_0r	= (status & 0x0001) ? 1 : 0;


	char pBuff [24];
	ST7789_DrawString_10x16 (5, 85, "Status register SHT30", CYAN);

	ST7789_DrawRectangle(0, 111, 239, 239, CYAN);
	ST7789_DrawRectangle(1, 110, 238, 238, CYAN);

	sprintf(pBuff, "Alert pending [15] = %d", Alert_pending_15r);
	ST7789_DrawString_10x16_background (10, 115, pBuff, MAGENTA, background_color);

	sprintf(pBuff, "Heater        [13] = %d", Heater_13r);
	ST7789_DrawString_10x16_background (10, 135, pBuff, MAGENTA, background_color);

	sprintf(pBuff, "RH alert      [10] = %d", RH_alert_10r);
	ST7789_DrawString_10x16_background (10, 155, pBuff, MAGENTA, background_color);

	sprintf(pBuff, "System reset   [4] = %d", System_reset_4r);
	ST7789_DrawString_10x16_background (10, 175, pBuff, MAGENTA, background_color);

	sprintf(pBuff, "Command status [1] = %d", Command_status_1r);
	ST7789_DrawString_10x16_background (10, 195, pBuff, MAGENTA, background_color);

	sprintf(pBuff, "checksum status[0] = %d", checksum_status_0r);
	ST7789_DrawString_10x16_background (10, 215, pBuff, MAGENTA, background_color);


	RTC_view = 1;
	vTaskResume(defaultTaskHandle);

}


