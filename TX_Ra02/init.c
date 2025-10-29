/*
 * init.c
 *
 * Created: 26.10.2025 9:38:27
 *  Author: centr
 */ 
#include "init.h"
#include "SHT30.h"
#include "i2c.h"
#include "SPI.h"
#include "UART.h"
#include "Ra02_LoRa.h"
#include "ADC.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#include <avr/sleep.h> //Needed for sleep_mode
#include <avr/power.h> //Needed for powering down perihperals such as the ADC/TWI and Timers

#define SCL_CLOCK 100000UL // I2C clock frequency (100kHz standard)

void GPIO_init(void){
	//----------- GPIO -----------
	// USED 6 digital pins
	// PD7(LED)=output
	// PB0(3V3 power-on)=output
	// PB1(RST Ra-02)=output
	// PB2(CS Ra-02)=output
	// PC3( 0V connect to ADC circuit )=output
	// PC6( 0V connect to FRAM )=output
	// PC2( charge detect )=input
	// DDRx 0-input; 1-otput
	//----------------------------
	
	PORTD = 0x00;			// The Port B Data Register 0 default;
	PORTB = 0x00;
	PORTC = 0x00;
	
	//-----------------------------------------------------------------------------------------
	//DDRx The Port  Data Direction Register 0 - input; / 1 - output;
	DDRD  = 0b10000101;		// PD7(LED)=output; 
	DDRB  = 0b00000001;		// PB5(CLK SPI)=output; PB3(MOSI SPI)=output; PB2(CS SPI)=output; PB1(RST Ra-02)=output; PB0-(3V3-ON)=output;
	DDRC  = 0b01001000;		// PC7(+V SHT30)=output; PC6(0V FRAM)=output; PC3(0V ADC)=output; 
	
	//-----------------------------------------------------------------------------------------
	// P-UP on unused pins (input)
	PORTD = 0b00111111;		// 1=P-UP. unused pins (input) PD0...PD6 - P-UPed
	PORTB = 0b00000000;		// 1=P-UP.
	PORTC = 0b00111010;		// 1=P-UP. unused pins (input) PC1-P-UPed; PC6(0V SHT30)output = 0; PC4(SDA)-P-UP, PC5(SCL)-P-UP; PC3(ADC 0V)output = 1; 
	
	//----------------------------------------------------------------------------------------
	DIDR0 = 0b00111100;		// PC5...2 digital pins
}

void GPIO_de_init(void){
	//----------- GPIO -----------
	// PB0(power 3v3)=output 0

	//-----------------------------------------------------------------------------------------
	//DDRx The Port  Data Direction Register 0 - input; / 1 - output;
	DDRD  = 0b10000000;		// PD7(LED)=output;
	DDRB  = 0b00000001;		// PB5(CLK SPI)=output; PB3(MOSI SPI)=output; PB2(CS SPI)=output; PB1(RST Ra-02)=output; PB0-(3V3-ON)=output;
	DDRC  = 0b00000000;		// PC7(+V SHT30)=output; PC6(0V FRAM)=output; PC3(0V ADC)=output;
	
	//-----------------------------------------------------------------------------------------
	// P-UP on unused pins (input)
	PORTD = 0b01111111;		// 1=P-UP. unused pins (input) PD0...PD6 - P-UPed
	PORTB = 0b11111110;		// 1=P-UP.
	PORTC = 0b10111111;		// 1=P-UP. unused pins (input) PC1-P-UPed; PC6(0V SHT30)output = 0; PC4(SDA)-P-UP, PC5(SCL)-P-UP; PC3(ADC 0V)output = 1;
	
	//----------------------------------------------------------------------------------------
	DIDR0 = 0b00000000;		// PC5...2 digital pins
}


void timer1_init (void){
	// инициализация Timer1
	cli();					// отключить глобальные прерывани¤
	TCCR1A = 0;				// установить регистры в 0
	TCCR1B = 0;
	
	OCR1A = 990;			// установка регистра совпадени¤  OCR1A/(1e6/1024)
	TCCR1B |= (1 << WGM12);	// включение в CTC режим
	TCCR1B |= (1 << CS10);	// ”становка битов CS10 и CS12 на коэффициент делени¤ 1024
	TCCR1B |= (1 << CS12);
	TIMSK1 |= (1 << OCIE1A); // включение прерываний по совпадению
	sei();					// включить глобальные прерывани¤
}


void sleep_init (void){
	  //Power down various bits of hardware to lower power usage
	  set_sleep_mode(SLEEP_MODE_PWR_SAVE);
	  sleep_enable();

	  //Shut off ADC, TWI, SPI, Timer0, Timer1

	  ADCSRA &= ~(1<<ADEN); //Disable ADC
	  
	  ACSR = (1<<ACD); //Disable the analog comparator
	  DIDR0 = 0x3F; //Disable digital input buffers on all ADC0-ADC5 pins
	  DIDR1 = (1<<AIN1D)|(1<<AIN0D); //Disable digital input buffer on AIN1/0
}

void sleep_idle_startup (unsigned char status){
	if (status>0){
		
		
			activation_of_electrical_circuits (0);
			SPI_MasterDeInit();
			i2c_deinit();
			uart_deinit();
			GPIO_de_init();
			
			
		SMCR = 0b00000001;		// Sleep Mode - idle, sleep - enabled
		sleep_cpu();
	}
	else {
		SMCR = 0b00000000;		// sleep - disabled
	}
}

void activation_of_electrical_circuits (uint8_t en){
	if (en == 1){
		GPIO_init();
		ADC_0V_en(0);			// adc 0V circuit connected
		init_adc();
		DIDR0 = 0b00111100;		// PC5...2 digital pins
		SHT30_0V_en(0);
		_delay_ms(5);			// Small delay for stabilization
	}
	else {
		LED(0);
		ADC_0V_en(1);			// adc 0V circuit disconnected
		deinit_adc();
		DIDR0 = 0b00000000;		// PC5...0 analog pins, minimum consumption
		SHT30_0V_en(1);
	}
}

