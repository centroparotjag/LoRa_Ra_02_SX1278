/*
 * ADC.h
 *
 * Created: 29.10.2025 9:43:13
 *  Author: centr
 */ 


#ifndef ADC_H_
#define ADC_H_
#include <avr/io.h>

void deinit_adc(void);
void init_adc(void);
uint8_t Vbat_adc_read(void);

#endif /* ADC_H_ */