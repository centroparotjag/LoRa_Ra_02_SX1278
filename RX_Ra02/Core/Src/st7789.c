/*
 * st7789.c
 *
 *  Created on: Oct 10, 2025
 *      Author: centr
 */


#include "stm32f4xx_hal.h"
#include <stdlib.h>
#include "main.h"
#include <st7789.h>
#include "font.h"

uint8_t ST7789_Width, ST7789_Height;

void ST7789_Init(uint8_t Width, uint8_t Height)
{
  ST7789_Width = Width;
  ST7789_Height = Height;
  HAL_Delay(15);
  ST7789_HardReset();
  ST7789_SoftReset();
  ST7789_SleepModeExit();

  ST7789_ColorModeSet(ST7789_ColorMode_65K | ST7789_ColorMode_16bit);
  HAL_Delay(10);
  ST7789_MemAccessModeSet(4, 1, 1, 0);
  HAL_Delay(10);
  ST7789_InversionMode(1);
  HAL_Delay(10);
  ST7789_FillScreen(0);
  ST7789_SetBL(100);
  ST7789_DisplayPower(1);
  HAL_Delay(150);
}

void ST7789_HardReset(void)
{
	HAL_GPIO_WritePin(res_IPS_GPIO_Port, res_IPS_Pin, GPIO_PIN_RESET);
	HAL_Delay(15);
  HAL_GPIO_WritePin(res_IPS_GPIO_Port, res_IPS_Pin, GPIO_PIN_SET);
  HAL_Delay(150);
}

void ST7789_SoftReset(void)
{
  ST7789_SendCmd(ST7789_Cmd_SWRESET);
  HAL_Delay(130);
}

void ST7789_SendCmd(uint8_t Cmd)
{
  HAL_GPIO_WritePin(dc_IPS_GPIO_Port, dc_IPS_Pin, GPIO_PIN_RESET);
  //HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
	if (READ_BIT(SPI1->CR1, SPI_CR1_SPE) != (SPI_CR1_SPE)) SET_BIT(SPI1->CR1, SPI_CR1_SPE);
	*((__IO uint8_t *)&SPI1->DR) = Cmd;
  //HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);

}

void ST7789_SendData(uint8_t Data)
{
	HAL_GPIO_WritePin(dc_IPS_GPIO_Port, dc_IPS_Pin, GPIO_PIN_SET);
  //HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
  if (READ_BIT(SPI1->CR1, SPI_CR1_SPE) != (SPI_CR1_SPE)) SET_BIT(SPI1->CR1, SPI_CR1_SPE);
	*((__IO uint8_t *)&SPI1->DR) = Data;
  //HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
}

void ST7789_SleepModeEnter( void )
{
	ST7789_SendCmd(ST7789_Cmd_SLPIN);
  HAL_Delay(500);
}

void ST7789_SleepModeExit( void )
{
	ST7789_SendCmd(ST7789_Cmd_SLPOUT);
  HAL_Delay(130);
}


void ST7789_ColorModeSet(uint8_t ColorMode)
{
  ST7789_SendCmd(ST7789_Cmd_COLMOD);
  ST7789_SendData(ColorMode & 0x77);
}

void ST7789_MemAccessModeSet(uint8_t Rotation, uint8_t VertMirror, uint8_t HorizMirror, uint8_t IsBGR)
{
  uint8_t Value;
  Rotation &= 7;

  ST7789_SendCmd(ST7789_Cmd_MADCTL);

  switch (Rotation)
  {
  case 0:
    Value = 0;
    break;
  case 1:
    Value = ST7789_MADCTL_MX;
    break;
  case 2:
    Value = ST7789_MADCTL_MY;
    break;
  case 3:
    Value = ST7789_MADCTL_MX | ST7789_MADCTL_MY;
    break;
  case 4:
    Value = ST7789_MADCTL_MV;
    break;
  case 5:
    Value = ST7789_MADCTL_MV | ST7789_MADCTL_MX;
    break;
  case 6:
    Value = ST7789_MADCTL_MV | ST7789_MADCTL_MY;
    break;
  case 7:
    Value = ST7789_MADCTL_MV | ST7789_MADCTL_MX | ST7789_MADCTL_MY;
    break;
  }

  if (VertMirror)
    Value = ST7789_MADCTL_ML;
  if (HorizMirror)
    Value = ST7789_MADCTL_MH;

  if (IsBGR)
    Value |= ST7789_MADCTL_BGR;

  ST7789_SendData(Value);
}

void ST7789_InversionMode(uint8_t Mode)
{
  if (Mode)
    ST7789_SendCmd(ST7789_Cmd_INVON);
  else
    ST7789_SendCmd(ST7789_Cmd_INVOFF);
}

void ST7789_FillScreen(uint16_t color)
{
  ST7789_FillRect(0, 0,  ST7789_Width, ST7789_Height, color);
}

void ST7789_FillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  if ((x >= ST7789_Width) || (y >= ST7789_Height)) return;
  if ((x + w) > ST7789_Width) w = ST7789_Width - x;
  if ((y + h) > ST7789_Height) h = ST7789_Height - y;
  ST7789_SetWindow(x, y, x + w - 1, y + h - 1);
  for (uint32_t i = 0; i < (h * w); i++) ST7789_RamWrite(&color, 1);
}

void ST7789_SetWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
  ST7789_ColumnSet(x0, x1);
  ST7789_RowSet(y0, y1);
  ST7789_SendCmd(ST7789_Cmd_RAMWR);
}

void ST7789_RamWrite(uint16_t *pBuff, uint16_t Len)
{
  while (Len--)
  {
    ST7789_SendData(*pBuff >> 8);
    ST7789_SendData(*pBuff & 0xFF);
  }
}

static void ST7789_ColumnSet(uint16_t ColumnStart, uint16_t ColumnEnd)
{
  if (ColumnStart > ColumnEnd)
    return;
  if (ColumnEnd > ST7789_Width)
    return;

  ColumnStart += ST7789_X_Start;
  ColumnEnd += ST7789_X_Start;

  ST7789_SendCmd(ST7789_Cmd_CASET);
  ST7789_SendData(ColumnStart >> 8);
  ST7789_SendData(ColumnStart & 0xFF);
  ST7789_SendData(ColumnEnd >> 8);
  ST7789_SendData(ColumnEnd & 0xFF);
}

static void ST7789_RowSet(uint16_t RowStart, uint16_t RowEnd)
{
  if (RowStart > RowEnd)
    return;
  if (RowEnd > ST7789_Height)
    return;

  RowStart += ST7789_Y_Start;
  RowEnd += ST7789_Y_Start;

  ST7789_SendCmd(ST7789_Cmd_RASET);
  ST7789_SendData(RowStart >> 8);
  ST7789_SendData(RowStart & 0xFF);
  ST7789_SendData(RowEnd >> 8);
  ST7789_SendData(RowEnd & 0xFF);
}

extern TIM_HandleTypeDef htim4;


void ST7789_SetBL(uint8_t Value)
{
  if (Value > 100){
	Value = 100;
  }

  uint16_t compare = Value * 40;

  if (Value = 0){
	  compare = 10;
  }

  __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, compare);
}

void ST7789_DisplayPower(uint8_t On)
{
  if (On)
    ST7789_SendCmd(ST7789_Cmd_DISPON);
  else
    ST7789_SendCmd(ST7789_Cmd_DISPOFF);
}

void ST7789_DrawRectangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
  ST7789_DrawLine(x1, y1, x1, y2, color);
  ST7789_DrawLine(x2, y1, x2, y2, color);
  ST7789_DrawLine(x1, y1, x2, y1, color);
  ST7789_DrawLine(x1, y2, x2, y2, color);
}

void ST7789_DrawRectangleFilled(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t fillcolor)
{
  if (x1 > x2)
    SwapInt16Values(&x1, &x2);
  if (y1 > y2)
    SwapInt16Values(&y1, &y2);
	ST7789_FillRect(x1, y1, x2 - x1, y2 - y1, fillcolor);
}

void ST7789_DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
  // Вертикальная линия
  if (x1 == x2)
  {
    // Отрисовываем линию быстрым методом
    if (y1 > y2)
      ST7789_FillRect(x1, y2, 1, y1 - y2 + 1, color);
    else
      ST7789_FillRect(x1, y1, 1, y2 - y1 + 1, color);
    return;
  }

  // Горизонтальная линия
  if (y1 == y2)
  {
    // Отрисовываем линию быстрым методом
    if (x1 > x2)
      ST7789_FillRect(x2, y1, x1 - x2 + 1, 1, color);
    else
      ST7789_FillRect(x1, y1, x2 - x1 + 1, 1, color);
    return;
  }

  ST7789_DrawLine_Slow(x1, y1, x2, y2, color);
}

static void SwapInt16Values(int16_t *pValue1, int16_t *pValue2)
{
  int16_t TempValue = *pValue1;
  *pValue1 = *pValue2;
  *pValue2 = TempValue;
}

static void ST7789_DrawLine_Slow(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
  const int16_t deltaX = abs(x2 - x1);
  const int16_t deltaY = abs(y2 - y1);
  const int16_t signX = x1 < x2 ? 1 : -1;
  const int16_t signY = y1 < y2 ? 1 : -1;

  int16_t error = deltaX - deltaY;

  ST7789_DrawPixel(x2, y2, color);

  while (x1 != x2 || y1 != y2)
  {
    ST7789_DrawPixel(x1, y1, color);
    const int16_t error2 = error * 2;

    if (error2 > -deltaY)
    {
      error -= deltaY;
      x1 += signX;
    }
    if (error2 < deltaX)
    {
      error += deltaX;
      y1 += signY;
    }
  }
}

void ST7789_DrawPixel(int16_t x, int16_t y, uint16_t color)
{
  if ((x < 0) ||(x >= ST7789_Width) || (y < 0) || (y >= ST7789_Height))
    return;

  ST7789_SetWindow(x, y, x, y);
  ST7789_RamWrite(&color, 1);
}

void ST7789_DrawCircleFilled(int16_t x0, int16_t y0, int16_t radius, uint16_t fillcolor)
{
  int x = 0;
  int y = radius;
  int delta = 1 - 2 * radius;
  int error = 0;

  while (y >= 0)
  {
    ST7789_DrawLine(x0 + x, y0 - y, x0 + x, y0 + y, fillcolor);
    ST7789_DrawLine(x0 - x, y0 - y, x0 - x, y0 + y, fillcolor);
    error = 2 * (delta + y) - 1;

    if (delta < 0 && error <= 0)
    {
      ++x;
      delta += 2 * x + 1;
      continue;
    }

    error = 2 * (delta - x) - 1;

    if (delta > 0 && error > 0)
    {
      --y;
      delta += 1 - 2 * y;
      continue;
    }

    ++x;
    delta += 2 * (x - y);
    --y;
  }
}

void ST7789_DrawCircle(int16_t x0, int16_t y0, int16_t radius, uint16_t color)
{
  int x = 0;
  int y = radius;
  int delta = 1 - 2 * radius;
  int error = 0;

  while (y >= 0)
  {
    ST7789_DrawPixel(x0 + x, y0 + y, color);
    ST7789_DrawPixel(x0 + x, y0 - y, color);
    ST7789_DrawPixel(x0 - x, y0 + y, color);
    ST7789_DrawPixel(x0 - x, y0 - y, color);
    error = 2 * (delta + y) - 1;

    if (delta < 0 && error <= 0)
    {
      ++x;
      delta += 2 * x + 1;
      continue;
    }

    error = 2 * (delta - x) - 1;

    if (delta > 0 && error > 0)
    {
      --y;
      delta += 1 - 2 * y;
      continue;
    }

    ++x;
    delta += 2 * (x - y);
    --y;
  }
}

//-------------------------------------------------------------------------------------------------------

//void test_display_demo (void){
//	//    ST7789_Init(240, 240);
//	    	  // Инициализация входа для кнопки
//	        // Тест вывода основных цветов
//	        //ST7789_FillScreen(BLACK);
//	        ST7789_SetBL(100);
//
//	        uint16_t color = RGB565(255, 0, 0);
//	        ST7789_FillScreen(color);
//	        osDelay(200); //HAL_Delay(500);
//
//	        color = RGB565(0, 255, 0);
//	        ST7789_FillScreen(color);
//	        osDelay(200);
//
//	        color = RGB565(50, 55, 50);
//	        ST7789_FillScreen(color);
//	        osDelay(200);
//
//	        color = RGB565(0, 0, 255);
//	        ST7789_FillScreen(color);
//	        osDelay(200);
//
//	        color = RGB565(255, 255, 0);
//	        ST7789_FillScreen(color);
//	        osDelay(200);
//
//	        color = RGB565(255, 0, 255);
//	        ST7789_FillScreen(color);
//	        osDelay(200);
//
//	        color = RGB565(0, 255, 255);
//	        ST7789_FillScreen(color);
//	        osDelay(200);
//
//	        color = RGB565(255, 255, 255);
//	        ST7789_FillScreen(color);
//	        osDelay(200);
//
//	        ST7789_FillScreen(BLACK);
//	        ST7789_SetBL(100);
//
//	    		for (uint8_t y = 0; y<240 ; y++) {
//	    			ST7789_DrawLine(120, 120, 239, y, RGB565(y+10, 0, 0));
//	    		}
//
//	    		for (uint8_t x = 0; x<240 ; x++) {
//	    			ST7789_DrawLine(120, 120, x, 239, RGB565(0, x+10, 0));
//	    		}
//
//	    		for (uint8_t y = 0; y<240 ; y++) {
//	    			ST7789_DrawLine(120, 120, 0, y, RGB565(0, 0, y+10));
//	    		}
//
//	    		for (uint8_t x = 0; x<240 ; x++) {
//	    			ST7789_DrawLine(120, 120, x, 0, RGB565(x+10, x+10, x+10));
//	    		}
//	    	osDelay(300);
//
//	        ST7789_FillScreen(BLACK);
//	        ST7789_SetBL(100);
//
//	        for (uint8_t x = 0; x < 240 ; x = x + 20) {
//	    			for (uint8_t y = 0; y < 240; y = y + 20) {
//	    				ST7789_DrawRectangleFilled(x + 3, y + 3, x + 17, y + 17, RGB565(x, y, 0));
//	    				ST7789_DrawRectangle(x + 2, y + 2, x + 19, y + 19, RGB565(250, 250, 250));
//	    			}
//	    		}
//	        osDelay(300);
//
//	        ST7789_FillScreen(BLACK);
//	        ST7789_SetBL(100);
//
//	        for (uint8_t x = 0; x < 240 ; x = x + 20) {
//	    			for (uint8_t y = 0; y < 240; y = y + 20) {
//	            ST7789_DrawCircleFilled(x + 10, y + 10, 8, RGB565(x, y, 0));
//	            ST7789_DrawCircle(x + 10, y + 10, 9, RGB565(0, y, x));
//	    			}
//	    		}
//	        osDelay(300);
//
//	        ST7789_FillScreen(BLACK);
//	        ST7789_SetBL(100);
//}
//
//void disp_test (void){
//	uint16_t color = RGB565(0, 255, 255);
//	ST7789_DrawCircle(80, 80, 30, color);
//
//	color = RGB565(255, 255, 255);
//	ST7789_DrawLine(80, 80, 150, 200, color);
//
//	color = RGB565(055, 120, 255);
//	ST7789_DrawRectangleFilled(20, 20, 45, 45, color);
//
//	color = RGB565(200, 120, 255);
//	ST7789_DrawCircleFilled(40, 150, 20, color);
//
//	color = RGB565(0, 0, 0);
//	ST7789_DrawChar_5x8 (40, 150, '#', color);
//
//	color = RGB565(255, 255, 255);
//	//char str[] = {"Test text 1 2 3 4 5 6 7 8 9 10 11 12"};
//	ST7789_DrawString_5x8 (0, 210, "Test text 5x8 pix",  color);
//
//	color = RGB565(255, 10, 10);
//	ST7789_DrawChar_10x16 (180, 10, '3', color);
//	ST7789_DrawString_10x16 (0, 220, "Test text 10x16 pix 0123456789", color);
//}


//=========================== text 5x8 ===================================================

void ST7789_DrawChar_5x8 (int16_t x0, int16_t y0, char character, uint16_t color)
{
	int pix = 0x01;
	int sym = character - 0x20;

	for (int16_t X= 0; X < 5; ++X) {
		pix = 0x01;
		for (int16_t Y= 0; Y < 8; ++Y) {
			if ( FONT_5x8[sym][X] & pix ) {
				ST7789_DrawPixel(X + x0, Y + y0, color);
			}
			pix = pix << 1;
		}
	}
}


void ST7789_DrawString_5x8 (int16_t x0, int16_t y0, char * str, uint16_t color)
{
  uint16_t i = 0;
  while (str[i] != '\0' && i<=34) {
	  ST7789_DrawChar_5x8 ( x0+(7*i), y0, str[i], color);
	  i++;
  }
}

//=========================== text 10x16 ===================================================

void ST7789_DrawChar_10x16 (int16_t x0, int16_t y0, char character, uint16_t color)
{
	int pix = 0x01;
	int sym = character - 0x20;
	for (int16_t X= 0; X < 10; ++X) {
		pix = 0x01;
		for (int16_t Y= 0; Y < 8; ++Y) {
			if ( FONT_terminal_10x15[sym][X*2] & pix ) {
				ST7789_DrawPixel(X + x0, Y + y0, color);
			}
			pix = pix << 1;
		}
		pix = 0x01;
		for (int16_t Y= 0; Y < 8; ++Y) {
			if ( FONT_terminal_10x15[sym][X*2+1] & pix ) {
				ST7789_DrawPixel(X + x0, Y + 8 + y0, color);
			}
			pix = pix << 1;
		}
	}
}

void ST7789_DrawString_10x16 (int16_t x0, int16_t y0, char * str, uint16_t color)
{
  uint16_t i = 0;
  while (str[i] != '\0' && i<=24) {
	  ST7789_DrawChar_10x16  ( x0+(10*i), y0, str[i], color);
	  i++;
  }
}

//-----
void ST7789_DrawChar_10x16_background (int16_t x0, int16_t y0, char character, uint16_t color, uint16_t background_color)
{
	int pix = 0x01;
	int sym = character - 0x20;
	for (int16_t X= 0; X < 10; ++X) {
		pix = 0x01;
		for (int16_t Y= 0; Y < 8; ++Y) {
			if ( FONT_terminal_10x15[sym][X*2] & pix ) {
				ST7789_DrawPixel(X + x0, Y + y0, color);
			}
			else {
				ST7789_DrawPixel(X + x0, Y + y0, background_color);
			}
			pix = pix << 1;
		}
		pix = 0x01;
		for (int16_t Y= 0; Y < 8; ++Y) {
			if ( FONT_terminal_10x15[sym][X*2+1] & pix ) {
				ST7789_DrawPixel(X + x0, Y + 8 + y0, color);
			}
			else {
				ST7789_DrawPixel(X + x0, Y + 8 + y0, background_color);
			}
			pix = pix << 1;
		}
	}
}

void ST7789_DrawString_10x16_background (int16_t x0, int16_t y0, char * str, uint16_t color, uint16_t background_color)
{
  uint16_t i = 0;
  while (str[i] != '\0' && i<=24) {
	  ST7789_DrawChar_10x16_background  ( x0+(10*i), y0, str[i], color, background_color);
	  i++;
  }
}

