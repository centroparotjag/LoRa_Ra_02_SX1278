/*
 * font.h
 *
 *  Created on: Oct 11, 2025
 *      Author: centr
 */

#ifndef INC_FONT_H_
#define INC_FONT_H_

#include "stm32f4xx_hal.h"
  // Characters definition
  // -----------------------------------
  // number of columns for chars
  #define CHARS_COLS_LENGTH  5
  // @const Characters
  extern const uint8_t FONT_5x8 [][CHARS_COLS_LENGTH];
  extern const uint8_t FONT_terminal_10x15[96][20];

#endif /* INC_FONT_H_ */
