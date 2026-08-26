#ifndef _UART_H_
#define _UART_H_

#include <stdbool.h>
#include <stdint.h>

extern volatile bool uart_rx_done;

int uart_printf(const char *fmt, ...);
int uart_printf_null(const char *fmt, ...);

#define UART_PRINTF uart_printf_null

void uart_init(uint32_t baudrate);
void uart_send(unsigned char *buff, int len);

#endif
