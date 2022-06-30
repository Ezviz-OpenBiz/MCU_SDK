/*******************************************************************************
 * Copyright © 2017-2022 Ezviz Inc.
 * @file:    tsl_protocol.c
 * @brief:   at指令携带的物模型协议解析以及发送
 *				1. 用户在此文件中实现控制台定义的所有物模型功能点设置，包括属性设置以及操作设置
 *				2. 此文件提供控制台定义的所有物模型功能点上报接口函数，包括属性上报以及事件上报，
 *				    用户在本地检测到状态变化或者事件发生时调用此API
 *          	3. 物模型定义的功能点数据类型以字符串传参，用户根据定义的类型进行转化后对mcu的各功能设置
 * Change Logs:
 * Date           Author       Notes
 * 2022           Lee&Young    此为自动生成代码,如在开发平台有相关修改请重新下载MCU_SDK
 *******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tsl_protocol.h"
#include "mcu_at_system.h"
/************测试示例代码************/
#include "led.h"       
char *Switch = "true"; 
int Temperature = 34;  
/*************************************/

void fn_property_Switch_sync()
{
	char szIdentifier[32] = "Switch";
	char data[32];
	/***************测试示例代码***************/
	sprintf(data, "%s", Switch);          //设备此刻Switch赋值给data
	/*************************************/
	property_up_handler_entry(szIdentifier, data);
}

void fn_property_Temperature_sync()
{
	char szIdentifier[32] = "Temperature";
	char data[32];
	/***************测试示例代码***************/
	sprintf(data, "%d", Temperature);     //设备此刻Temperature赋值给data
	/*************************************/
	property_up_handler_entry(szIdentifier, data);
}

void fn_event_RunModeComplate_sync()
{
	char szIdentifier[32] = "RunModeComplate";
	char data[32];
	/***************测试示例代码***************/
	sprintf(data, "%s", "{}"); //设备此刻Temperature赋值给data
	/*************************************/
	event_up_handler_entry(szIdentifier, data);
}

static void fn_property_Switch(char *data, char *value)
{
	if (strcmp(value, "true") == 0)
	{
		/***************测试示例代码***************/
		GPIO_SetBits(GPIOE, GPIO_Pin_6);      //to do 填入设备要执行的语句
		Switch = "true";
		/*************************************/
	}
	else if (strcmp(value, "false") == 0)
	{	
		/***************测试示例代码***************/
		GPIO_ResetBits(GPIOE, GPIO_Pin_6);    //to do 填入设备要执行的语句
		Switch = "false";
		/*************************************/
	}
	fn_property_Switch_sync();
}

static void fn_property_Temperature(char *data, char *value)
{
	if (value != NULL)
	{
		/***************测试示例代码***************/
		Temperature = atoi(value);			  //to do 填入设备要执行的语句
		Temperature = Temperature + 1;        //to do 填入设备要执行的语句
		/*************************************/
	}
	fn_property_Temperature_sync();
}

static void fn_action_SystemReset(char *data, char *value)
{
	if (strcmp(value, "true") == 0)
	{
		/***************测试示例代码***************/
		GPIO_SetBits(GPIOE, GPIO_Pin_6);      //to do 填入设备要执行的语句
		/*************************************/
	}
	else if (strcmp(value, "false") == 0)
	{
		/***************测试示例代码***************/
		GPIO_ResetBits(GPIOE, GPIO_Pin_6);    //to do 填入设备要执行的语句
		/*************************************/
	}
}

AT_PROPERTY_CMD property_cmd[] =
{
	{"LightControl", "Switch", tsl_data_type_bool, fn_property_Switch},
	{"Humiture", "Temperature", tsl_data_type_number, fn_property_Temperature},
	{NULL, NULL, 0, NULL}
};

AT_ACTION_CMD action_cmd[] =
{
	{"SystemMaintenance", "SystemReset", fn_action_SystemReset},
	{NULL, NULL, NULL}
};

AT_EVENT_CMD event_cmd[] =
{
	{"PetHouseMode", "RunModeComplate", NULL},
	{NULL, NULL, NULL}
};