#ifndef ADC_H_
#define ADC_H_

void adc_init(void);
void adc_start(void);
ISR(ADC_vect);

#endif /* ADC_H_ */