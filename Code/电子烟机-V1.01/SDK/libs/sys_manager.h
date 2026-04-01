#ifndef _sys_manager_h_
#define _sys_manager_h_


#include  "stdint.h"

typedef enum
{
 ERR_NONE=1000,             //                    0          
 ERR_CLOSE_DOOR=1001,      //关门错误             1
 ERR_OPEN_DOOR=1002,      //开门错误              2 
 ERR_UNLOCK_LIMIT=1003,   //限位解锁错误          3 
 ERR_LOCK_LIMIT=1004,     //限位上锁错误          4  
 ERR_PW=1005,             //真实密码错误          5
 ERR_UNAUTHORIZED=1006,   //未授权操作            6
 ERR_OUTPUT_TIMEOUT=1007, //出货超时             7
 ERR_DIAL_IS_RUN=1008,    //正在运行请勿重复操作 8
 ERR_VOL=1009,            //欠压或者电压过高 不允许充电  9 
 ERR_NONENTITY=1010,  	  //货道不存在            6
 ERR_NAME_LEN =1011,  	  //名称长度错误          6
 ERR_BATTERY = 1012		  //电池电量疑似不足
}ERR_CODE;


typedef struct 
{
  
  volatile unsigned char  dial_number;        //货道编号  旋转机 1-5 格子机 1-10
  volatile unsigned long  tick;              //节拍 ms
 
  volatile unsigned int   charger_cnt;      //充电计数  秒  
  volatile unsigned char  charger_suspend;
  volatile unsigned char  charger_en;
  
  // Bool
  volatile unsigned char  ble_connect:1;             // 0 无连接       1 已连接
  volatile unsigned char  output_stat:1;           // 0 没有出货   1 正在出货
  volatile unsigned char  authorization:1;         // 0 没授权 1 已授权 
  volatile unsigned char  oad:1;         // 0 没授权 1 已授权
  // Bool
  volatile unsigned char private_key; //用来加密真实密码
  volatile ERR_CODE err_code;
 
	uint32_t  time_cnt;
	uint32_t  time;
	uint32_t  wake_time_1;
	uint32_t  wake_time_2;
	uint8_t   led_state;
	uint8_t  ble_msg_recv;
	char ble_buffer[64]; 
	 
   char ble_mac_address[31];
   char current_password[13];
	uint32_t ble_name_len;
	#pragma pack(1)
	struct
	{		
		uint8_t  name_flag;
		char 	   ble_name[17];
		uint8_t  led_time_flag;
		uint32_t led_start_time;
		uint32_t led_end_time;
	}config;
 	#pragma pack() 
	
}SysValue;

extern SysValue __sys_manager;

void bdaddr_env_init(void);

#endif
