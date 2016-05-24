#ifndef BUCK_H_
#define BUCK_H_

uint8_t dutyCycle;

#define ENABLE 1
#define DISABLE 0
#define CV_U 4.19
#define CC_I 512
#define CutOff_I 102
#define START 1
#define STOP 0

#define dPot_addr 0x5C
#define dPot_write 0x00
#define dPot_incr 0x04
#define dPot_decr 0x08

void BuckIrt_init(void);
void BuckIrt(uint8_t state);
void adcBuck_init(void);
void adcBuck(uint8_t state);
void duty_cycle(uint8_t state, uint8_t data);

ISR(TIMER0_COMPA_vect);
ISR(ADC_vect);

#endif /* BUCK_H_ */