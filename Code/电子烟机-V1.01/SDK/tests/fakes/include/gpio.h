#ifndef _GPIO_H_
#define _GPIO_H_

#include <stdbool.h>
#include <stdint.h>
#include "BK3432_reg.h"

typedef enum { INPUT, OUTPUT, FLOAT, HIRESI } Dir_Type;
typedef enum { PULL_HIGH, PULL_LOW, PULL_NONE } Pull_Type;

#define GPIO_P00 0x00
#define GPIO_P01 0x01
#define GPIO_P02 0x02
#define GPIO_P03 0x03
#define GPIO_P04 0x04
#define GPIO_P05 0x05
#define GPIO_P06 0x06
#define GPIO_P07 0x07
#define GPIO_P10 0x10
#define GPIO_P11 0x11
#define GPIO_P12 0x12
#define GPIO_P13 0x13
#define GPIO_P14 0x14
#define GPIO_P15 0x15
#define GPIO_P16 0x16
#define GPIO_P17 0x17
#define GPIO_P20 0x20
#define GPIO_P21 0x21
#define GPIO_P22 0x22
#define GPIO_P23 0x23
#define GPIO_P24 0x24
#define GPIO_P25 0x25
#define GPIO_P26 0x26
#define GPIO_P27 0x27
#define GPIO_P30 0x30
#define GPIO_P31 0x31
#define GPIO_P32 0x32
#define GPIO_P33 0x33
#define GPIO_P34 0x34
#define GPIO_P35 0x35
#define GPIO_P36 0x36
#define GPIO_P37 0x37

void gpio_init(void);
void gpio_config(uint8_t gpio, Dir_Type dir, Pull_Type pull);
uint8_t gpio_get_input(uint8_t gpio);
uint8_t gpio_get_output(uint8_t gpio);
void gpio_set(uint8_t gpio, uint8_t val);

#endif
