#ifndef __GPIO_DEMO_H__
#define __GPIO_DEMO_H__

#include <stddef.h>     
#include "BK3432_reg.h"
#include "gpio.h"

#define L1_GPIO       GPIO_P33
#define L2_GPIO       GPIO_P13
#define L3_GPIO       GPIO_P12
#define L4_GPIO       GPIO_P11
#define L5_GPIO       GPIO_P10
#define L6_GPIO       GPIO_P07
#define L7_GPIO       GPIO_P14
#define L8_GPIO       GPIO_P35
#define L9_GPIO   	 GPIO_P32
#define L10_GPIO  	 GPIO_P31
#define L_POWER_GPIO  GPIO_P05


#define LED_GPIO  	  GPIO_P33
#define VBAT_AIN_CTL   GPIO_P01

#define MAX_LOAD_NUMBER 7
void hal_load_gpio_init(void);
uint8_t hal_load_close(uint8_t load_number);
uint8_t hal_load_open(uint8_t load_number);
void hal_load_power_open(void);
void hal_load_power_close(void);
void hal_lock_open(uint8_t load_number);


void hal_led_open(void);
void hal_led_close(void);

void hal_adc_ain_init(void);
void hal_adc_ain_open(void);
void hal_adc_ain_close(void);
uint16_t get_delay_time(uint16_t vol);
void hal_load_open_all(void);

#endif /* HEARTRATE_H */


