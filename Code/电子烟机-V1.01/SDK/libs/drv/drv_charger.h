#ifndef  __DRV_CHARGER_H__
#define __DRV_CHARGER_H__

#include "gpio.h"



#define CHARGER_PW_GPIO   GPIO_P04


void hal_charger_pw_gpio_init(void);
void hal_charger_pw_open(void);
void hal_charger_pw_close(void);

#endif /* HEARTRATE_H */
