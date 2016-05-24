#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>

#include "uart.h"
#include "Cells.h"

#define UART_BAUD_RATE 9600

#define cellReading 1
#define mainReading 0

char adcDisplay[4];
char cellDisplay;
uint8_t cellNumber;
uint16_t adcReadings[cellNumber_MAX];
uint8_t balanceByte[cellNumber_MAX];
volatile uint16_t adcReadings_I;
uint8_t charger;

void uart_out(void);
float adcConvert(uint16_t readings, uint8_t mode);

int main(void)
{
    uart_init(UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU));
	sei();
	
    while (1) 
    {
		uart_out();
    }
	
	return 0;
}

void uart_out(void)
{
	//Outputs adc measurements to PC
	uart_puts("Start");
	uart_putc('\n');
	uart_putc(itoa(cellNumber, cellDisplay, 10));
	uart_putc('\n');
	for (uint8_t x = 0; x < cellNumber; x++)
	{
		uart_puts(dtostrf(adcConvert(adcReadings[x], cellReading), 4, 2, adcDisplay));
		uart_putc('\n');
		uart_putc(itoa(cellNumber, balanceByte[x], 10));
		uart_putc('\n');
	}
	uart_puts(dtostrf(adcConvert(adcReadings_I, mainReading), 4, 2, adcDisplay));
	uart_putc('\n');
	uart_putc(itoa(charger, cellDisplay, 10));
	uart_putc('\n');
}

float adcConvert(uint16_t readings, uint8_t mode)
{
	float result;
	//Convert 10bit adc value to voltage, current
	if (mode == cellReading) result = (1.1*1024)/readings;
	else result = (readings*1024)/5;
	return result;
}
