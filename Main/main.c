#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>

#include "i2c_master.h"
#include "uart.h"
#include "Cells.h"
#include "buck.h"

#define UART_BAUD_RATE 9600

#define cellReading 1
#define mainReading 0

uint8_t cellNumber;
uint8_t balanceByte[cellNumber_MAX];
uint16_t adcReadings[cellNumber_MAX];
volatile float adcReadingsMean;
volatile uint16_t adcReadings_I;
uint8_t charger;

void cellNumber_count(void);
void cell_adcReadings(void);
void cell_balance(void);
void uart_out(void);
float adcConvert(uint16_t readings, uint8_t state);
void mean_readings(void);

int main(void)
{
	uart_init(UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU));
	i2c_init();
	BuckIrt_init()
	adcBuck_init();
	sei();

	while(1)
	{
		cellNumber_count();
		cell_adcReadings();
		cell_balance();
		uart_out();
		
		BuckIrt(ENABLE);
		adcBuck(START);
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
	//Clearing balanceByte array to form a new one filled with correct bytes
	memset(balanceByte, 0, cellNumber_MAX);	
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

void uart_out(void)
{
	char adcDisplay[4];
	char cellDisplay;
	//Outputs BMS information and measurements to computer
	uart_puts("Start");
	uart_putc('\n');
	uart_putc(*utoa(cellNumber, &cellDisplay, 10));
	uart_putc('\n');
	for (uint8_t x = 0; x < cellNumber; x++)
	{
		uart_puts(dtostrf(adcConvert(adcReadings[x], cellReading), 4, 2, adcDisplay));
		uart_putc('\n');
		uart_putc(*utoa(balanceByte[x], &cellDisplay, 10));
		uart_putc('\n');
	}
	uart_puts(dtostrf(adcConvert(adcReadings_I, mainReading), 4, 2, adcDisplay));
	uart_putc('\n');
	uart_putc(*utoa(charger, &cellDisplay, 10));
	uart_putc('\n');
}

float adcConvert(uint16_t readings, uint8_t state)
{
	float result;
	//Convert 10bit adc value to voltage, current
	if (state == cellReading) result = (1.1*1024)/readings;
	else result = (readings*1024)/5;
	return result;
}

void mean_readings(void)
{
	//Calculate mean of cell adc readings
	float temp;
	for (uint8_t x = 0; x < cellNumber; x++) temp += adcConvert(adcReadings[x], cellReading);
	adcReadingsMean = temp / cellNumber;
}