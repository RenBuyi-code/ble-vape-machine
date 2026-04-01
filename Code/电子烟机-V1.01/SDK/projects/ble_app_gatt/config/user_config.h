/**
 *******************************************************************************
 *
 * @file user_config.h
 *
 * @brief Application configuration definition
 *
 * Copyright (C) RivieraWaves 2009-2016
 *
 *******************************************************************************
 */
 
#ifndef _USER_CONFIG_H_
#define _USER_CONFIG_H_


 
 /******************************************************************************
  *############################################################################*
  * 							SYSTEM MACRO CTRL                              *
  *############################################################################*
  *****************************************************************************/

//如果需要使用GPIO进行调试，需要打开这个宏
#define GPIO_DBG_MSG			  0
//UART使能控制宏
#define UART_PRINTF_EN			1//不使用这个打印 不输出debug 信息
//蓝牙硬件调试控制
#define DEBUG_HW					0




 
/*******************************************************************************
 *#############################################################################*
 *								APPLICATION MACRO CTRL                         *
 *#############################################################################*
 *******************************************************************************/
 
//连接参数更新控制
#define UPDATE_CONNENCT_PARAM  			1

//最小连接间隔
#define BLE_UAPDATA_MIN_INTVALUE			20
//最大连接间隔 
#define BLE_UAPDATA_MAX_INTVALUE			40
//连接Latency
#define BLE_UAPDATA_LATENCY				0
//连接超时
#define BLE_UAPDATA_TIMEOUT				1000
//

//设备名称
#define APP_DFLT_DEVICE_NAME           ("KZ-G45-OAD")


 //广播包UUID配置
#define APP_FFF0_ADV_DATA_UUID        "\x03\x03\xE0\xFF"
#define APP_FFF0_ADV_DATA_UUID_LEN    (4)`

#define APP_SCNRSP_DATA 			"\x0B\x08\x4b\x5a\x2d\x47\x34\x35\x2d\x4f\x41\x44"
#define APP_SCNRSP_DATA_LEN     (12)


//广播参数配置
/// Advertising channel map - 37, 38, 39
#define APP_ADV_CHMAP           (0x07)
/// Advertising minimum interval - 1000ms (1600*0.625ms)
#define APP_ADV_INT_MIN         (1600)
/// Advertising maximum interval - 1000ms (1600*0.625ms)
#define APP_ADV_INT_MAX         (1600)
/// Fast advertising interval
#define APP_ADV_FAST_INT        (32)

#define MAX_DAY_WAKE_MAX        (86400/(800*0.625))



/*******************************************************************************
 *#############################################################################*
 *								DRIVER MACRO CTRL                              *
 *#############################################################################*
 ******************************************************************************/

//DRIVER CONFIG
#define UART_DRIVER				   1
#define UART2_DRIVER  				1
#define GPIO_DRIVER				   1
#define AUDIO_DRIVER					0
#define RTC_DRIVER					1
#define ADC_DRIVER					1
#define I2C_DRIVER					0
#define PWM_DRIVER					0

#endif /* _USER_CONFIG_H_ */
