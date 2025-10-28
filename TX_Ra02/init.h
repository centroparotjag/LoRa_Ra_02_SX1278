/*
 * init.h
 *
 * Created: 26.10.2025 9:39:23
 *  Author: centr
 */ 


#ifndef INIT_H_
#define INIT_H_

#ifndef F_CPU
#define F_CPU 8000000UL
#endif
#include <util/delay.h>

#define ADC_0V_en(i)	( (i) ? (PORTC |= (1 << PC3)) : (PORTC &= ~(1<<PC3)) )
#define SHT30_0V_en(i)	( (i) ? (PORTD |= (1 << PD2)) : (PORTD &= ~(1<<PD2)) )
#define POW_3V3_en(i)	( (i) ? (PORTB |= (1 << PB0)) : (PORTB &= ~(1<<PB0)) )
#define	LED(i)			( (i) ? (PORTD |= (1 << PD7)) : (PORTD &= ~(1<<PD7)) )

#define	CS_Ra02(i)		( (i) ? (PORTB |= (1 << PB2)) : (PORTB &= ~(1<<PB2)) )
#define	RST_Ra02(i)		( (i) ? (PORTB |= (1 << PB1)) : (PORTB &= ~(1<<PB1)) )

void GPIO_init(void);
void i2c_init(void) ;
void sleep_init (void);
void sleep_idle_startup (unsigned char status);


#endif /* INIT_H_ */