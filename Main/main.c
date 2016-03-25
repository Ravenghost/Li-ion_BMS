#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "I2C_master.h"
#include "uart.h"
#include "Cells.h"

#define UART_BAUD_RATE 9600

uint16_t adcReadings[3];
char adcDisplay[4];

float adcConvert(uint16_t Readings);

int main(void)
{
	uart_init(UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU));
	i2c_init();
	sei();
	_delay_ms(10);

	while(1){
	i2c_start(CELL1+I2C_WRITE);
	i2c_write(0xA);
	i2c_stop();
	
	i2c_start(CELL1+I2C_READ);
	adcReadings[0] = i2c_read_ack();
	adcReadings[0] |= (i2c_read_nack()<<8);
	i2c_stop();
	
	i2c_start(CELL2+I2C_WRITE);
	i2c_write(0xA);
	i2c_stop();
	
	i2c_start(CELL2+I2C_READ);
	adcReadings[1] = i2c_read_ack();
	adcReadings[1] |= (i2c_read_nack()<<8);
	i2c_stop();
	
	i2c_start(CELL3+I2C_WRITE);
	i2c_write(0xA);
	i2c_stop();
	
	i2c_start(CELL3+I2C_READ);
	adcReadings[2] = i2c_read_ack();
	adcReadings[2] |= (i2c_read_nack()<<8);
	i2c_stop();
	
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

float adcConvert(uint16_t Readings)
{
	float Result;
	//Convert 10bit adc value to voltage
	Result = (1.1*1023)/Readings;
	return Result;
}
