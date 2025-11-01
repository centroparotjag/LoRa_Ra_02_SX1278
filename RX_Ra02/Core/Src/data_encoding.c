/*
 * data_encoding.c
 *
 *  Created on: Oct 30, 2025
 *      Author: centr
 */

#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "data_encoding.h"


uint8_t counter_code = 0x55;

uint8_t table_coding[16] = {0xE5, 0x59, 0xA3, 0x7B, 0xF4, 0x2C, 0xD6, 0x33,
							0x49, 0x2D, 0x42, 0x16, 0x8C, 0xB7, 0x26, 0x95};


uint8_t calculate_crc8_coding(const uint8_t *data, size_t length) {
    uint8_t crc = 0xA5; // Initial CRC value
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


void encoding_decoding_data (uint8_t key_init, uint8_t* data_in, uint8_t* data_out, uint8_t len){
	for(uint8_t i = 0; i<len; ++i){
		data_out[i] = data_in[i] ^ table_coding[(key_init + i) & 0x0F];
	}
}

#include <st7789.h>
extern uint16_t background_color;

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

uint8_t DecodingAndVerificationOfDataPacket (uint8_t* DATA_RAW, uint8_t* DecodedDataPacket){
	uint8_t key_init = decoding_key_init (DATA_RAW[5]);
	encoding_decoding_data (key_init, DATA_RAW, DecodedDataPacket, 5);
	//--------------- check CRC -------------------------------
	uint8_t CRC_calc = calculate_crc8_coding(DATA_RAW, 6);
	if(CRC_calc == DATA_RAW[6]){
		return 1;
	}
	return 0;
}

uint8_t fast_CRC_check (uint8_t* DATA_RAW){
	//--------------- check CRC -------------------------------
	uint8_t CRC_calc = calculate_crc8_coding(DATA_RAW, 6);
	if(CRC_calc == DATA_RAW[6]){
		return 1;
	}
	return 0;
}

//===================== test ===================================================


extern uint8_t LoRa_RxBuffer[7];



void test_encoding_decoding_data (void){
	counter_code++;

	char buff[24];

	sprintf(buff, "RAW %02X %02X %02X %02X %02X %02X %02X", LoRa_RxBuffer[0], LoRa_RxBuffer[1], LoRa_RxBuffer[2], LoRa_RxBuffer[3],
														  LoRa_RxBuffer[4], LoRa_RxBuffer[5], LoRa_RxBuffer[6]);
	ST7789_DrawString_10x16_background(0, 110, buff, WHITE, background_color);

	uint8_t key_init = decoding_key_init (LoRa_RxBuffer[5]);

	sprintf(buff, "key_init = %X", key_init);
	ST7789_DrawString_10x16_background(0, 130, buff, WHITE, background_color);

//	//--------------- encoded data and forming packet TX ----------------
//	uint8_t encoded_Buffer[7] = {0};
//	uint8_t KEY_NUM = encoding_key_init (counter_code);
//	encoding_decoding_data (counter_code, LoRa_RxBuffer, encoded_Buffer, 5);
//	encoded_Buffer[5] = KEY_NUM;
//	uint8_t CRC81 = calculate_crc8_coding(encoded_Buffer, 6);
//	encoded_Buffer[6] = CRC81;
//	//--------------------------------------------------------------------
//
//	sprintf(buff, "ENC %02X %02X %02X %02X %02X %02X %02X", encoded_Buffer[0], encoded_Buffer[1], encoded_Buffer[2], encoded_Buffer[3],
//															encoded_Buffer[4], encoded_Buffer[5], encoded_Buffer[6]);
//	ST7789_DrawString_10x16_background(0, 130, buff, YELLOW, background_color);
//
//
//	//--------------- decoded data and forming packet TX ----------------
//	uint8_t decoded_Buffer[5] = {0};
//	uint8_t key_init = decoding_key_init (encoded_Buffer[5]);
//	encoding_decoding_data (key_init, encoded_Buffer, decoded_Buffer, 5);
//	uint8_t CRC82 = calculate_crc8_coding(encoded_Buffer, 6);
//	//--------------------------------------------------------------------
//
//	uint16_t col = RED;
//
//	if (CRC81 == CRC82 && LoRa_RxBuffer[0]==decoded_Buffer[0] &&
//                          LoRa_RxBuffer[1]==decoded_Buffer[1] &&
//						  LoRa_RxBuffer[2]==decoded_Buffer[2] &&
//						  LoRa_RxBuffer[3]==decoded_Buffer[3] &&
//						  LoRa_RxBuffer[4]==decoded_Buffer[4]){
//		col = GREEN;
//	}
//
//
//	sprintf(buff, "DEC %02X %02X %02X %02X %02X %02X %02X", decoded_Buffer[0], decoded_Buffer[1], decoded_Buffer[2], decoded_Buffer[3],
//			decoded_Buffer[4], key_init, CRC82);
//	ST7789_DrawString_10x16_background(0, 150, buff, col, background_color);


}


//
//void test_encoding_decoding_data (void){
//	counter_code++;
//
//	char buff[24];
//
//	sprintf(buff, "RAW %02X %02X %02X %02X %02X    %02X", LoRa_RxBuffer[0], LoRa_RxBuffer[1], LoRa_RxBuffer[2], LoRa_RxBuffer[3], LoRa_RxBuffer[4], counter_code);
//	ST7789_DrawString_10x16_background(0, 110, buff, WHITE, background_color);
//
//	//--------------- encoded data and forming packet TX ----------------
//	uint8_t encoded_Buffer[7] = {0};
//	uint8_t KEY_NUM = encoding_key_init (counter_code);
//	encoding_decoding_data (counter_code, LoRa_RxBuffer, encoded_Buffer, 5);
//	encoded_Buffer[5] = KEY_NUM;
//	uint8_t CRC81 = calculate_crc8_coding(encoded_Buffer, 6);
//	encoded_Buffer[6] = CRC81;
//	//--------------------------------------------------------------------
//
//	sprintf(buff, "ENC %02X %02X %02X %02X %02X %02X %02X", encoded_Buffer[0], encoded_Buffer[1], encoded_Buffer[2], encoded_Buffer[3],
//															encoded_Buffer[4], encoded_Buffer[5], encoded_Buffer[6]);
//	ST7789_DrawString_10x16_background(0, 130, buff, YELLOW, background_color);
//
//
//	//--------------- decoded data and forming packet TX ----------------
//	uint8_t decoded_Buffer[5] = {0};
//	uint8_t key_init = decoding_key_init (encoded_Buffer[5]);
//	encoding_decoding_data (key_init, encoded_Buffer, decoded_Buffer, 5);
//	uint8_t CRC82 = calculate_crc8_coding(encoded_Buffer, 6);
//	//--------------------------------------------------------------------
//
//	uint16_t col = RED;
//
//	if (CRC81 == CRC82 && LoRa_RxBuffer[0]==decoded_Buffer[0] &&
//                          LoRa_RxBuffer[1]==decoded_Buffer[1] &&
//						  LoRa_RxBuffer[2]==decoded_Buffer[2] &&
//						  LoRa_RxBuffer[3]==decoded_Buffer[3] &&
//						  LoRa_RxBuffer[4]==decoded_Buffer[4]){
//		col = GREEN;
//	}
//
//
//	sprintf(buff, "DEC %02X %02X %02X %02X %02X %02X %02X", decoded_Buffer[0], decoded_Buffer[1], decoded_Buffer[2], decoded_Buffer[3],
//			decoded_Buffer[4], key_init, CRC82);
//	ST7789_DrawString_10x16_background(0, 150, buff, col, background_color);
//
//
//}














