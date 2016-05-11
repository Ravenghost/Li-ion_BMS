#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "i2c_master.h"
#include "uart.h"
#include "Cells.h"

#define UART_BAUD_RATE 4800

uint8_t cellNumber;
uint16_t adcReadings[cellNumber_MAX];
char adcDisplay[4];

void cellNumber_count(void);
void cell_adcReadings(void);
void cell_balance(void);
float adcConvert(uint16_t readings);

int main(void)
{
	uart_init(UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU));
	i2c_init();
	sei();
	//_delay_ms(10);

	while(1)
	{
		cellNumber_count();
		cell_adcReadings();
		cell_balance();
	
		uart_putc(0xC);
		uart_puts("Cele Itampa\r");
		uart_puts("1    ");
		uart_puts(dtostrf(adcConvert(adcReadings[0]), 4, 2, adcDisplay));
		uart_puts(" V\r");
		uart_puts("2    ");
		uart_puts(dtostrf(adcConvert(adcReadings[1]), 4, 2, adcDisplay));
		uart_puts(" V\r");
		uart_puts("3    ");
		uart_puts(dtostrf(adcConvert(adcReadings[2]), 4, 2, adcDisplay));
		uart_puts(" V");
		_delay_ms(500);
		
	}
	
	return 0;
}

void cellNumber_count(void)
{
	cellNumber = 0;
	//Count connected cells by checking if sending cell address returns 0(acknowledge)
	for (uint8_t x = 0; x < cellAddress_MAX; x+=2)
	{
		if (i2c_start(x) == 0)
		{
			cellNumber++;
			i2c_stop();
		}
	}
}

void cell_adcReadings(void)
{
	uint8_t c = cellAddress_MIN;
	//read adc results and store them to adcReadings array
	for (uint8_t x = 0; x < cellNumber; x++)
	{
		//Set i2c address pointer to adc readings memory
		i2c_start(c+I2C_WRITE);
		i2c_write(cellMemory_adc);
		i2c_stop();
		//Start reading adc information and write to array
		i2c_start(c+I2C_READ);
		adcReadings[x] = i2c_read_ack();
		adcReadings[x] |= (i2c_read_nack()<<8);
		i2c_stop();
		c+=2;
	}
}

void cell_balance(void)
{
	uint8_t balanceByte[cellNumber_MAX] = {0};
	uint8_t c = cellAddress_MIN;
	
	for (uint8_t c1 = 0; c1 < cellNumber; c1++)
	{
		//Cycle compares c1[0...cellNumber] cells to c2[c1+1...cellNumber] cells voltage
		for (uint8_t c2 = c1 + 1; c2 < cellNumber; c2++)
		{
			if (adcReadings[c1] > adcReadings[c2])
			{
				//Set balance byte for c2(adc incrementation inverted) if cell voltage c1 is higher than c2 and cellBalance_TH threshold
				if ((adcReadings[c1] - adcReadings[c2]) > cellBalance_TH) balanceByte[c2] = 0xBB;
			}
			else
			{
				//Set balance byte for c1(adc incrementation inverted) if cell voltage c2 is higher than c1 and cellBalance_TH threshold
				if ((adcReadings[c2] - adcReadings[c1]) > cellBalance_TH) balanceByte[c1] = 0xBB;
			}
		}
	}
	//Write balance bytes to cells
	for (uint8_t x = 0; x < cellNumber; x++)
	{
		//Set i2c address pointer to balance memory and write balance byte
		i2c_start(c+I2C_WRITE);
		i2c_write(cellMemory_balance);
		i2c_write(balanceByte[x]);
		i2c_stop();
		c+=2;
	}
}

float adcConvert(uint16_t readings)
{
	float result;
	//Convert 10bit adc value to voltage
	result = (1.1*1024)/readings;
	return result;
}