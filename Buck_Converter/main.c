#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>

#define ENABLE 1
#define DISABLE 0
#define CV_U 1
#define CC_I 1
#define CutOff_I 1
#define START 1
#define STOP 0

volatile uint16_t adcReadings_U;
volatile uint16_t adcReadings_I;
uint16_t adcReadings_All;

void BuckIrt_init(void);
void BuckIrt(uint8_t state);
void adcBuck_init(void);
void adcBuck(uint8_t state);
void PWM_init(void);
void PWM(uint8_t state);

int main(void)
{
	BuckIrt_init();
	adcBuck_init();
	PWM_init();
	BuckIrt(ENABLE);
	adcBuck(START);
	sei();
	while (1)
	{
	}
}

ISR(TIMER0_COMPA_vect)
{
	PWM(ENABLE);
	//D=(53/8)*E1N(1.1*1023/ADC(N)) ISR wake
	//CC mode
	if (adcReadings_All < CV_U)
	{
		if (adcReadings_U < CC_I)
		{
			//Increase duty cycle
			OCR2B++;
		}
		else if (adcReadings_U > CC_I)
		{
			//Decrease duty cycle
			OCR2B--;
		}
	}
	//CV mode
	else //if (adcReadings_All >= CV_U)
	{
		if ((adcReadings_U < CV_U) && (adcReadings_I > CutOff_I))
		{
			//Increase duty cycle
			OCR2B++;
		}
		else if ((adcReadings_U > CV_U) && (adcReadings_I > CutOff_I))
		{
			//Decrease duty cycle
			OCR2B--;
		}
		else
		{
			//Disconnect charger
			PWM(DISABLE);
			BuckIrt(DISABLE);
		}
	}
}

ISR(ADC_vect)
{
	if (ADMUX & 0x01)
	{
		adcReadings_I = ADCL;
		adcReadings_I |= (ADCH<<8);
	}
	else
	{
		adcReadings_U = ADCL;
		adcReadings_U |= (ADCH<<8);
	}
	//Change from ADC channel 0 to 1
	ADMUX ^= (1<<MUX0);
	adcBuck();
}

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

void PWM_init(void)
{
	//Set OC2B pin as output
	DDRD |= (1<<DDD3);
	//Set operation mode = Fast PWM non-inverting, PWM pin = OC2B
	TCCR2A |= (1<<COM2B1)|(1<<WGM21)|(1<<WGM20);
	TCCR2B |= (1<<WGM22);
	//Set PWM to work at 100kHz frequency
	OCR2A = 159;
	//Duty cycle
	OCR2B = 0;
}

void PWM(uint8_t state)
{
	if (state == ENABLE)
	{
		//Reset counter value
		TCNT2 = 0x00;
		//Set prescaler = 1
		TCCR2B |= (1<<CS20);
	}
	else
	{
		//Stop timer
		TCCR2B &= ~(1<<CS20);
	}
}