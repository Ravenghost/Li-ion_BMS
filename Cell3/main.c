#ifndef F_CPU
#define F_CPU 1000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#include "I2C_slave.h"

int main(void){
	DDRB |= 1<<DDB0;
	
	I2C_init(0x6);
	sei();
	
	ADMUX = (1<<REFS0)|(1<<MUX3)|(1<<MUX2)|(1<<MUX1);
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADIE);
	ADCSRA |=1<<ADSC;
	
	while(1){
		if(rxbuffer[0x0] == 0xFF){
			PORTB |= 1<<PORTB0;
		}
		else{
			PORTB |= 0<<PORTB0;
		}
	}
	
	return 0;
}

ISR(ADC_vect)
{
	txbuffer[0xA] = ADCL;
	txbuffer[0xB] = ADCH;
	
	ADCSRA |=1<<ADSC;
}