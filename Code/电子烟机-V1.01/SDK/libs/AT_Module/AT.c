#include "AT.h"
#include "uart.h"
#include "tools.h"
#include "password.h"
#include "app_fff0.h"
#include "drv_load.h"
#include "ke_timer.h"
#include "app_task.h"
#include "adc.h"
#include "drv_charger.h"
#include "icu.h"      // timer definition
#include "flash.h"
#include "app.h"
#include "wdt.h"
#include <stdlib.h>
#include <ctype.h>
#include "co_math.h"
#include "drv_battery.h"
#include "app_motor.h"


#define AT_TEST 1

static char AT_OOOK[] = "+OOOK:1\r\n\0";




void Delay_ms(int num);
void Delay_us(int num);


ERR_CODE at_outoput_cb(char* buffer)
{
#if AT_TEST > 0u
    UART_PRINTF("at_outoput_cb\r\n");
#endif
    if((__sys_manager.authorization ==1 || uart_rx_done == 1 ) && __sys_manager.output_stat == 0)
    {
      
            __sys_manager.dial_number = (buffer[3]-48);
		 
			    UART_PRINTF("__sys_manager.dial_number = %d\r\n",__sys_manager.dial_number);
            //货道从0开始 1-10
            if(__sys_manager.dial_number <= (MAX_LOAD_NUMBER-1) && __sys_manager.dial_number > 0)
            {
					   uart2_init(9600);	
						__sys_manager.output_stat = 1;
                  wdt_feed(0x3FFF);
					   motor_send_msg(__sys_manager.dial_number);						
						Delay_ms(1);	
						//gpio_config(GPIO_P16,INPUT,PULL_HIGH);
//					   while(1)
//						{
//							if(gpio_get_input(GPIO_P16) == 0)
//							{
//								break;
//							}				
//						}
						
						//uart2_init(9600);	
					   UART_PRINTF("motor finish \r\n");
						ke_timer_set(APP_LOAD_CLOSE_TIMER, TASK_APP,300);

            }
            else
            {
                return ERR_NONENTITY; //货道不存在
            }
	 }
    else if (__sys_manager.authorization  ==1 && __sys_manager.output_stat == 0) // 正在出货
    {
        return ERR_DIAL_IS_RUN;
    }
    else  // 没有授权
    {
        return ERR_UNAUTHORIZED;
    }
    return ERR_NONE;
}


ERR_CODE at_charger_cb(char* buffer)
{
    if(__sys_manager.authorization == 0)
    {
        return ERR_UNAUTHORIZED;
    }
    buffer[5] -= 48;
    if(buffer[5] > 8)buffer[5] = 8; //最多10个小时
    __sys_manager.charger_cnt = ((unsigned int)buffer[5])*3600; // 以小时为计数 软件定时器没那么准 给多几秒
    __sys_manager.charger_cnt+=60;

    __sys_manager.charger_en =1;//正在充电

    hal_charger_pw_open();

    UART_PRINTF("ISCHA = %ld",__sys_manager.charger_cnt);
    ke_timer_set(APP_CHAR_CLOSE_TIMER,TASK_APP,(__sys_manager.charger_cnt*SEC));

    UART_PRINTF("SEC = %ld",(__sys_manager.charger_cnt*SEC));

    app_fff1_send_lvl((uint8_t*)"ISCHA\r\n\0",strlen("ISCHA\r\n\0")); //发送
    __sys_manager.authorization = 0;

    return ERR_NONE;
}

ERR_CODE at_charger_cb_chb(char* buffer)
{
    uint16_t len = 0;
    char *p_str = NULL;
    uint32_t time = 0;

    if(__sys_manager.authorization == 0)
    {
        return ERR_UNAUTHORIZED;
    }

    len = strlen(buffer);
    len -=5;
    p_str = buffer+5;
    time = atoi(p_str);
    UART_PRINTF("len = %d\r\n",len);
    UART_PRINTF("buffer = %s\r\n",p_str);
    UART_PRINTF("time s = %d sec\r\n",time);

    hal_charger_pw_open();

    ke_timer_set(APP_CHAR_CLOSE_TIMER,TASK_APP,(time*SEC));
    app_fff1_send_lvl((uint8_t*)"ISCHA\r\n\0",strlen("ISCHA\r\n\0")); //发送
    __sys_manager.authorization = 0;

    return ERR_NONE;
}



ERR_CODE at_password_cb(char* buffer)
{

    static unsigned char temp  =0 ;

    temp = co_rand_byte();  //取最低字节
    temp ^=0x67;

    __sys_manager.private_key = temp;


    password_get((char*)&buffer[3],(char*)&__sys_manager.current_password);
    AT_OOOK[6] = temp;
#if AT_TEST > 0u
    UART_PRINTF("__sys_manager.private_key = %x\r\n",__sys_manager.private_key);
    UART_PRINTF("encryption current_password:");

    for(int i =0 ; i<12; i++)
    {

        UART_PRINTF("%X",__sys_manager.current_password[i]^(__sys_manager.private_key));

    }
    UART_PRINTF("\r\n");
#endif

#if AT_TEST > 0u
    UART_PRINTF("ture current_password:");
    for(int j =0 ; j<12; j++)
    {
        UART_PRINTF("%X",__sys_manager.current_password[j]);
    }
    UART_PRINTF("\r\n");
#endif

    app_fff1_send_lvl((uint8_t*)AT_OOOK,strlen(AT_OOOK));

    return ERR_NONE;
}


unsigned char err_cnt = 0;
unsigned char en_code[12]= {0,0,0,0,0,0,0,0,0,0,0};
unsigned char pass[12]= {0,0,0,0,0,0,0,0,0,0,0};

ERR_CODE at_oook_key_cb(char* buffer)
{

    uint8_t i =0;
    wdt_feed(0x3FFF);
    byte_cpy((unsigned char*)en_code,(unsigned char*)&buffer[6],12);



    for(i=0; i<12; i++) //解密
    {
        buffer[i+6] = (en_code[i]^ __sys_manager.private_key)^ 0x67;

    }

    if((strstr((char*)buffer,__sys_manager.current_password)!=0))//密码正确
    {
        __sys_manager.authorization = 1;

        app_fff1_send_lvl((uint8_t*)"IC\r\n\0",strlen("IC\r\n\0"));
        UART_PRINTF("IC\r\n");

    }
    else
    {
        return  ERR_PW;
    }

    return ERR_NONE;
}

ERR_CODE at_em_cb(char* buffer)
{



    return ERR_NONE;
}

ERR_CODE at_restart_cb(char* buffer)
{


    wdt_enable(10);
    while(1) {;} //等待重启


    return ERR_NONE;
}

ERR_CODE at_factory_cb(char* buffer)
{
    if(__sys_manager.authorization == 1)
    {

        return ERR_NONE;
    }
    else
    {
        return ERR_UNAUTHORIZED;
    }

}

uint32_t cut_buffer_decimal_number(char *dst,char *src,uint32_t src_len)  //截取10进制数字符
{
    int i =0;
    uint32_t len =0;

    for(i=0; i<src_len; i++)
    {
        if(isdigit(*src) && *src != NULL)
        {
            *dst = *src;
            UART_PRINTF("dst = %c\r\n",*dst);
            dst++;
            src++;
            len++;
        }
        else
        {
            src++;
        }

    }
    return len;
}

ERR_CODE at_set_mac_addres_cb(char* buffer)
{
    __sys_manager.ble_name_len = cut_buffer_decimal_number(__sys_manager.config.ble_name,buffer,18);
    if(__sys_manager.ble_name_len != 16)
    {
        uart_printf("Write failed\r\n");
        return ERR_NAME_LEN;
    }

    UART_PRINTF("\r\n__sys_manager.config.name_flag = %x\r\n",__sys_manager.config.name_flag);
    UART_PRINTF("uart_rx_done  = %d\r\n",uart_rx_done);
    UART_PRINTF("__sys_manager.authorization  = %d\r\n",__sys_manager.authorization);

    if( ( __sys_manager.config.name_flag == 0xff) || uart_rx_done == 1 || __sys_manager.authorization == 1)
    {

        UART_PRINTF("\r\nbe_name_len = %d\r\n",__sys_manager.ble_name_len);
        UART_PRINTF("__sys_manager.config.ble_name  = %s\r\n",__sys_manager.config.ble_name);

        Write_NVR_Flash(0x8080,(uint8_t*)&__sys_manager.config.ble_name,16);//写入名称
        Read_NVR_Flash(0x8080,(uint8_t*)__sys_manager.config.ble_name,16);//读数据
        UART_PRINTF("read __sys_manager.config.ble_name  =  %s\r\n",__sys_manager.config.ble_name);

        if(strcmp(__sys_manager.config.ble_name,buffer+2)==0) //写入成功
        {
            UART_PRINTF("wirte __sys_manager.config.name_flag \r\n");
            __sys_manager.config.name_flag =1;
            Write_NVR_Flash(0x80,&__sys_manager.config.name_flag,1);//存储配置标志位
            Read_NVR_Flash(0x80,&__sys_manager.config.name_flag,1);//读数据
            if(__sys_manager.config.name_flag == 1) //写入成功
            {
                UART_PRINTF("__sys_manager.config.name_flag  =  %d\r\n",__sys_manager.config.name_flag);
                app_fff1_send_lvl((uint8_t*)"SETOK\r\n\0",strlen("SETOK\r\n\0"));
                uart_printf("%s",buffer);//写入成功输出一下
                wdt_enable(10);
                while(1) {;} //等待重启
            }
            else
            {
                uart_printf("Write failed\r\n");
                app_fff1_send_lvl((uint8_t*)"Write failed\r\n\0",strlen("Write failed\r\n\0"));
            }
        }
        else
        {
            uart_printf("Write failed\r\n");
            app_fff1_send_lvl((uint8_t*)"Write failed\r\n\0",strlen("Write failed\r\n\0"));
        }
    }
    else
    {
        uart_printf("Write failed\r\n");
        app_fff1_send_lvl((uint8_t*)"Write failed\r\n\0",strlen("Write failed\r\n\0"));
    }

    return ERR_NONE;
}

ERR_CODE at_sleep_cb(char* buffer)
{
    appm_disconnect();

    return ERR_NONE;
}

ERR_CODE at_vol_get_cb(char* buffer)
{
    uint16_t vol = 0;
    uint8_t data[20];
    memset(data,20,0);
    uint8_t len = 0;

    vol = (( (adc_get_value(4)*1000) /1023  )*1050)/1000;
    vol*=6.5;

    vol =  0.0000011556 * vol + 1.0854017010 *vol - 455.2749756; //拟合 110K 并 20K 范围6.5 - 2.2V
    sprintf((char*)data,"V:%d\r\n",vol);
    len = strlen((char*)data);
    app_fff1_send_lvl(data,len);

    return ERR_NONE;
}


ERR_CODE at_open_all_cb(char* buffer)
{
	
    return ERR_NONE;
}

const char* ver_str = "VER:3.10\r\n\0";
ERR_CODE at_ver_cb(char* buffer)
{
 
    app_fff1_send_lvl((uint8_t*)ver_str,strlen(ver_str));

    return ERR_NONE;
}


ERR_CODE at_del_cfg(char* buffer)
{
 
	 __sys_manager.config.name_flag = 0XFF;
	 Write_NVR_Flash(0x80,&__sys_manager.config.name_flag,1);//存储配置标志位
	 wdt_enable(5);
    while(1) {;} //等待重启
	
    return ERR_NONE;
}


#include "drv_rgb.h"

ERR_CODE rgb_test(char* buffer)
{
	 uint8_t ch=0;
	 char rgb_string[6];
	
	 ch =buffer[4]-48;
	
	 memcpy(rgb_string,&buffer[5],8);	
	

	 drv_ws2812_set_color(ch,strtoul(rgb_string,0,0));	

    return ERR_NONE;
}


ERR_CODE at_st_cb(char* buffer)
{
	 uint16_t len = 0;
    char *p_str = NULL;
    uint32_t time = 0;

	 	
    __sys_manager.time = atoi(p_str+4);
	 UART_PRINTF("__sys_manager.time = %d\r\n",__sys_manager.time);
    return ERR_NONE;
}

ERR_CODE at_swt_cb(char* buffer)
{


    return ERR_NONE;
}

ERR_CODE at_set_cb(char* buffer)
{
		

    return ERR_NONE;
}

AT AT_Shell[SHELL_LEN]=
{
    {"+O:",at_outoput_cb},
    {"+P:",at_password_cb},
    {"+OOOK:",at_oook_key_cb},
    {"+RST",at_restart_cb},
    {"+Z",at_set_mac_addres_cb},
    {"+S?",at_sleep_cb},
    {"+ALL",at_open_all_cb},
	 {"+VER",at_ver_cb},
	 {"+DEL",at_del_cfg},
	 {"RGB:",rgb_test},
	 {"+ST:",at_st_cb},
	 {"+SST:",at_st_cb},
	 {"+SET:",at_st_cb}
	 
};

ERR_CODE at_traverse(char* buffer,AT at_shell[])
{
    unsigned char i = 0;
    ERR_CODE  err_code =  ERR_NONE;

    UART_PRINTF("at_traverse\r\n");
    //UART_PRINTF("----buffer = %s\r\n",buffer);
    //UART_PRINTF("----strlen = %d\r\n",strlen(buffer));

    for(i = 0 ; i < SHELL_LEN ; i++)
    {
#if AT_TEST>0
        UART_PRINTF("at_traverse i=%d\r\n",i);
#endif
        if(strstr(buffer,(char*)at_shell[i].AT_CMD) !=0 )
        {
#if AT_TEST>0
            UART_PRINTF("find at cmd : %s\r\n",at_shell[i].AT_CMD);
#endif
            err_code = at_shell[i].cmd_cb(buffer);
            break;
        }
    }

    return err_code;
}


void error_event_report(void)
{
    static char err_buffer[20];

    if( __sys_manager.err_code != ERR_NONE )
    {
        memset(err_buffer,0,20);
        sprintf(err_buffer,"ERR:%4d\r\n",__sys_manager.err_code);
        app_fff1_send_lvl((uint8_t*)err_buffer,strlen(err_buffer));
        __sys_manager.err_code = ERR_NONE;
        __sys_manager.output_stat = 0;

    }
}
