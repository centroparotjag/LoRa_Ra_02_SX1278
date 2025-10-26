/*
 * SHT30.c
 *
 * Created: 26.10.2025 13:44:06
 *  Author: centr
 */ 
#include "SHT30.h"
#include "i2c.h"
#include "init.h"
#include <util/delay.h>
#include <avr/io.h>

// Function to calculate CRC-8/NRSC-5
uint8_t calculate_crc8_nrsc5(const uint8_t* data) {
	uint8_t crc = 0xFF; // Initial CRC value
	for (uint8_t i = 0; i < 2; i++) {
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
	uint8_t heater_enable_cmd [2]	=  {0x30,0x6D};
	uint8_t heater_disable_cmd[2]	=  {0x30,0x66};

	if (onoff == 0){
		WRITE_DATA_I2C (SHT30_ADDR_I2C, heater_enable_cmd, 2);
	}
	else {
		WRITE_DATA_I2C (SHT30_ADDR_I2C, heater_disable_cmd, 2);
	}
}

uint16_t SHT30_read_status_reg (void){
	uint8_t read_status_reg_cmd[2]	= {0xF3, 0x2D};
	uint16_t status_reg = 0;

	//HAL_I2C_Master_Transmit(& hi2c1, SHT30_i2c_addr, read_status_reg_cmd, 2, 100);
	//HAL_Delay(16);
	
	WRITE_DATA_I2C (SHT30_ADDR_I2C, read_status_reg_cmd, 2);
	//_delay_ms(1); // Small delay
	

	uint8_t pData[3] = {0};
	//HAL_I2C_Master_Receive(& hi2c1, SHT30_i2c_addr, pData, 3, 100);
	READ_DATA_I2C (SHT30_ADDR_I2C, pData, 3);

	// CRC check
	uint8_t data_t_crc[2] = {pData[0], pData[1]};
	if (pData[2] !=  calculate_crc8_nrsc5(data_t_crc) ){
		return 0;
	}

	status_reg |= (pData[0]<<8) | pData[1];
	return status_reg;
}

uint8_t mesurement_t_h_SHT30 (uint8_t* Th){
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
		//HAL_I2C_Master_Transmit(& hi2c1, SHT30_i2c_addr, soft_reset_cmd, 2, 100);
		//HAL_Delay(2);
		WRITE_DATA_I2C (SHT30_ADDR_I2C, soft_reset_cmd, 2);
		_delay_ms(2); // Small delay
	}

	//HAL_I2C_Master_Transmit(& hi2c1, SHT30_i2c_addr, mesurement_cmd, 2, 100);
	//HAL_Delay(16);
	WRITE_DATA_I2C (SHT30_ADDR_I2C, mesurement_cmd, 2);
	_delay_ms(16); // Small delay

	uint8_t pData[6] = {0};
	//HAL_I2C_Master_Receive(& hi2c1, SHT30_i2c_addr, pData, 6, 100);
	READ_DATA_I2C (SHT30_ADDR_I2C, pData, 6);

	// CRC check
	uint8_t data_t_crc[2] = {pData[0], pData[1]};
	if (pData[2] !=  calculate_crc8_nrsc5(data_t_crc) ){
		return 0;
	}

	uint8_t data_h_crc[2] = {pData[3], pData[4]};
	if (pData[5] !=  calculate_crc8_nrsc5(data_h_crc) ){
		return 0;
	}

	//------------- convert raw data to T, h----------------------------------
	//uint16_t temp_raw=0; 
	//uint16_t hum_raw=0;
	//temp_raw |= (pData[0] << 8) | pData[1];
	//hum_raw  |= (pData[3] << 8) | pData[4];

	//* temperature = (175.0f * ((float)temp_raw / 65535.0f)) - 45.0f;
	//* humidity = 100.0f * ((float)hum_raw / 65535.0f);
	Th[0] = pData[0];
	Th[1] = pData[1];
	Th[2] = pData[3];
	Th[3] = pData[4];
	return 1;
}
