#include "tools.h"
#include "password.h"
#include "sys_manager.h"
#include "uart.h"

volatile const char ture_code_book[]="SMOYcKg2WS%N!j^R";
volatile const char encryption[] = "jinwandalaohu"; 
volatile static char code_book[12];
volatile static char dec[12];
unsigned char code_index = 0;

#define CODE_TEST 0


void password_get(char* buffer,char* dec_buffer) 
{

  uint8_t i = 0;             
  
  for(i=0;i<12;i++) 
  {
    dec[i]= (buffer[i]^encryption[i]);	
    code_index = (int)HexToAsc(dec[i]);
#if CODE_TEST > 0u  		
		UART_PRINTF("code_index:%d\r\n",code_index);	
#endif			
    code_index = code_index <= 18 ? code_index:0;
    //if(index > 11) index = 0;
    code_book[i] = __sys_manager.ble_mac_address[code_index]; 
		
  }

#if CODE_TEST > 0u  
	UART_PRINTF("dec:");
	for(i=0;i<12;i++)
  {
    UART_PRINTF("%X ",dec[i]);	
  }
	UART_PRINTF("\r\n");
	
  UART_PRINTF("code_book:");
	for(i=0;i<12;i++)
  {
    UART_PRINTF("%X",code_book[i]);	
  }
	UART_PRINTF("\r\n");
#endif	
	
  for(i=0;i<12;i++)
  {
    code_index = (int)HexToAsc(code_book[i]); 
    dec_buffer[i] = ture_code_book[code_index];   
  }
#if CODE_TEST > 0u  	
	UART_PRINTF("ture_code_book:");
	for(i=0;i<12;i++)
  {
    UART_PRINTF("%X",ture_code_book[i]);	
  }
	UART_PRINTF("\r\n");
#endif		

}
