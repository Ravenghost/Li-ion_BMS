#include <avr/io.h>
#include <avr/interrupt.h>

void power_down(void);
void charger_init(void);

void power_down(void)
{
	//Sleep Enable
	SMCR |= (1<<SE);
	//Sleep mode = power-down
	SMCR |= (1<<SM1)|(1<<SM0);
	//Clear SE bit after waking up
	//SMCR &= ~(1<<SE);
}

void charger_init(void)
{
	//External interrupt request 0 = enable, low level of INT0 generates an interrupt request
	EIMSK |= (1<<INT0);
}