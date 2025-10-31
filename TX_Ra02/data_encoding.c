/*
 * data_encoding.c
 *
 * Created: 30.10.2025 17:34:59
 *  Author: centr
 */ 

#include <stdio.h>
#include "data_encoding.h"

uint8_t volatile counter_code = 0x55;		// initial

uint8_t table_coding[16] = {0xE5, 0x59, 0xA3, 0x7B, 0xF4, 0x2C, 0xD6, 0x33,
							0x49, 0x2D, 0x42, 0x16, 0x8C, 0xB7, 0x26, 0x95}; // keys - XOR

//-------------------------------- CRC -------------------------------------
uint8_t calculate_crc8_coding(const uint8_t *data, uint8_t length) {
	uint8_t crc = 0xA5; // Initial CRC value
	for (uint8_t i = 0; i < length; i++) {
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

//-------------------------------------------------------------------------
void encoding_decoding_data (uint8_t key_init, uint8_t* data_in, uint8_t* data_out, uint8_t len){
	for(uint8_t i = 0; i<len; ++i){
		data_out[i] = data_in[i] ^ table_coding[(key_init + i) & 0x0F];
	}
}

//----------------- Start num key-XOR encoding/decoding ------------------
uint8_t encoding_key_init (uint8_t key_cnt){
	uint8_t key_eki  = 0x09;
	uint8_t key_init = 0;
	key_cnt &= 0x0F;
	uint8_t upper_half_byte = (key_cnt ^ key_eki) & 0x0F;
	uint8_t lower_half_byte = (key_cnt ^ table_coding[upper_half_byte]) & 0x0F;
	key_init |= upper_half_byte<<4 | lower_half_byte;
	return key_init;
}

uint8_t decoding_key_init (uint8_t key_init_encoded){
	uint8_t upper_half_byte = (key_init_encoded >> 4) & 0x0F;
	uint8_t lower_half_byte = key_init_encoded & 0x0F;
	lower_half_byte = (lower_half_byte ^ table_coding[upper_half_byte]) & 0x0F; 			// decoded lower_half_byte
	return lower_half_byte;
}

//-----------------------------------------------------------------------
void DataEncoding_FormationTransmittedPackets (uint8_t* DATA_RAW, uint8_t* EncodedDataPacket){
	uint8_t KEY_NUM = encoding_key_init (counter_code);
	encoding_decoding_data (counter_code, DATA_RAW, EncodedDataPacket, 5);
	EncodedDataPacket[5] = KEY_NUM;
	uint8_t CRC_8 = calculate_crc8_coding(EncodedDataPacket, 6);
	EncodedDataPacket[6] = CRC_8;
	//counter_code++;
	
	//======== counter code randominaiser ===================
	uint16_t KeyCounter = 0;
	for(uint8_t i = 0; i<7; ++i){
		KeyCounter += DATA_RAW [i];
	}
	
	if((uint8_t)KeyCounter == counter_code) {
		counter_code++;
	}
	else {
		counter_code = KeyCounter;
	}
}


