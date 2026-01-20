/*
 * Flash_W25Q64.h
 *
 *  Created on: Oct 18, 2025
 *      Author: centr
 */

#ifndef INC_FLASH_W25Q64_H_
#define INC_FLASH_W25Q64_H_

uint8_t flash_W25Q64_pressence (void);
void read_data_flash_W25Q64 (uint32_t addr, uint8_t* data, uint8_t Size);
uint32_t signature_and_data_search (void);

#endif /* INC_FLASH_W25Q64_H_ */
