/*
 * display.h
 *
 *  Created on: Oct 12, 2025
 *      Author: centr
 */

#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_

#include "stm32f4xx_hal.h"

void power_on_displayed (void);
void shutdown_displayed (void);
uint8_t displaying_images_from_flash (void);
void battery_level_10_18 (uint16_t x0, uint16_t y0, float V);

#endif /* INC_DISPLAY_H_ */
