#ifndef _APP_MOTOR_H_
#define _APP_MOTOR_H_

#include <stdint.h>
#include "gpio.h"
#include "uart2.h"


uint8_t motor_send_msg(uint8_t number); //0 失败 1 成功
uint8_t motor_receive(void);

#endif

