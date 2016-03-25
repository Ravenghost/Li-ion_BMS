#include <avr/io.h>

uint8_t cellNumber;
void cell_adcReadings(void);

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
