#ifndef  _DRV_RGB_H_
#define  _DRV_RGB_H_

#include "stdint.h"



#define RGB_POWER1_ON    gpio_set(GPIO_P11, 1)
#define RGB_POWER1_OFF   gpio_set(GPIO_P11, 0)


#define RGB_POWER2_ON   gpio_set(GPIO_P12, 1)
#define RGB_POWER2_OFF  gpio_set(GPIO_P12, 0)

void drv_ws2812_gpio_init(void);
void drv_ws2812_set_color(uint8_t ch, unsigned long RGB);
void rgb_test_hi(uint8_t ch);
void rgb_test_low(uint8_t ch);

void drv_ws2812_testch1(uint8_t ch, uint32_t RGB_TABLE[],uint32_t len);
uint32_t adjustBrightness(uint32_t rgb,int step);

#endif


