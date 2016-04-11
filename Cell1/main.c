#ifndef F_CPU
#define F_CPU 4000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>

#include "Cells.h"
#include "I2C_slave.h"
#include "adc.h"

int main(void)
{
	//Change system clock prescaler = 2
	CLKPR = (1<<CLKPCE);
	CLKPR = (1<<CLKPS0);
	//Balance pin
	DDRB |= (1<<DDB0);
	adc_init();
	I2C_init(CELL1);
	sei();
	
	while (1)
	{
		if (rxbuffer[cellMemory_balance] == 0xBB)
		{
			PORTB |= (1<<PORTB0);
		}
		else
		{
			PORTB &= ~(1<<PORTB0);
		}
	}
}