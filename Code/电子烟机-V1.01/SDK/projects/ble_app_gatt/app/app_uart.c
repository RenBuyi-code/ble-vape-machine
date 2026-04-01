#include "app_uart.h"

uart_buffer_t uart_buffer;



void get_data_to_uart_buffer(uint8_t *buffer,uint32_t len)
{

    if(len > sizeof(uart_buffer.buffer))
    {
        len = sizeof(uart_buffer.buffer);
    }
    memset(uart_buffer.buffer,0,sizeof(uart_buffer.buffer));
    memcpy(uart_buffer.buffer,buffer,len);
}


