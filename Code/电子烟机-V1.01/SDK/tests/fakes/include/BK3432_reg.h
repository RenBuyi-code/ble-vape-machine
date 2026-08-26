#ifndef _FAKE_BK3432_REG_H_
#define _FAKE_BK3432_REG_H_

/* Host-side stand-in for the BK3432 register map: the tests never touch real
 * registers, so every register is backed by a plain variable. */
#include <stdint.h>

extern uint32_t fake_reg_scratch;

#define REG_APB5_GPIOA_DATA fake_reg_scratch
#define REG_APB5_GPIOB_DATA fake_reg_scratch
#define REG_APB5_GPIOC_DATA fake_reg_scratch
#define REG_APB5_GPIOD_DATA fake_reg_scratch

#endif
