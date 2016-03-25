#ifndef F_CPU
#define F_CPU 1000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>

#include "Cells.h"
#include "I2C_slave.h"
#include "adc.h"

int main(void)
{
	//Balance pin
	DDRB |= (1<<DDB0);
	adc_init();
	I2C_init(CELL2);
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