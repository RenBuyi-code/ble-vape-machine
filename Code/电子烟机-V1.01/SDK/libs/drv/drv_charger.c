/**************************************************************************************************
  Filename:       drv_charger.c
  Revised:        $Date $
  Revision:       $Revision $


**************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "drv_charger.h"


void hal_charger_pw_gpio_init(void)
{
	   gpio_config(CHARGER_PW_GPIO, OUTPUT, PULL_NONE);
		gpio_set(CHARGER_PW_GPIO,0); 
}


void hal_charger_pw_open(void) 
{
	gpio_set(CHARGER_PW_GPIO,1); 
}


void hal_charger_pw_close(void) 	   
{
	gpio_set(CHARGER_PW_GPIO,0); 
}



