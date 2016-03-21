#include <avr/io.h>

uint16_t adcReadings[]
uint8_t cellNumber;
uint8_t balanceByte;

void balance(void)
{
	memset(balanceByte, 0, sizeof(balanceByte));
	for(uint8_t c1 = 0; c1 < cellNumber; c1++){
		for(uint8_t c2 = c1 + 1; c2 < cellNumber; c2++){
			if(adcReadings[c1] > adcReadings[c2]){
				if((adcReadings[c1] - adcReadings[c2]) > 5){
					balanceByte[c1] = 0xFF;
				}
			}
			else{
				if((adcReadings[c2] - adcReadings[c1]) > 5){
					balanceByte[c2] = 0xFF;
				}
			}
		}
	}
}