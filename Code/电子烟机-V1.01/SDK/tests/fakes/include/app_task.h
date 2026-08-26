#ifndef _APP_TASK_H_
#define _APP_TASK_H_

#include <stdint.h>

#define SEC (100) /* kernel ticks per second, mirrors the firmware value */

#define TASK_APP 20

#define APP_LOAD_CLOSE_TIMER 1
#define APP_CHAR_CLOSE_TIMER 2
#define APP_OUTPUT_TIMEOUT_TIMER 3

#endif
