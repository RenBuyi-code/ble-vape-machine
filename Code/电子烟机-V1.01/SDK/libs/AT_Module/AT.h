#ifndef _AT_H_
#define _AT_H_

#include "sys_manager.h"

#define AT_DISC "AT+DISC"
#define AT_NAME "AT+NAME"
#define AT_ADVIN "AT+ADVIN5"
#define AT_SLEEP "AT+SLEEP1"

typedef struct 
{
  const char* AT_CMD;
  ERR_CODE(*cmd_cb)(char* buffer);
}AT;


#define  SHELL_LEN  13
extern AT AT_Shell[SHELL_LEN];

unsigned char  ble_status_change(char* AT_CMD,char* rx_buffer); 
ERR_CODE at_set_mac_addres_cb(char* buffer);
ERR_CODE at_traverse(char* buffer,AT at_shell[]);
void error_event_report(void);

#endif
