/*
 * data_encoding.h
 *
 *  Created on: Oct 30, 2025
 *      Author: centr
 */

#ifndef DATA_ENCODING_H_
#define DATA_ENCODING_H_

#include "stm32f4xx_hal.h"

uint8_t calculate_crc8_coding(const uint8_t *data, size_t length);
void encoding_decoding_data (uint8_t key_init, uint8_t* data_in, uint8_t* data_out, uint8_t len);
uint8_t encoding_key_init (uint8_t key_cnt);
uint8_t decoding_key_init (uint8_t key_init_encoded);
void test_encoding_decoding_data (void);


#endif /* DATA_ENCODING_H_ */
