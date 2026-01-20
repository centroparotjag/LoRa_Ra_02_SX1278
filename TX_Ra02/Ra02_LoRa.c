/*
 * Ra02_LoRa.c
 *
 * Created: 26.10.2025 20:20:22
 *  Author: centr
 * https://github.com/SMotlaq/LoRa
 */ 

#include "Ra02_LoRa.h"
#include <avr/io.h>
#include "SPI.h"
#include "init.h"

#define frequency               470       
#define spredingFactor          SF_7      
#define bandWidth			    BW_31_25KHz 
#define crcRate                 CR_4_5    
#define power				    POWER_20db // POWER_20db
#define overCurrentProtection   130       
#define preamble			    9         

void LoRa_reset(void){
	RST_Ra02(0);
	_delay_ms(1);  
	RST_Ra02(1);
	_delay_ms(100); // Small delay for stabilization  
}

void LoRa_gotoMode(uint8_t mode){
	uint8_t    read;
	uint8_t    data;

	read = LoRa_read( RegOpMode);
	data = read;

	if(mode == SLEEP_MODE){
		data = (read & 0xF8) | 0x00;
		current_mode = SLEEP_MODE;
	}else if (mode == STNBY_MODE){
		data = (read & 0xF8) | 0x01;
		current_mode = STNBY_MODE;
	}else if (mode == TRANSMIT_MODE){
		data = (read & 0xF8) | 0x03;
		current_mode = TRANSMIT_MODE;
	}else if (mode == RXCONTIN_MODE){
		data = (read & 0xF8) | 0x05;
		current_mode = RXCONTIN_MODE;
	}else if (mode == RXSINGLE_MODE){
		data = (read & 0xF8) | 0x06;
		current_mode = RXSINGLE_MODE;
	}

	LoRa_write(RegOpMode, data);
	_delay_ms(10); 
}


void LoRa_readReg(uint8_t* address, uint16_t r_length, uint8_t* output, uint16_t w_length){
	CS_Ra02(0);
	
	for (uint8_t i = 0; i<r_length; ++i){
		SPI_MasterTransmit(address[i]);
	}

	for (uint8_t i = 0; i<r_length; ++i){
		output[i] = SPI_MasterTransmit(0x00);
	}
	
	CS_Ra02(1);
}


void LoRa_writeReg(uint8_t* address, uint16_t r_length, uint8_t* values, uint16_t w_length){
	
	//HAL_GPIO_WritePin(_LoRa->CS_port, _LoRa->CS_pin, GPIO_PIN_RESET);
	CS_Ra02(0);
	
	
	//HAL_SPI_Transmit(_LoRa->hSPIx, address, r_length, TRANSMIT_TIMEOUT);
	//while (HAL_SPI_GetState(_LoRa->hSPIx) != HAL_SPI_STATE_READY);
		for (uint8_t i = 0; i<r_length; ++i){
			SPI_MasterTransmit(address[i]);
		}
	
	//HAL_SPI_Transmit(_LoRa->hSPIx, values, w_length, TRANSMIT_TIMEOUT);
	//while (HAL_SPI_GetState(_LoRa->hSPIx) != HAL_SPI_STATE_READY);
		for (uint8_t i = 0; i<w_length; ++i){
			SPI_MasterTransmit(values[i]);
		}
	
	
	//HAL_GPIO_WritePin(_LoRa->CS_port, _LoRa->CS_pin, GPIO_PIN_SET);
	CS_Ra02(1);
}


void LoRa_setLowDaraRateOptimization(uint8_t value){
	uint8_t	data;
	uint8_t	read;

	read = LoRa_read(RegModemConfig3);

	if(value)
		data = read | 0x08;
	else
		data = read & 0xF7;

	LoRa_write(RegModemConfig3, data);
	_delay_ms(10); 
}


void LoRa_setAutoLDO(void){
	double BW[] = {7.8, 10.4, 15.6, 20.8, 31.25, 41.7, 62.5, 125.0, 250.0, 500.0};

	LoRa_setLowDaraRateOptimization((long)((1 << spredingFactor) / ((double)BW[bandWidth])) > 16.0);
}


void LoRa_setFrequency(int16_t freq){
	uint8_t  data;
	uint32_t F;
	F = (freq * 524288)>>5;

	// write Msb:
	data = F >> 16;
	LoRa_write( RegFrMsb, data);
	_delay_ms(5); 

	// write Mid:
	data = F >> 8;
	LoRa_write( RegFrMid, data);
	_delay_ms(5);

	// write Lsb:
	data = F >> 0;
	LoRa_write(RegFrLsb, data);
	_delay_ms(5);
}


void LoRa_setSpreadingFactor(uint8_t SF){
	uint8_t	data;
	uint8_t	read;

	if(SF>12)
		SF = 12;
	if(SF<7)
		SF = 7;

	read = LoRa_read(RegModemConfig2);
	_delay_ms(10);

	data = (SF << 4) + (read & 0x0F);
	LoRa_write( RegModemConfig2, data);
	_delay_ms(10);

	LoRa_setAutoLDO();
}


void LoRa_setPower( uint8_t p){
	LoRa_write(RegPaConfig, p);
	_delay_ms(10);
}


void LoRa_setOCP(uint8_t current){
	uint8_t	OcpTrim = 0;

	if(current<45)
		current = 45;
	if(current>240)
		current = 240;

	if(current <= 120)
		OcpTrim = (current - 45)/5;
	else if(current <= 240)
		OcpTrim = (current + 30)/10;

	OcpTrim = OcpTrim + (1 << 5);
	LoRa_write(RegOcp, OcpTrim);
	_delay_ms(10);
}


void LoRa_setTOMsb_setCRCon(void){
	uint8_t read, data;

	read = LoRa_read(RegModemConfig2);

	data = read | 0x07;
	LoRa_write(RegModemConfig2, data);
	_delay_ms(10);
}


void LoRa_setSyncWord(uint8_t syncword){
	LoRa_write(RegSyncWord, syncword);
	_delay_ms(10);
}


uint8_t LoRa_read(uint8_t address){
	uint8_t read_data;
	uint8_t data_addr;

	data_addr = address & 0x7F;
	LoRa_readReg(&data_addr, 1, &read_data, 1);
	//_delay_ms(5);

	return read_data;
}


void LoRa_write(uint8_t address, uint8_t value){
	uint8_t data;
	uint8_t addr;

	addr = address | 0x80;
	data = value;
	LoRa_writeReg( &addr, 1, &data, 1);
	//_delay_ms(5);
}


void LoRa_BurstWrite(uint8_t address, uint8_t *value, uint8_t length){
	uint8_t addr;
	addr = address | 0x80;
	CS_Ra02(0);
	SPI_MasterTransmit(addr);
	//Write data in FiFo
	for (uint8_t i = 0; i<length; ++i){
		SPI_MasterTransmit(value[i]);
	}
	//HAL_Delay(5);
	CS_Ra02(1);
}

uint8_t LoRa_isvalid(void){
	return 1;
}

uint8_t LoRa_transmit(uint8_t* data, uint8_t length, uint16_t timeout){
	uint8_t read;
	int mode = current_mode;
	LoRa_gotoMode(STNBY_MODE);
	read = LoRa_read(RegFiFoTxBaseAddr);
	LoRa_write(RegFiFoAddPtr, read);
	LoRa_write(RegPayloadLength, length);
	LoRa_BurstWrite(RegFiFo, data, length);
	LoRa_gotoMode(TRANSMIT_MODE);
	while(1){
		read = LoRa_read(RegIrqFlags);
		if((read & 0x08)!=0){
			LoRa_write( RegIrqFlags, 0xFF);
			LoRa_gotoMode(mode);
			return 1;
		}
		else{
			if(--timeout==0){
				LoRa_gotoMode(mode);
				return 0;
			}
		}
		_delay_ms(2);
	}
}


void LoRa_startReceiving(void){
	LoRa_gotoMode(RXCONTIN_MODE);
}


uint8_t LoRa_receive(uint8_t* data, uint8_t length){
	uint8_t read;
	uint8_t number_of_bytes;
	uint8_t min = 0;

	for(int i=0; i<length; i++)
	data[i]=0;

	LoRa_gotoMode(STNBY_MODE);
	read = LoRa_read(RegIrqFlags);
	if((read & 0x40) != 0){
		LoRa_write(RegIrqFlags, 0xFF);
		number_of_bytes = LoRa_read( RegRxNbBytes);
		read = LoRa_read(RegFiFoRxCurrentAddr);
		LoRa_write(RegFiFoAddPtr, read);
		min = length >= number_of_bytes ? number_of_bytes : length;
		for(int i=0; i<min; i++)
		data[i] = LoRa_read(RegFiFo);
	}
	LoRa_gotoMode(RXCONTIN_MODE);
	return min;
}


int LoRa_getRSSI(void){
	uint8_t read;
	read = LoRa_read(RegPktRssiValue);
	return -164 + read;
}


uint16_t LoRa_init(void){
	uint8_t    data;
	uint8_t    read;

	if(LoRa_isvalid()){
		// goto sleep mode:
			LoRa_gotoMode(SLEEP_MODE);
			_delay_ms(10);

		// turn on LoRa mode:
			read = LoRa_read(RegOpMode);
			_delay_ms(10);
			data = read | 0x80;
			LoRa_write(RegOpMode, data);
			_delay_ms(100);

		// set frequency:
			LoRa_setFrequency(frequency);

		// set output power gain:
			LoRa_setPower(power);

		// set over current protection:
			LoRa_setOCP(overCurrentProtection);

		// set LNA gain:
			LoRa_write(RegLna, 0x23);

		// set spreading factor, CRC on, and Timeout Msb:
			LoRa_setTOMsb_setCRCon();
			LoRa_setSpreadingFactor(spredingFactor);

		// set Timeout Lsb:
			LoRa_write(RegSymbTimeoutL, 0xFF);

		// set bandwidth, coding rate and expilicit mode:
			// 8 bit RegModemConfig --> | X | X | X | X | X | X | X | X |
			//       bits represent --> |   bandwidth   |     CR    |I/E|
			data = 0;
			data = (bandWidth << 4) + (crcRate << 1);
			LoRa_write(RegModemConfig1, data);
			LoRa_setAutoLDO();

		// set preamble:
			LoRa_write(RegPreambleMsb, preamble >> 8);
			LoRa_write(RegPreambleLsb, preamble >> 0);

		// DIO mapping:   --> DIO: RxDone
			read = LoRa_read(RegDioMapping1);
			data = read | 0x3F;
			LoRa_write(RegDioMapping1, data);

		// goto standby mode:
			LoRa_gotoMode(STNBY_MODE);
			current_mode = STNBY_MODE;
			_delay_ms(10);

			read = LoRa_read(RegVersion);
			if(read == 0x12)
				return LORA_OK;
			else
				return LORA_NOT_FOUND;
	}
	else {
		return LORA_UNAVAILABLE;
	}
}

uint8_t Ra_02_pressence (void){
	uint8_t read = LoRa_read(RegVersion);
	if(read == 0x12)
		return 1;
	else
		return 0;
}



void LoRa_transmit_main_data (uint8_t* data, uint8_t len){
	POW_3V3_en(1);
	_delay_ms(1);
	SPI_MasterInit();
	LoRa_reset();
	LoRa_init();
	
	LoRa_transmit(data, len, 1000);
	
	POW_3V3_en(0);
	SPI_MasterDeInit();
}
	
	


