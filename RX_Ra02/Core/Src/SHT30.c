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

#include "st7789.h"


extern I2C_HandleTypeDef hi2c1;


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


uint8_t mesurement_t_h_SHT30 (float* temperature, float* humidity){
	//-----------command for SHT30 ------------------------
	uint8_t soft_reset_cmd[2] 		= {0x30, 0xA2};
	uint8_t mesurement_cmd[2] 		= {0x24, 0x00};	// Repeatability (LSB) High 0x06, Medium 0x0D, Low 0x10
//	uint8_t heater_enable_cmd [2]	= {0x30, 0x6D};
//	uint8_t heater_disable_cmd[2]	= {0x30, 0x66};
//	uint8_t read_status_reg_cmd[2]	= {0xF3, 0x2D};
//	uint8_t clear_status_reg_cmd[2]	= {0x30, 0x41};

	HAL_I2C_Master_Transmit(& hi2c1, SHT30_i2c_addr, soft_reset_cmd, 2, 100);
	HAL_Delay(2);
	HAL_I2C_Master_Transmit(& hi2c1, SHT30_i2c_addr, mesurement_cmd, 2, 100);
	HAL_Delay(16);

	uint8_t pData[6] = {0};
	HAL_I2C_Master_Receive(& hi2c1, SHT30_i2c_addr, pData, 6, 100);

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
	uint16_t temp_raw = pData[0] << 8 + pData[1];
	uint16_t hum_raw  = pData[3] << 8 + pData[4];

	* temperature = -45.0f + 175.0f * ((float)temp_raw / 65535.0f);
	* humidity = 100.0f * ((float)hum_raw / 65535.0f);

	return 1;
}

void displayed_t_h (void){
	float temperature =0;
	float humidity = 0;
	mesurement_t_h_SHT30 (& temperature, & humidity);

	char buff [24] = {0};
	sprintf (buff, "T = %.2f" , temperature);

	ST7789_DrawString_10x16 (95, 45, buff, YELLOW);


}
