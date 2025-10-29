/*
 * ADC.c
 *
 * Created: 29.10.2025 9:43:00
 *  Author: centr
 */ 


#include <avr/io.h>
#include "ADC.h"
#include "init.h"
#include "UART.h"
#include <stdio.h>

// Function to initialize the ADC
void init_adc(void) {
	ADC_0V_en(0);			// adc 0V circuit connected
	// Set ADC reference 
	ADMUX = (1 << REFS1) | (1 << REFS0);	//Internal 1.1V voltage reference with external capacitor at AREF pin
	// Enable ADC and set prescaler to 128 (for a 1MHz clock, this gives 7.812kHz ADC clock)
	// ADPS2, ADPS1, ADPS0 = 111
	ADCSRA = (1 << ADEN) | (1 << ADPS1) | (1 << ADPS0);

}

void deinit_adc(void){
	ADC_0V_en(1);		// adc 0V circuit disconnected
	ADMUX = 0;			// internal VREF turned off
	ADCSRA = 0;			// ADEN = 0;
}


uint8_t  Vbat_adc_read(void){
	ADMUX = 0;
	ADMUX = (1 << REFS0);					//AVCC with external capacitor at AREF pin, ADC0
	// Wait for the ADC to settle
	_delay_ms(2);
	// Start a single conversion
	ADCSRA |= (1 << ADSC);
	// Wait for the conversion to complete
	while (ADCSRA & (1 << ADSC)) {
	// do nothing
	}
	uint16_t raw_ref_2_5=ADC;
	
	ADC_0V_en(1);			// adc 0V circuit disconnected
	
	//Vbat = (1023*250)/ADC
	// Vbat = Vbat  - 250;
	
	uint32_t Vbat = 255750/raw_ref_2_5;
	uint8_t V = (uint8_t)(Vbat-250);
	
	//============== DEBUG ===================================================================
	//LED(1);
	//char buff[64];
	//sprintf(buff, "ref=%d      ", V);
	//
	//for(uint8_t i=0; i<10; ++i){
		//uart_transmit(buff[i]);
	//}
	//uart_transmit('\r');
	//LED(0);
	//========================================================================================
	return V;
}