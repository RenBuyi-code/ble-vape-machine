#include "drv_battery.h"
#include "adc.h"
#include "drv_load.h"

void Delay_ms(int num);

int drv_get_battery_vol(void)
{
	 int value = 0;


    value = (( (adc_get_value(4)*1000) /1023  )*1050)/1000;
    value*=6.5;
    value =0.0000011556 * value + 1.0854017010 *value - 455.2749756; //ÄâºÏ 110K ²¢ 20K ·¶Î§6.5 - 2.2V

    return value;
	
}
