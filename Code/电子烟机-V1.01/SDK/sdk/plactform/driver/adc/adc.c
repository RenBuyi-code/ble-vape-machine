#include <stdint.h>        // standard integer definition
#include <string.h>        // string manipulation
#include <stddef.h>     // standard definition
#include "BK3432_reg.h"
#include "uart.h"
#include "gpio.h"
#include "adc.h"

void Delay_ms(int num); //sync from svn revision 18
void Delay(int num); //sync from svn revision 18

void adc_init(uint8_t channel,uint8_t mode)
{
	uint32_t cfg;

	//enable adc clk
	REG_AHB0_ICU_ADCCLKCON &= ~(0x01 << 0);
	//adc div
	REG_AHB0_ICU_ADCCLKCON = (0x5 << 1);
		
	//set special as peripheral func
	gpio_config(0x30 + channel,FLOAT,PULL_NONE);

	//set adc mode/channel/wait clk
	cfg  = ( (mode << BIT_ADC_MODE ) | (channel << BIT_ADC_CHNL) | (0x01 << BIT_ADC_WAIT_CLK_SETTING));
	REG_APB7_ADC_CFG =  cfg;

	//set adc sample rate/pre div
	cfg |= ((18 << BIT_ADC_SAMPLE_RATE) | (3 << BIT_ADC_PRE_DIV)|(0x0 << BIT_ADC_DIV1_MODE));

	REG_APB7_ADC_CFG =  cfg;
	
	cfg |= (0x0 << BIT_ADC_FILTER);
	REG_APB7_ADC_CFG =  cfg;

	REG_APB7_ADC_CFG |= (0x01 << BIT_ADC_INT_CLEAR);
	//REG_APB7_ADC_CFG |= (0x01 << BIT_ADC_EN);//不能先使能ADC，不然ADC FIFO满时没有读出再次启动ADC就不会有中断

}

void adc_deinit(uint8_t channel)
{
    //gpio_config(0x30 + channel,INPUT,PULL_HIGH);
	
		gpio_config(0x30 + channel,OUTPUT,PULL_NONE);
		gpio_set(0x30 + channel,0);

    REG_APB7_ADC_CFG &= ~(SET_ADC_EN+(0x03 << BIT_ADC_MODE ));
    REG_AHB0_ICU_ADCCLKCON |= (0x01 << 0);
    REG_AHB0_ICU_INT_ENABLE &= ~(0x01 << 8); 
}


uint16_t g_adc_value,adc_flag;

void adc_isr(void)
{
    REG_APB7_ADC_CFG |= (0x01 << BIT_ADC_INT_CLEAR);
    adc_flag=1;	
}

void Delay_us(int num);
uint16_t adc_get_value(uint8_t channel)
{
  uint16_t adc_cnt,g_adc_value;
	uint32_t cfg;

   adc_cnt=0;
	adc_init(channel,1);	
	//set special as peripheral func
	gpio_config(GPIOD_0 + channel,FLOAT,PULL_NONE); 
	cfg = REG_APB7_ADC_CFG;
	cfg &= ~(7 << BIT_ADC_CHNL);
	cfg |= SET_ADC_EN+(0x01 << BIT_ADC_MODE )| (channel << BIT_ADC_CHNL);
	REG_APB7_ADC_CFG = cfg;
    //REG_APB7_ADC_CFG |= SET_ADC_EN+(0x01 << BIT_ADC_MODE )| (chanle << BIT_ADC_CHNL);
    
	 Delay(200); //need delay after set config
    while ( REG_APB7_ADC_CFG &(0x01 << BIT_ADC_BUSY) )  
    {
        adc_cnt++;       
        if(adc_cnt>350)
        {
            UART_PRINTF("g_adc_value_timeout\r\n");
            break;
        }
        Delay_us(10);
    } 
    if( 0==(REG_APB7_ADC_CFG &(0x01 << BIT_ADC_BUSY)))
    {
        g_adc_value=(REG_APB7_ADC_DAT);
        
    }
		
    UART_PRINTF("g_adc_value=%d\r\n",g_adc_value);
    REG_APB7_ADC_CFG &= ~(SET_ADC_EN+(0x03 << BIT_ADC_MODE ));
		
		//adc_deinit(channel);
    
		return g_adc_value;     
		
}


uint16_t adc_get_vol(uint8_t channel)
{

		uint16_t vbat_vol = 0;
		vbat_vol  = (( (adc_get_value(channel)*1000) /1023  )*1050)/1000;
		vbat_vol *= 6.5;
		vbat_vol  = 0.0000011556 * vbat_vol + 1.0854017010 *vbat_vol - 455.2749756; //拟合 110K 并 20K 范围6.5 - 2.2V
		return vbat_vol;
}


