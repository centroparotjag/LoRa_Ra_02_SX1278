/*
 * data_encoding.h
 *
 * Created: 30.10.2025 17:35:13
 *  Author: centr
 */ 


#ifndef DATA_ENCODING_H_
#define DATA_ENCODING_H_

uint8_t calculate_crc8_coding(const uint8_t *data, uint8_t length);
void encoding_decoding_data (uint8_t key_init, uint8_t* data_in, uint8_t* data_out, uint8_t len);
uint8_t encoding_key_init (uint8_t key_cnt);
uint8_t decoding_key_init (uint8_t key_init_encoded);
void DataEncoding_FormationTransmittedPackets (uint8_t* DATA_RAW, uint8_t* EncodedDataPacket);


#endif /* DATA_ENCODING_H_ */