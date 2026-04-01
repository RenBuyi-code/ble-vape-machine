#include "gpio.h"
#include "rf.h"
#include "drv_rgb.h"
#include "uart.h"






uint8_t rgb_io_pin[3]= {GPIO_P02,GPIO_P03,GPIO_P10};




volatile unsigned long * rgb_io_register[3]= {
    &REG_APB5_GPIOA_DATA,
    &REG_APB5_GPIOA_DATA,
    &REG_APB5_GPIOB_DATA
};

void rgb_test_hi(uint8_t ch)
{
    uint32_t  io_reg_val_hi = 0;

    io_reg_val_hi  = gpio_set_value_get(rgb_io_pin[ch],1);
    *rgb_io_register[ch] = io_reg_val_hi;
    //gpio_set(GPIO_P02,1);
}


void rgb_test_low(uint8_t ch)
{
//	  uint32_t  io_reg_val_low = 0;
//
//     io_reg_val_low  = gpio_set_value_get(rgb_io_pin[ch],0);
//	  *rgb_io_register[ch] = io_reg_val_low;
    gpio_set(GPIO_P02,0);
}



void drv_ws2812_gpio_init(void)
{
    gpio_config(GPIO_P02,OUTPUT,PULL_NONE); //常亮
    gpio_config(GPIO_P03,OUTPUT,PULL_NONE); //中间
    gpio_config(GPIO_P10,OUTPUT,PULL_NONE); //外围
}


#include "ll.h"

void drv_ws2812_set_color(uint8_t ch, unsigned char Index, unsigned long RGB)
{
    unsigned long GRB = 0;
    unsigned char i = 0;
    unsigned char k=0;
    unsigned long Dat = 0;

    uint32_t  io_reg_val_hi = 0;
    uint32_t  io_reg_val_low = 0;
    uint8_t  index[24];
	
    GRB = (  ((RGB&0x00FF00)<<8) | ((RGB&0xff0000)>>8) | (RGB&0x0000ff) );
	 Dat = GRB;
    UART_PRINTF("RGB = %#lX\r\n",RGB);
    UART_PRINTF("GRB = %#lX\r\n",Dat);
	
	
    io_reg_val_hi  = gpio_set_value_get(rgb_io_pin[ch],1);
    io_reg_val_low = gpio_set_value_get(rgb_io_pin[ch],0);

    *(&REG_APB5_GPIOA_DATA) = io_reg_val_low;
    *(&REG_APB5_GPIOA_DATA) = io_reg_val_low;
    Delay_ms(1);

    for(k = 0; k < 24; k++)
    {
        if(Dat & 0x00800000) //发送1码
        {
            index[k]=1;
        }
        else //发送0码
        {
            index[k]=0;
        }
        Dat <<= 1;
    }

    GLOBAL_INT_STOP();
	 

		 for(k = 0; k < 24; k++)
		 {
			 switch(index[k])
			 {
				 case 1:
						
					  *(&REG_APB5_GPIOA_DATA) = io_reg_val_hi;
					  __nop();//190ns
					  *(&REG_APB5_GPIOA_DATA) = io_reg_val_low;
					  break;
				 case 0:
					  *(&REG_APB5_GPIOA_DATA) = io_reg_val_hi;	 
					  *(&REG_APB5_GPIOA_DATA) = io_reg_val_low;
						__nop();
				 
					  break;
			 }
		 }

    GLOBAL_INT_START();

}



