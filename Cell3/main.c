#ifndef F_CPU
#define F_CPU 1000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>

#include "Cells.h"
#include "I2C_slave.h"

void adc_init(void);
void adc_start(void);

int main(void)
{
	//Balance pin
	DDRB |= (1<<DDB0);
	adc_init();
	I2C_init(CELL3);
	sei();
	
	while (1)
	{
		if (rxbuffer[0x0] == 0xFF)
		{
			PORTB |= (1<<PORTB0);
		}
		else
		{
			PORTB &= ~(1<<PORTB0);
		}
	}
}

ISR(ADC_vect)
{
	txbuffer[0xA] = ADCL;
	txbuffer[0xB] = ADCH;
	adc_start();
}

void adc_init(void)
{
	//AVcc reference, 1.1V (VBG) channel
	ADMUX |= (1<<REFS0)|(1<<MUX3)|(1<<MUX2)|(1<<MUX1);
	//Enable ADC, enable ADC interrupt, set prescaler = 128
	ADCSRA |= (1<<ADEN)|(1<<ADPS2)|(1<<ADIE);
	adc_start();
}

void adc_start(void)
{
	//ADC start conversion
	ADCSRA |=(1<<ADSC);
}