#ifndef _WDT_H_
#define _WDT_H_

#include <stdint.h>

void wdt_feed(uint16_t wdt_cnt);
void wdt_enable(uint16_t wdt_cnt);
void wdt_disable(void);

#endif
