#ifndef _DRV_RGB_H_
#define _DRV_RGB_H_

#include <stdint.h>

/* The real RGB driver bit-bangs WS2812 timing from flash-resident asm-ish
 * delays, so the tests link against a fake implementation instead. */
void drv_ws2812_gpio_init(void);
void drv_ws2812_set_color(uint8_t ch, unsigned long RGB);

#endif
