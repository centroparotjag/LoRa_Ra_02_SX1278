/*
 * DS18B20.h
 *
 *  Created on: Nov 1, 2025
 *      Author: centr
 */

#ifndef INC_DS18B20_H_
#define INC_DS18B20_H_

#include "stm32f4xx_hal.h"

#define SEARCH_ROM			0xF0
#define READ_ROM			0x33
#define MATCH_ROM			0x55
#define SKIP_ROM			0xCC
#define ALARM_SEARCH		0xEC

#define CONVERT_T			0x44
#define WRITE_SCRATCHPAD	0x4E
#define READ_SCRATCHPAD		0xBE
#define COPY_SCRATCHPAD		0x48
#define RECALL_E2			0xB8
#define READ_POWER_SUPPLY	0xB4

#define PROCESS				0x03
#define OK					0x01
#define ERROR				0x00

#define RESOLUTION_9b		0x1F
#define RESOLUTION_10b		0x3F
#define RESOLUTION_11b		0x5f
#define RESOLUTION_12b		0x7F

void DQ_O_I_mode (uint8_t OI);
uint8_t DQ_read (void);
void DQ_OUT (uint8_t Out);
void Power_DS18B20 (uint8_t pow_ON);
uint8_t polling_DS18B20 (void);
uint8_t DS18B20_RESET_PRESENCE (void);
void WRITE_SLOT ( uint8_t SLOT);
uint8_t READ_SLOT (void);
uint8_t READ_BYTE (void);
void WRITE_BYTE ( uint8_t DATA);
uint8_t dsCRC8(const uint8_t *addr, uint8_t len);
uint8_t SCRATCHPAD_READ (void);
uint8_t TEST_AND_WRITE_DEFAULT_SRAM (void);
uint8_t READ_TEMPERATURE (void);
uint8_t READ_ROM_64 (void);
uint8_t read_t_after_convert_DS12B20 (void);
uint8_t start_t_convert_DS12B20 (void);
void time_out_convert_t (uint8_t  timer, uint8_t  conversial_period);
uint8_t START__DS18B20_CONVERT_TEMPERATURE (void);
uint16_t READ_DS18B20_TEMPERATURE (void);

#endif /* INC_DS18B20_H_ */
