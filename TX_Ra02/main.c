/*
 * TX_Ra02.c
 *
 * Created: 26.10.2025 9:00:56
 * Author : centr
 */ 

#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

#include "SHT30.h"
#include "init.h"
#include "i2c.h"
#include "SPI.h"
#include "UART.h"
#include "Ra02_LoRa.h"
#include "ADC.h"

#define DISABLE 0
#define ENABLE  1

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
	.low = 0x52 //0x52	//0xD2 - 8MHz
};
LOCKBITS = (0xFF);	// no_lock
//******************************** !!! ********************************************

uint32_t volatile sleep_timer = 60;

int main(void)
{	
	WDTCSR=0;
	GPIO_init();		// Initialize GPIO
	timer1_init ();
	sleep_init ();
	
	//==================== DEBUG ======================================
	uint8_t sd[64] = { 0,1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,
					  16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
					  32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
					  48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63};
	//=================================================================


    while (1) 
    {
		uint8_t T_h [4] = {0};

		if (sleep_timer > 60){				// period in seconds
			sleep_idle_startup (DISABLE);
			//---------- measurement ADC, TH30 (T, h) -----------
			activation_of_electrical_circuits (1);
			measurement_t_h_SHT30 (T_h);
			
			LED(1);
			uint8_t Vbat =  Vbat_adc_read();
			LED(0);
			
			activation_of_electrical_circuits (0);
				
			//-------- forming data ---------------		
			sd[0] =	T_h[0];			// T-msB
			sd[1] =	T_h[1];			// T-lsB
			sd[2] =	T_h[2];			// h-msB
			sd[3] =	T_h[3];			// h-lsB
			sd[4] =	Vbat;			// V_battery = (Vbat+250)/100;

			//---------- LoRa transmit data ----------
			 LoRa_transmit_main_data (sd, 5);
			//----------------------------------------
			
			sleep_timer = 0;			// reset timer (1s)
		}
		

			sleep_idle_startup (ENABLE);		//  sleep here

	}
}




// TIMER1 interrupt service routine
ISR(TIMER1_COMPA_vect)
{
	sleep_timer++;
}

