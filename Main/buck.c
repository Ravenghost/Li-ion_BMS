#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>

#include "buck.h"
#include "i2c_master.h"

void BuckIrt_init(void)
{
	//Set operation mode = CTC
	TCCR0A|= (1<<WGM01);
	//Set timer to work at 1000Hz frequency
	OCR0A = 249;
}

void BuckIrt(uint8_t state)
{
	if (state == ENABLE)
	{
		//Reset counter value
		TCNT0 = 0x00;
		//Set timer with prescaler = 64
		TCCR0B |= (1<<CS01)|(1<<CS00);
		//Enable CTC interrupt
		TIMSK0 |= (1<<OCIE0A);
	}
	else
	{
		//Stop timer
		TCCR0B &= ~((1<<CS01)|(1<<CS00));
		//Disable CTC interrupt
		TIMSK0 &= ~(1<<OCIE0A);
	}
	//Reset duty cycle
	dutyCycle = 0;
}

void adcBuck_init(void)
{
	//AVcc reference, ADC0 channel
	ADMUX |= (1<<REFS0);
	//Enable ADC, enable ADC interrupt, set prescaler = 128
	ADCSRA |= (1<<ADEN)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}

void adcBuck(uint8_t state)
{
	//ADC start conversion
	if (state = START) ADCSRA |= (1<<ADSC);
	else ADCSRA &= ~(1<<ADSC);
}

void duty_cycle(uint8_t state, uint8_t data)
{
	i2c_start(dPot_addr+I2C_WRITE);
	if (state == dPot_write)
	{
		i2c_write(state);
		i2c_write(data);
	}
	else i2c_write(state);
	i2c_stop();
}

ISR(TIMER0_COMPA_vect)
{
	mean_readings();
	//D=(53/8)*E1N(1.1*1023/ADC(N)) ISR wake
	//CC mode
	if (adcReadingsMean < CV_U)
	{
		if ((adcReadings_I < CC_I) && (dutyCycle < 254))
		{
			//Increase duty cycle
			duty_cycle(dPot_incr, 0);
		}
		else if ((adcReadings_I > CC_I && (dutyCycle > 2))
		{
			//Decrease duty cycle
			duty_cycle(dPot_decr, 0);
		}
	}
	//CV mode
	else //if (adcReadings_All >= CV_U)
	{
		if ((adcReadingsMean < CV_U) && (adcReadings_I > CutOff_I))
		{
			//Increase duty cycle
			OCR2B++;
		}
		else if ((adcReadingsMean > CV_U) && (adcReadings_I > CutOff_I))
		{
			//Decrease duty cycle
			OCR2B--;
		}
		else
		{
			//Disconnect charger
			BuckIrt(DISABLE);
		}
	}
}

ISR(ADC_vect)
{
	uint8_t adcLow = ADCL;
	adcReadings_I = (ADCH<<8)|(adcLow);
	adcBuck(START);
}