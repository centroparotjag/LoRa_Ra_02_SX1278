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
	.low = 0x52	//0x42
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
	

    while (1) 
    {

 
		
		POW_3V3_en(1);
		SPI_MasterInit();
		RST_Ra02(0);
		_delay_ms(2); // Small delay for stabilization
		RST_Ra02(1);
		_delay_ms(10); // Small delay for stabilization
		LoRa_init();

		uint8_t send_data[16] = {0xAA, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
								 0x08, 0x09, 0x0A, 0xB, 0x0C, 0x0D, 0x0E, 0x0F};
		LED(1);
		LoRa_transmit(send_data, 128, 500);
		LED(0);

		SPI_MasterDeInit();
		POW_3V3_en(0);

		_delay_ms(10000); // Small delay for stabilization

		//SHT30_0V_en(0);
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

