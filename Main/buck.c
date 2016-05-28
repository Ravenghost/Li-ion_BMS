#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>

#include "buck.h"
#include "i2c_master.h"

void buck_init(void)
{
	//Set operation mode = CTC
	TCCR0A|= (1<<WGM01);
	//Set timer to work at 1000Hz frequency
	OCR0A = 249;
}

void buck(uint8_t state)
{
	if (state == ENABLE)
	{
		//Reset counter value
		TCNT0 = 0x00;
		//Set timer with prescaler = 64
		TCCR0B |= (1<<CS01)|(1<<CS00);
		//Enable CTC interrupt
		TIMSK0 |= (1<<OCIE0A);
		//Enable charger
		PORTB &= ~(1<<PORTB0);
		charger = 1;
		//Start adc
		buckAdc(START);
	}
	else
	{
		//Stop timer
		TCCR0B &= ~((1<<CS01)|(1<<CS00));
		//Disable CTC interrupt
		TIMSK0 &= ~(1<<OCIE0A);
		//Disable charger
		PORTB |= (1<<PORTB0);
		charger = 0;
		//Stop adc
		buckAdc(STOP);
	}
	//Reset duty cycle
	dutyCycle = 0;
	duty_cycle(dPot_write, 0);
	//Reset Current variable value
	adcReadings_I = 0;
}

void buckAdc_init(void)
{
	//AVcc reference, ADC0 channel
	ADMUX |= (1<<REFS0);
	//Enable ADC, enable ADC interrupt, set prescaler = 128
	ADCSRA |= (1<<ADEN)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}

void buckAdc(uint8_t state)
{
	//ADC start conversion
	if (state = START) ADCSRA |= (1<<ADSC);
	else ADCSRA &= ~(1<<ADSC);
}

void duty_cycle(uint8_t state, uint8_t data)
{
	//Send i2c message to set digital potentiometer value
	i2c_start(dPot_addr+I2C_WRITE);
	if (state == dPot_write)
	{
		i2c_write(state);
		i2c_write(data);
	}
	//Increase or decrease digital potentiometer value
	else i2c_write(state);
	i2c_stop();
}

ISR(TIMER0_COMPA_vect)
{
	//D=(53/8)*E1N(1.1*1023/ADC(N)) ISR wake
	//Don't charge if one cell is fully charged
	for (uint8_t x = 0; x < cellNumber; x++)
	{
		if (adcReadings[x] > 268) stopF = 1;
		else stopF = 0;
	}
	//CC mode
	if (adcReadingsMean < CV_U && stopF == 0)
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
	else if (adcReadingsMean >= CV_U && stopF == 0)
	{
		if ((adcReadingsMean < CV_U) && (adcReadings_I > CutOff_I) && (dutyCycle < 254))
		{
			//Increase duty cycle
			duty_cycle(dPot_incr, 0);
		}
		else if ((adcReadingsMean > CV_U) && (adcReadings_I > CutOff_I) && (dutyCycle > 2))
		{
			//Decrease duty cycle
			duty_cycle(dPot_decr, 0);
		}
	}
	else
	{
		//Disconnect charger
		buck(DISABLE);
	}
}

ISR(ADC_vect)
{
	uint8_t adcLow = ADCL;
	adcReadings_I = (ADCH<<8)|(adcLow);
	buckAdc(START);
}