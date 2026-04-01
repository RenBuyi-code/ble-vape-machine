#ifndef _i2c_s_H_
#define _i2c_s_H_

#include "stdint.h"

uint8_t i2cs_rx_byte(char ack);
uint8_t i2cs_tx_byte(uint8_t dat);
void i2cs_tx_data(uint8_t devAddr7,uint8_t addr,uint8_t*buf,uint8_t size);
void i2cs_rx_data(uint8_t devAddr7,uint8_t addr,uint8_t*buf,uint8_t size);
void i2cs_delay(uint16_t dly_cnt);




#define SCLK GPIO_P11
#define SDAT GPIO_P10

#define SCL0_HIGH()		  gpio_set(SCLK,1)
#define SCL0_LOW()		  gpio_set(SCLK,0)
#define SDA0_HIGH()		  gpio_set(SDAT,1)
#define SDA0_LOW()		  gpio_set(SDAT,0)
#define SDA0_SetInput()	  gpio_config(SDAT,INPUT,PULL_NONE)
#define SDA0_SetOutput()  gpio_config(SDAT,OUTPUT,PULL_NONE)
#define SDA0_READ()		  gpio_get_input(SDAT)

void i2cs_start(void);
void i2cs_stop(void);





#endif
