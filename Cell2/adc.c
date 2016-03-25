#include <avr/io.h>
#include <avr/interrupt.h>

#include "adc.h"

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