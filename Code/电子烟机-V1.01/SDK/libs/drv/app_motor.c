#include "app_motor.h"


#define MOTOR_DATA_IO  GPIO_P16


void Delay_us(int num);

uint8_t position_table[7]={
									 0xF0, //初始化位置
									 0xE1, //1号位
									 0xD2, //2号位		
									 0xC3, //3号位	
									 0xB4, //4号位	
									 0xA5, //5号位	
									 0x96, //6号位										 	
									};

									
void motor_send_msg(uint8_t number)
{	
	uart2_send(&position_table[number],1); //只需要一个字节就可以控制
}

void motor_rest(void)
{	
	uart2_send(&position_table[0],1); //只需要一个字节就可以控制
}

// 
uint8_t motor_receive(void)
{
	uint8_t data = 0;
	uint8_t i = 0;
	uint32_t cnt= 0;
	
	gpio_config(MOTOR_DATA_IO,INPUT,PULL_HIGH);
	
	while(gpio_get_input(MOTOR_DATA_IO) == 1)
	{
		Delay_us(1);	
		++cnt;
		if(cnt > 8000000)
		{
			break;
		}
	}//等待起始位
	
	if(cnt < 800000)
	{
		Delay_us(106);//等待起始信号过去
			
		for(i = 0 ; i<8 ; i++)
		{		
			data|=( gpio_get_input(MOTOR_DATA_IO)<<i );	
			Delay_us(106);
		}	
		
	}
	else
	{
		data =0XFF;
	}
	uart2_init(9600);	
	
	return data;		
}


