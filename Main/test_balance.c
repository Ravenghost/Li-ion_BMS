#include <avr/io.h>

#define cellBalance 5

uint8_t cellNumber;
uint16_t adcReadings[cellNumber_MAX];
uint8_t balanceByte[cellNumber_MAX];

void cell_balance(void);
void array_reset(void);
void cellNumber_count(void);

void cell_balance(void)
{
	array_reset();
	for (uint8_t c1 = 0; c1 < cellNumber; c1++)
	{
		//Cycle compares c1[0...cellNumber] cells to c2[c1+1...cellNumber] cells voltage
		for (uint8_t c2 = c1 + 1; c2 < cellNumber; c2++)
		{
			if (adcReadings[c1] > adcReadings[c2])
			{
				//Set balance byte if cell voltage c1 is higher than c2 and cellBalance threshold
				if ((adcReadings[c1] - adcReadings[c2]) > cellBalance) balanceByte[c1] = 0xFF;
			}
			else
			{
				//Set balance byte if cell voltage c2 is higher than c1 and cellBalance threshold
				if ((adcReadings[c2] - adcReadings[c1]) > cellBalance) balanceByte[c2] = 0xFF;
			}
		}
	}
	uint8_t c = cellAddress_MIN;
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

void cellNumber_count(void)
{
	cellNumber = 0;
	for (uint8_t x = 0; x < cellAddress_MAX; x+=2)
	{
		if (i2c_start(x) == 0) cellNumber++;
	}
}


/*void array_reset(void)
{
	uint16_t *array_p;
	array_p = balanceByte;

	for (uint8_t x = 0; x < sizeof(balanceByte); x++)
	{
		*array_p = 0;
		*array_p++;
	}
}
*/
void array_reset(void)
{
	//Fill balanceByte array with 0
	for (uint8_t x = 0; x < sizeof(balanceByte); x++)
	{
		balanceByte[x] = 0;
	}
}