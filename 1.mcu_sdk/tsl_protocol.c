/*******************************************************************************
 * Copyright  2017-2022 Ezviz Inc.
 * @file:    tsl_protocol.c * @brief:   atָ��Я������ģ��Э������Լ�����
 *				1. �û��ڴ��ļ���ʵ�ֿ���̨�����������ģ�͹��ܵ����ã��������������Լ���������
 *				2. ���ļ��ṩ����̨�����������ģ�͹��ܵ��ϱ��ӿں��������������ϱ��Լ��¼��ϱ���
 *				    �û��ڱ��ؼ�⵽״̬�仯�����¼�����ʱ���ô�API
 *          	3. ��ģ�Ͷ���Ĺ��ܵ������������ַ������Σ��û����ݶ�������ͽ���ת�����mcu�ĸ���������
 * Change Logs:
 * Date           Author       Notes
 * 2022           Lee&Young    ��Ϊ�Զ����ɴ���,���ڿ���ƽ̨������޸�����������MCU_SDK
 *******************************************************************************/
#include "at_protocol_tsl.h"
#include <stdio.h>
#include "mcu_at_system.h"
#include "string.h"
#include "stdlib.h"
//�û��ɵ�����Щ���������ϱ�//
void fn_property_Switch_sync ()
{
    char szIdentifier[32]="Switch";
    char data[32];

    property_up_handler_entry(szIdentifier,data);
}
void fn_property_Temperature_sync ()
{
    char szIdentifier[32]="Temperature";
    char data[32];

    property_up_handler_entry(szIdentifier,data);
}

void fn_event_RunModeComplate_sync (char *data)
{
    char szIdentifier[32]="RunModeComplate";
    char data[32];

    event_up_handler_entry(szIdentifier,data);
}

//�û�������Щ���������豸ִ��//
static void fn_property_Switch(char *data,char *value)
{


    fn_property_Switch_sync();
}
static void fn_property_Temperature(char *data,char *value)
{


    fn_property_Temperature_sync();
}

static void fn_action_SystemReset(char *data,char *value)
{



}

//���ܵ����������ҹ��ܵ�//
AT_PROPERITY_CMD property_cmd[]=
{
    {"LightControl","Switch",tsl_data_type_bool,fn_property_Switch},
    {"Humiture","Temperature",tsl_data_type_number,fn_property_Temperature},
    {NULL,NULL,NULL}
};
AT_ACTION_CMD action_cmd[]=
{
    {"SystemMaintenance","SystemReset",fn_action_SystemReset},
    {NULL,NULL,NULL}
};
AT_EVENT_CMD event_cmd[]=
{
    {"PetHouseMode","RunModeComplate",fn_event_RunModeComplate},
    {NULL,NULL,NULL}
};