

#ifndef _ADC_H_
#define _ADC_H_



#include <stdbool.h>          // standard boolean definitions
#include <stdint.h>           // standard integer functions

#define ENABLE_ADC  1

#define VBAT_CH			4

void adc_init(uint8_t chanle,uint8_t mode);
void adc_isr(void);
//uint16_t adc_get_value(void);
uint16_t adc_get_value(uint8_t channel);
void adc_deinit(uint8_t channel);
uint16_t adc_get_vol(uint8_t channel);

#endif //



