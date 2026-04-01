/**************************************************************************************************
  Filename:       drv_load.c
  Revised:        $Date $
  Revision:       $Revision $


**************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "drv_load.h"


uint8_t LOAD_CTR_GPIO_TAB[MAX_LOAD_NUMBER] = 
{
	0Xff,			
	L1_GPIO,
	L2_GPIO,
	L3_GPIO,
	L4_GPIO,
	L5_GPIO,
	L6_GPIO,
};


void hal_led_init(void)
{
	gpio_config(LED_GPIO,OUTPUT,PULL_NONE); //连接灯
}

void hal_led_open(void)
{
	gpio_set(LED_GPIO,1);
	//RGB_IO_H;
}

void hal_led_close(void)
{
	gpio_set(LED_GPIO,0);
	//RGB_IO_L;
}


unsigned int delay_time_ms = 100;

uint16_t get_delay_time(uint16_t vol)
{
	uint16_t time = 100;
	if(vol >= 6000 && vol < 7000)
	{
		time = 100;
	}
	else if(vol >= 5000 && vol < 6000)
	{
		time = 100;
	}
	else if(vol >= 4000 && vol < 5000)
	{
		time = 150;	
	}
	else if(vol >= 2800 && vol < 4000)
	{
		time = 140;
	}
	else if( vol < 2800)
	{
		time = 180;
	}
	
	return time;
}
void hal_charger_pw_gpio_init(void);

void hal_load_gpio_init(void)
{
		uint8_t cnt = 0;
	
		hal_charger_pw_gpio_init();
		hal_adc_ain_init();
		hal_led_init();
		hal_led_close();
		
		gpio_config(L_POWER_GPIO, OUTPUT, PULL_NONE);
		hal_load_power_close();
	
		for( cnt=1; cnt < sizeof(LOAD_CTR_GPIO_TAB) ; cnt++ ) 
		{	
				gpio_config(LOAD_CTR_GPIO_TAB[cnt],OUTPUT,PULL_NONE); //负载控制引脚初始化
				gpio_set(LOAD_CTR_GPIO_TAB[cnt],0); 		//关闭负载
		}
}


void Delay_ms(int num);

uint8_t hal_load_open(uint8_t load_number) // 负载编号不可以超过数组大小
{
		if(load_number > (MAX_LOAD_NUMBER-1) || load_number==0) 
			return 0 ;//打开失败
		
		gpio_set(LOAD_CTR_GPIO_TAB[load_number],1); // NMOS驱动
			
		return 1;//打开成功
}

uint8_t hal_load_close(uint8_t load_number) 	   //负载编号不可以超过数组大小
{
		if(load_number > (MAX_LOAD_NUMBER-1) || load_number==0) 
			return 0 ; 
		
		gpio_set(LOAD_CTR_GPIO_TAB[load_number],0); 
		//hal_load_power_close();

		return 1;//打开成功
}




#include "uart.h"
void hal_load_power_open(void)
{
	UART_PRINTF("hal_load_power_open\r\n");
	gpio_set(L_POWER_GPIO,1); //打开电源 使用最好加延时
	hal_adc_ain_open();	
}

void hal_load_power_close(void)
{
	//("hal_load_power_close\r\n");
	gpio_set(L_POWER_GPIO,0); //关闭电源		
	hal_adc_ain_close();
}


void hal_adc_ain_init(void)
{
	gpio_config(VBAT_AIN_CTL, OUTPUT, PULL_NONE);
}

void hal_adc_ain_open(void)
{
	
	gpio_set(VBAT_AIN_CTL,1); //打开电源
	
}
void hal_adc_ain_close(void)
{
	gpio_set(VBAT_AIN_CTL,0); //打开电源

}


