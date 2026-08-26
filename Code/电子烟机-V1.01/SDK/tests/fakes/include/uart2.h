#ifndef _UART2_H_
#define _UART2_H_

#include <stdint.h>

void uart2_init(uint32_t baudrate);
void uart2_send(unsigned char *buff, int len);

#endif
