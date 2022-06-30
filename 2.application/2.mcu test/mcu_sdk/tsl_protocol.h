/**********************************Copyright (c)**********************************
**                       版权所有 (C), 2015-2022, EZVIZ
**
**                             http://www.iot.eziot.com
**
*********************************************************************************/

#ifndef __TSL_PROTOCOL_H_
#define __TSL_PROTOCOL_H_
/************测试代码************/
#include "stm32f10x.h"          //替换成自己单片机的头文件
#include "core_cm3.h"           //循环升级使用到SCB->VTOR引用这个文件
/*******************************/
#define DOMAIN_LEN_MAX 32
#define KEY_LEN_MAX 32 
#define VALUE_LEN_MAX 64
#define WIFI_RECVBUF_SIZE  256
#define WIFI_URCBUF_SIZE   128
#define TXBUF_SIZE 2048
#define RXBUF_SIZE 2048                           //支持OTA升级时要比1024大一些

#define PERIOD        "5"                         //配置配网超时时间
#define WINDOWSIZE    "1024"                        //配置Ymodem传输的窗口大小
#define MCUVERSION    "V1.0.0 build 220613"        //配置MCU固件版本号
#define NAME          "Lee"                        //配置产测路由器名称
#define PASSWORD      "12345678"                   //配置产测路由器密码

#define MCUOTA                                     //配置是否支持MCU OTA

#ifdef  MCUOTA
//#define ApplicationAddress 0x08008C00
#define PAGE_SIZE (0x400)                          /* 1 Kbytes *///定义单片机flash中页大小
#define FLASH_SIZE (0x10000)                       /* 64 KBytes */
#define OTA_FLASH_SIZE (FLASH_SIZE - 8 * 1024) / 2 //ota_flash_size-bootloader再除以二，因为有两个分区升级
//#define FLASH_IMAGE_SIZE (uint32_t)(FLASH_SIZE - (ApplicationAddress - 0x08000000))

#define DEBUG                                      //配置是否打印调试信息
#endif

	typedef enum {
		tsl_data_type_bool,
		tsl_data_type_int,
		tsl_data_type_number,
		tsl_data_type_string,
		tsl_data_type_array,
		tsl_data_type_object,
		tsl_data_type_null,
		tsl_data_type_max,
	} TSL_DATA_ATYPE_E;

typedef struct 
{
	char domain_identify[DOMAIN_LEN_MAX];
	char key_identify[KEY_LEN_MAX];
	TSL_DATA_ATYPE_E data_type;
	void (*func)(char *data, char *value);	
}AT_PROPERTY_CMD;

typedef struct 
{
	char domain_identify[DOMAIN_LEN_MAX];
	char key_identify[KEY_LEN_MAX];
	void (*func)(char *data, char *value);	
}AT_ACTION_CMD;

typedef struct 
{
	char domain_identify[DOMAIN_LEN_MAX];
	char key_identify[KEY_LEN_MAX];
	void (*func)(char *data, char *value);	
}AT_EVENT_CMD;

extern AT_PROPERTY_CMD property_cmd[];
extern AT_ACTION_CMD action_cmd[];
extern AT_EVENT_CMD event_cmd[];
#endif

