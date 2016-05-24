#include <avr/io.h>

#define cellBalance_TH 5

uint8_t cellNumber;
uint16_t adcReadings[cellNumber_MAX];

void cell_balance(void);
void array_reset(void);
void cellNumber_count(void);

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
				//Set balance byte for c2(adc incrementation ivnerted) if cell voltage c1 is higher than c2 and cellBalance threshold
				if ((adcReadings[c1] - adcReadings[c2]) > cellBalance_TH) balanceByte[c2] = 0xBB;
			}
			else
			{
				//Set balance byte for c1(adc incrementation ivnerted) if cell voltage c2 is higher than c1 and cellBalance threshold
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