/*
 * TX_Ra02.c
 *
 * Created: 26.10.2025 9:00:56
 * Author : centr
 */ 

#include <avr/io.h>
#include "SHT30.h"
#include "init.h"
#include "i2c.h"
#include "SPI.h"
#include "UART.h"
#include "Ra02_LoRa.h"


//************************** FUSES !!! & LOCK !!! *********************************
// EXTENDET.BODLEVEL = brown-out detection at VCC=1.8V
// HIGH.SPEN
// HIGH.EESAVE
// HIGH.BOOTSZ = Boot Flash size = 1024 words start address=$3C00
// HIGH.BOOTRST
// LOW.CKDIV8
// LOW.SUT_CKSEL = Int. RC Osc. 8 MHz; Start-up time PWRDWN/RESET:6 CK/14 CK + 0 ms
FUSES = {
	.extended = 0xFE,	//0xFE,
	.high = 0xD2,
	.low = 0xD2 //0x52	//0x42
};
LOCKBITS = (0xFF);	// no_lock
//******************************** !!! ********************************************


int main(void)
{
	GPIO_init();		// Initialize GPIO
	i2c_init();			// Initialize I2C
	SPI_MasterDeInit();
	//SPI_MasterInit();	// Initialize SPI
	uart_init();		// Initialize UART
	_delay_ms(100);		// Small delay for stabilization    
	
	SHT30_0V_en(0);
	_delay_ms(5);		// Small delay for stabilization
	SHT30_heater (1);
	



		//_delay_ms(500); // Small delay for stabilization
		//LoRa_startReceiving();

	uint8_t counter = 0;
	uint8_t sd[64] = { 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,
					  17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,
					  33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,
					  49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64};



    while (1) 
    {

		//---------------------------------------
		POW_3V3_en(1);
		_delay_ms(2);
		SPI_MasterInit();
		LoRa_reset();
		LoRa_init();
									LED(1);
		LoRa_transmit(sd, 64, 1000);
									LED(0);
		SHT30_0V_en(0);
		//----------------------------------------




		_delay_ms(10000); // Small delay for stabilization

		for(uint8_t i=0; i<64; i++){
			sd[i]=sd[i]+1;
		}

		
		//_delay_ms(2);		// Small delay for stabilization
		//uint8_t Th[4];
		//uint8_t check_crc = mesurement_t_h_SHT30 (Th);
		//SHT30_0V_en(1);
				
		
		//char buff[20] = "Hello word";
		//sprintf(buff, "t=%02X%02X h=%02X%02X crc=%d", Th[0], Th[1], Th[2], Th[3], check_crc);
		//
		//for(uint8_t i=0; i<19; ++i){
			//uart_transmit(buff[i]);
		//}
		//uart_transmit('\r');

    }
}

