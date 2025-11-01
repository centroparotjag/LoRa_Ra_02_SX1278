/*
 * DS18B20.c
 *
 *  Created on: Nov 1, 2025
 *      Author: centr
 */


#include "DS18B20.h"
#include "stm32f4xx_hal.h"
#include "main.h"
extern TIM_HandleTypeDef htim10;
//====================================== GPIO ============================================
uint8_t DQ_read (void){
	uint8_t DQ_STATE = HAL_GPIO_ReadPin(DQ_DS18B20_GPIO_Port, DQ_DS18B20_Pin);
	if(DQ_STATE == 0) {return 0;}
	else  {return 1;}
}

void DQ_OUT (uint8_t Out){
	if ( Out ){
		HAL_GPIO_WritePin(DQ_DS18B20_GPIO_Port, DQ_DS18B20_Pin, GPIO_PIN_SET);			// DQ = 1
	}
	else{
		HAL_GPIO_WritePin(DQ_DS18B20_GPIO_Port, DQ_DS18B20_Pin, GPIO_PIN_RESET);		// DQ = 0
	}
}

void Power_DS18B20 (unsigned char pow_ON){
	  if (pow_ON){
		  HAL_GPIO_WritePin(POW_DS18B20_GPIO_Port, POW_DS18B20_Pin, GPIO_PIN_SET);		// VDD = 1
	  }
	  else {
		  HAL_GPIO_WritePin(POW_DS18B20_GPIO_Port, POW_DS18B20_Pin, GPIO_PIN_RESET);	// VDD = 0
	  }
}

//========================================================================================


void us_pause(uint8_t us){
	htim10.Instance->CNT = 0;
    while (htim10.Instance->CNT < us-1);
}

//-------------- test gpio ------------
void test_gpio_DS18b20(void){
	Power_DS18B20 (1);
	HAL_Delay(5);

	DQ_OUT (0);
	us_pause(5);
	DQ_OUT (1);

	HAL_Delay(5);
	Power_DS18B20 (0);
}



//------------------------------------



//---------------------- PRESENCE -----------------------------
uint8_t DS18B20_RESET_PRESENCE (void)
{
	unsigned char STATUS_PRESENCE;
	HAL_Delay(1);			//_delay_ms(1);
	DQ_OUT (0);
	HAL_Delay(1);			//_delay_ms(1);
	DQ_OUT (1);
	us_pause(60);			//_delay_us(60);			// 60u
	STATUS_PRESENCE = DQ_read ();
	HAL_Delay(1);			//_delay_ms(1); _delay_ms(1);
	return STATUS_PRESENCE;
}

//----------------- MASTER WRITE SLOT ------------------------
void WRITE_SLOT ( unsigned char SLOT){

	DQ_OUT (0);

	if (SLOT)	//////////// 1 ///////////   --|_|---------
	{
		us_pause(15);			//_delay_us(2);			// 5u
		DQ_OUT (1);
		us_pause(62);			//_delay_us(62); 			// 62u
	}
	else		//////////// 0 ///////////	--|_________|--
	{
		us_pause(65);			//_delay_us(65); 			// 65u
		DQ_OUT (1);
		us_pause(5);			//_delay_us(5);			// 5u
	}
}

//---------- MASTER READ SLOT ---------------------------------------
uint8_t READ_SLOT ( void){
	unsigned char READ = 0;
	DQ_OUT (0);
	us_pause(5);			//_delay_us(5);				// 5u (78i = 10us)
	DQ_OUT (1);
	us_pause(5);			//_delay_us(5);				// 5u (78i = 10us)
	READ = DQ_read ();
	us_pause(70);			//_delay_us(70);				// 70u (78i = 10us)
	return READ;
}

//----------- MASTER READ BYTE (Little endian) ----------------------
uint8_t READ_BYTE (void){
	unsigned char BYTE = 0;
	for (unsigned char i = 0; i < 8; ++i)
	{
		BYTE |= ((READ_SLOT () ) << i) ;
	}
	return BYTE;
}

//----------- MASTER WRITE BYTE (Little endian) ----------------------
void WRITE_BYTE ( uint8_t DATA){
	unsigned char arr [9] = {0};

	for (uint8_t i = 0; i < 8; ++i)
	{
		arr [i] = (DATA>>i) & 0x01 ;
	}



	for (uint8_t i = 0; i < 8; ++i)
	{
			WRITE_SLOT ( arr [i] );
	}
}

//----------------------- CRC --------------------------------------------------
uint8_t dsCRC8(const uint8_t *addr, uint8_t len){			//CRC 1-WIRE DS18B20

  unsigned char crc = 0;
  while (len--)
  {
    unsigned char inbyte = *addr++;
    for (unsigned char i = 8; i; i--)
    {
      unsigned char mix = (crc ^ inbyte) & 0x01;
      crc >>= 1;
      if (mix) crc ^= 0x8C;
      inbyte >>= 1;
    }
  }
  return crc;
}

uint8_t SCRATCHPAD [9];
//---------------------- READ SCRATCHPAD --------------------------------------
uint8_t SCRATCHPAD_READ (void){

	uint8_t i, SCRATCHPAD_i [9];

		WRITE_BYTE ( SKIP_ROM);					// DQ, DATA
		WRITE_BYTE ( READ_SCRATCHPAD);			// DQ, DATA

		//-------- READ 9 BYTE SCRATCHPAD --------
		for (i = 0; i < 9; ++i) {
			SCRATCHPAD_i [i] = READ_BYTE (  );	// DQ
		}

		//---- comparison of the CRC --------
		if (SCRATCHPAD_i[8] == dsCRC8(SCRATCHPAD_i, 8))
		{
			for (i = 0; i < 9; ++i) {
				SCRATCHPAD [i] = SCRATCHPAD_i [i];	// Copy SCRATCHPAD to in external buffer
			}

			unsigned int T_src=0;
			T_src = SCRATCHPAD [0];
			T_src |= SCRATCHPAD [1]<<8;

			return T_src;								// CRC - OK (0x01)
		}
		else
		{
			for (i = 0; i < 9; ++i) {
				SCRATCHPAD [i] = 0;					// RESET DATA in external SCRATCHPAD buffer
			}
			return 0;							// CRC - ERROR (0x00)
		}
}

//----------------------- TEST AND WRITE DEFAULT STATE SRAM -------------
uint8_t TEST_AND_WRITE_DEFAULT_SRAM (void){
	uint8_t i, CONFIG_RESOLUTION;
	CONFIG_RESOLUTION = RESOLUTION_12b;			// SET DEFAULT RESOLUTION
	if (DS18B20_RESET_PRESENCE ()) {
		return ERROR;							// error
	}
	else {}

	//--------------- READ SCRATCHPAD ----------------------------
	i = 0;
	while ( SCRATCHPAD_READ (  ) == 0 && i < 10) { i++; }

	//-------------- check resolution in SRAM --------------------
	if ( SCRATCHPAD [4] == CONFIG_RESOLUTION ){	// Config Register
		return OK;
	}
	else {}

	//================= NEW SEQUENCE WRITE SCRATCHPAD ===================
	DS18B20_RESET_PRESENCE ();

	WRITE_BYTE (SKIP_ROM);					// DQ, DATA
	WRITE_BYTE (WRITE_SCRATCHPAD);			// DQ, DATA

	WRITE_BYTE (0x4B);						// DQ, Th (default manufacture). SRAM 0x02
	WRITE_BYTE (0x46);						// DQ, Tl (default manufacture). SRAM 0x03
	WRITE_BYTE (CONFIG_RESOLUTION);			// DQ, config. 				     SRAM 0x04

	DS18B20_RESET_PRESENCE ();

	if ( !SCRATCHPAD_READ () ){				//READ SCRATCHPAD and CRC CHECK
		return 255;
	}
	else {}

	//-------------- check resolution in SRAM --------------------
	if ( SCRATCHPAD [4] != CONFIG_RESOLUTION ){	// Config. Register
		return 254;
	}
	else {}

	//================== SAVE ROM in SRAM ==============================
	DS18B20_RESET_PRESENCE ();
	WRITE_BYTE (SKIP_ROM);					// DQ, DATA
	WRITE_BYTE (COPY_SCRATCHPAD);			// DQ, DATA

	HAL_Delay(15);			//_delay_ms(15);

	return OK;							   // OK write SRAM
}

//*********************** READ ROM 64b *********************************************

//unsigned char ROM [9] = {};
//
//unsigned char READ_ROM_64 (void){
	//unsigned char i, ROM_i [9];
//
	//if (DS18B20_RESET_PRESENCE ()) {
		//return ERROR;						// error
	//}
	//else {}
//
	//WRITE_BYTE (READ_ROM);					// DQ, DATA
//
	//-------- READ 9 BYTE ROM --------
	//for (i = 0; i < 8; ++i) {
		//ROM_i [i] = READ_BYTE (  );			// DQ
	//}
//
	//---- comparison of the CRC --------
	//if (ROM_i[7] == dsCRC8(ROM_i, 7))
	//{
		//for (i = 0; i < 8; ++i) {
			//ROM [i] = ROM_i [i];			// Copy ROM to in external buffer
		//}
		//return OK;							// CRC - OK (0x01)
	//}
	//else
	//{
		//for (i = 0; i < 9; ++i) {
			//ROM [i] = 0;					// RESET DATA in external SCRATCHPAD buffer
		//}
		//return ERROR;						// CRC - ERROR (0x00)
	//}
//}

//*********************** READ TEMPERATURE ******************************************

uint8_t FLAG_convert = 0;
uint16_t internal_T = 0;

void time_out_convert_t (uint8_t  timer, uint8_t  conversial_period){
	extern  uint16_t T_src;

	if (  !(timer % conversial_period) && FLAG_convert==0)
	{
		Power_DS18B20(1);
		_delay_ms(5);
		TEST_AND_WRITE_DEFAULT_SRAM ();
		_delay_ms(1);
		start_t_convert_DS12B20 ();
		internal_T = timer+1;
		FLAG_convert=1;
	}

	if ( timer==internal_T)
	{
	    T_src = read_t_after_convert_DS12B20 ();
		Power_DS18B20(0);
		FLAG_convert = 0;
		internal_T = 0;
	}
}


uint8_t start_t_convert_DS12B20 (void){
		//--------- CONVERT T -----------------------------------
		if (DS18B20_RESET_PRESENCE ()) {
			return 0;					// error
		}
		else {}

		WRITE_BYTE (SKIP_ROM);				// DQ, DATA  0xCC
		WRITE_BYTE (CONVERT_T);				// DQ, DATA 0x44
		return 1;
}

uint8_t read_t_after_convert_DS12B20 (void){
	//---------- READ T ------------------------------------
	if (DS18B20_RESET_PRESENCE ()) {
		return 0;							// error
	}
	else {}

	return SCRATCHPAD_READ ();
}



uint8_t READ_TEMPERATURE (void){

	//--------- CONVERT T -----------------------------------
	if (DS18B20_RESET_PRESENCE ()) {
		return ERROR;							// error
	}
	else {}

	WRITE_BYTE (SKIP_ROM);					// DQ, DATA
	WRITE_BYTE (CONVERT_T);				// DQ, DATA
	_delay_ms(751);

	//---------- READ T ------------------------------------
	if (DS18B20_RESET_PRESENCE ()) {
		return ERROR;							// error
	}
	else {}

	return SCRATCHPAD_READ ();
}

//-------------------------------------------------------------------
uint8_t START__DS18B20_CONVERT_TEMPERATURE (void){
	//--------- CONVERT T -----------------------------------
	if (DS18B20_RESET_PRESENCE ()) {
		return ERROR;							// error
	}
	else {}

	WRITE_BYTE (SKIP_ROM);					// DQ, DATA
	WRITE_BYTE (CONVERT_T);				// DQ, DATA
}


uint16_t READ_DS18B20_TEMPERATURE (void){
	uint16_t T_RAW = 0;
	//---------- READ T ------------------------------------
	if (DS18B20_RESET_PRESENCE ()) {
		return ERROR;							// error
	}
	else {}
	SCRATCHPAD_READ ();
	T_RAW |= SCRATCHPAD [1]<<8 | SCRATCHPAD [0];
	return T_RAW;
}


