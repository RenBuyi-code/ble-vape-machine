
#ifndef _APP_UART_H_
#define _APP_UART_H_

#include <stddef.h>     // standard definition
#include "timer.h"      // timer definition
#include "uart.h"       // uart definition
#include "BK3432_reg.h"
#include "reg_uart.h"   // uart register
#include "rwip.h"       // SW interface
#include "h4tl.h"
#if (NVDS_SUPPORT)
#include "nvds.h"                    // NVDS Definitions
#endif
#include "dbg.h"

typedef struct
{

    uint8_t  buffer[255];
    uint32_t data_len;

} uart_buffer_t;

extern uart_buffer_t uart_buffer;

void get_data_to_uart_buffer(uint8_t *buffer,uint32_t len);

#endif
