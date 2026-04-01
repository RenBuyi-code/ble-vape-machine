#ifndef _APP_MOTOR_H_
#define _APP_MOTOR_H_

#include <stdint.h>
#include "gpio.h"
#include "uart2.h"


void motor_send_msg(uint8_t number);
uint8_t motor_receive(void);

#endif

