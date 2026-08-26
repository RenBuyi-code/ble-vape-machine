#ifndef _KE_TIMER_H_
#define _KE_TIMER_H_

#include <stdint.h>

void ke_timer_set(uint16_t timer_id, uint16_t task_id, uint32_t delay);
void ke_timer_clear(uint16_t timer_id, uint16_t task_id);

#endif
