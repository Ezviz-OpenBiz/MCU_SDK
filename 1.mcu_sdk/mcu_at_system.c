#include <string.h>
#include "mcu_at_system.h"
#include "tsl_protocol.h"
#include "at_parse.h"
#include "download.h"
/************测试示例代码************/
#include "usart1.h"
#include "usart3.h"
/*************************************/

void one_send_byte(unsigned char value)
{
    /***************测试示例代码***************/
    UART1_SendByte(value); //发送一个字节
    /*****************************************/
}

/******************6.发送函数*********************/
void uart_send_data(unsigned char *in, unsigned short len)
{
    if ((NULL == in) || (0 == len))
    {
        return;
    }
    while (len--)
    {
        one_send_byte(*in);
        in++;
    }
}
/******************发送函数*********************/

/******************4.发送基础命令函数*********************/
void distribution_network(void)
{
    char str[16];
    sprintf(str, "%s%s%s", "AT+WDST=0,", PERIOD, "\r\n");
    uart_send_data((unsigned char *)str, strlen(str));
}
void reset(void)
{
    uart_send_data((unsigned char *)"ATZ\r\n", strlen("ATZ\r\n"));
}
/******************发送基础命令函数*********************/

/******************3.处理下发命令入口函数*********************/
void property_get_handler_entry(char *identifier)
{
    int i, n;
    for (i = 0; property_cmd[i].key_identify != NULL; i++)
    {
        n = strlen(identifier);
        if (strncmp(identifier, property_cmd[i].key_identify, n) == 0)
        {
            property_cmd[i].func(identifier, NULL);
            break;
        }
    }
    if (property_cmd[i].key_identify == NULL)
    {
#ifdef DEBUG
        printf("no find this key");
#endif
    }
}

void property_set_handler_entry(char *identifier, char *value)
{
    uart_send_data((unsigned char *)"AT+ECTPS\r\n", strlen("AT+ECTPS\r\n"));
    int i, n;
    for (i = 0; property_cmd[i].key_identify != NULL; i++)
    {
        n = strlen(identifier);
        if (strncmp(identifier, property_cmd[i].key_identify, n) == 0)
        {
            property_cmd[i].func(identifier, value);
            break;
        }
    }
    if (property_cmd[i].key_identify == NULL)
    {
#ifdef DEBUG
        printf("no find this key");
#endif
    }
}

void action_set_handler_entry(char *identifier, char *value)
{
    uart_send_data((unsigned char *)"AT+ECTAS\r\n", strlen("AT+ECTAS\r\n"));
    int i, n;
    for (i = 0; action_cmd[i].key_identify != NULL; i++)
    {
        n = strlen(identifier);
        if (strncmp(identifier, action_cmd[i].key_identify, n) == 0)
        {
            action_cmd[i].func(identifier, value);
            break;
        }
    }
    if (property_cmd[i].key_identify == NULL)
    {
#ifdef DEBUG
        printf("no find this key");
#endif
    }
}
/******************处理下发命令入口函数*********************/

/******************5.处理上报命令入口函数*********************/
void property_up_handler_entry(char *identifier, char *value)
{
    int i, n;
    char szSendValue[9 + DOMAIN_LEN_MAX + 1 + KEY_LEN_MAX + 3 + VALUE_LEN_MAX + 1] = {0};

    for (i = 0; property_cmd[i].key_identify != NULL; i++)
    {
        n = strlen(identifier);
        if (strncmp(identifier, property_cmd[i].key_identify, n) == 0)
        {
            break;
        }
    }

    sprintf(szSendValue, "%s%s,%s,%d,%s%s", "AT+ECTPP=",
            property_cmd[i].domain_identify,
            property_cmd[i].key_identify,
            property_cmd[i].data_type,
            (char *)value,
            "\r\n");

    uart_send_data((unsigned char *)szSendValue, strlen(szSendValue));
}

void event_up_handler_entry(char *identifier, char *value)
{
    int i, n;
    char szSendValue[9 + DOMAIN_LEN_MAX + 1 + KEY_LEN_MAX + 3 + VALUE_LEN_MAX + 132 + 32 + 32 + 32] = {0};

    for (i = 0; event_cmd[i].key_identify != NULL; i++)
    {
        n = strlen(identifier);
        if (strncmp(identifier, event_cmd[i].key_identify, n) == 0)
        {
            break;
        }
    }

    sprintf(szSendValue, "%s%s,%s,%s%s", "AT+ECTEP=",
            event_cmd[i].domain_identify,
            event_cmd[i].key_identify,
            "{}",
            "\r\n");

    uart_send_data((unsigned char *)szSendValue, strlen(szSendValue));
}
/******************处理上报命令入口函数*********************/

/******************2.通过逗号位置分隔参数函数*********************/
/**
 * @description: 查找AT命令等号后第几个逗号离字符串开始指针的距离
 * @param {char} *args字符串开始的指针
 * @param {int} posAT命令等号后第几个逗号
 * @return {*}
 */
static int get_comma_pos(const char *args, int pos)
{
    int ret = -1;
    int comma_num = 0;
    int brace_num = 0;
    int sq_num = 0;
    int i = 0;
    int len = strlen(args);
    int tail_len = strlen("\0");
    while ('\0' != args[i] && i <= len - tail_len)
    {
        if (',' == args[i] && brace_num == 0 && sq_num == 0)
        {
            comma_num++;
        }
        if ('{' == args[i])
        {
            brace_num++;
        }
        if ('}' == args[i])
        {
            brace_num--;
        }
        if ('[' == args[i])
        {
            sq_num++;
        }
        if (']' == args[i])
        {
            sq_num--;
        }
        ret = i;
        if (pos == comma_num)
        {
            break;
        }

        i++;
    }
    return ret;
}

/**
 * @description: 
 * @param {char} *value返回查找的AT命令参数值
 * @param {char} *args字符串开始的指针
 * @param {int} value_posAT命令等号后第几个参数
 * @return {*}
 */
static int get_uart_value(char *value, const char *args, int value_pos)
{
    int ret = -1;
    do
    {
        int start_pos = -1;
        int end_pos = strlen(args);
        start_pos = get_comma_pos(args, value_pos - 1);
        end_pos = get_comma_pos(args, value_pos);

        if (-1 != start_pos && start_pos < end_pos)
        {
            memcpy(value, args + start_pos + 1, end_pos - start_pos - 1);
        }
        else
        {
#ifdef DEBUG
        printf("value parse failed\r\n");
#endif					
            break;
        }
        ret = 0;
    } while (0);

    return ret;
}
/******************通过逗号位置分隔参数函数*********************/

/******************1.URC命令调用函数*********************/
void fn_urc_property_get(char *recvbuf, int size)
{
    char *cur_urc_args = NULL;
    int argc = 0;
    cur_urc_args = recvbuf + strlen("URC+ECTPG");
    char identifier[32] = {0};
    char value[32] = {0};

    if (0 == get_uart_value(identifier, cur_urc_args, 2))
    {
#ifdef DEBUG
        printf("prop key: %s", value);
#endif
    }

    property_get_handler_entry(identifier);
}

void fn_urc_property_set(char *recvbuf, int size)
{
    char *cur_urc_args = NULL;
    int argc = 0;
    cur_urc_args = recvbuf + strlen("URC+ECTPS");
    char identifier[32] = {0};
    char value[32] = {0};

    if (0 == get_uart_value(identifier, cur_urc_args, 2))    //第2个参数是key的identifier
    {
#ifdef DEBUG
        printf("prop key: %s", identifier);
#endif
    }

    if (0 == get_uart_value(value, cur_urc_args, 4))
    {
#ifdef DEBUG
        printf("prop value: %s", value);
#endif
    }

    property_set_handler_entry(identifier, value);
}

void fn_urc_action_set(char *recvbuf, int size)
{
    char *cur_urc_args = NULL;
    int argc = 0;
    cur_urc_args = recvbuf + strlen("URC+ECTAS");
    char identifier[32] = {0};
    char value[32] = {0};

    if (0 == get_uart_value(identifier, cur_urc_args, 2)) //第2个参数是key的identifier
    {
#ifdef DEBUG
        printf("action key: %s", identifier);
#endif
    }

    if (0 == get_uart_value(value, cur_urc_args, 3))
    {
#ifdef DEBUG
        printf("action value : %s", value);
#endif
    }

    action_set_handler_entry(identifier, value);
}

void fn_urc_otast(char *recvbuf, int size)
{
    char str[16];
    sprintf(str, "%s%s%s", "AT+OTAST=", WINDOWSIZE, "\r\n");
    uart_send_data((unsigned char *)str, strlen(str));
}

void fn_otast(char *recvbuf, int size)
{
    set_flag(1);
}

void fn_urc_ecstac(char *recvbuf, int size)
{
    char *cur_urc_args = NULL;
    cur_urc_args = recvbuf + strlen("URC+ECSTAC");
    if (0 == strncmp(cur_urc_args + 1, "cloud", strlen("cloud")))
    {
        char str[32];
        sprintf(str, "%s%s%s", "AT+OTAI=", MCUVERSION, ",0\r\n");
        uart_send_data((unsigned char *)str, strlen(str));
    }
}

void fn_urc_otaac(char *recvbuf, int size)
{
    uart_send_data((unsigned char *)"AT+OTAAC=0\r\n", strlen("AT+OTAAC=0\r\n"));
    set_flag(3);
}

void fn_urc_tmc(char *recvbuf, int size)
{
    uart_send_data((unsigned char *)"AT+TMG?\r\n", strlen("AT+TMG?\r\n"));
}

void fn_tmg(char *recvbuf, int size)
{
	int year,mon,day,hour,min,sec;
	char timezone;
	sscanf(recvbuf,"+TMG:%04d-%02d-%02dT%02d:%02d:%02d",&year,&mon,&day,&hour,&min,&sec);
	#ifdef DEBUG
	printf("year is %04d\r\n",year);
	printf("mon is %02d\r\n",mon);
	printf("day is %02d\r\n",day);
	printf("hour is %02d\r\n",hour);
	printf("min is %02d\r\n",min);
	printf("sec is %02d\r\n",sec);
	#endif
}

void product_test(void)
{   
	  char str[48];
	  #ifdef DEBUG
	  printf("product test frist step\r\n");
	  #endif
	  sprintf(str, "%s%s%s%s%s", "AT+PTCONNECT=", NAME, ",", PASSWORD, "\r\n");
    uart_send_data((unsigned char *)str, strlen(str));
}

void fn_ptconnect(char *recvbuf, int size)
{
	  char str[32];
	  #ifdef DEBUG
	  printf("product test second step\r\n");
	  #endif
	  sprintf(str, "%s%s%s", "AT+PTGETRSSI=", NAME, "\r\n");
    uart_send_data((unsigned char *)str, strlen(str));
}

void fn_ptgetrssi(char *recvbuf, int size)
{
    int rssi;
	  sscanf(recvbuf,"+PTGETRSSI:%d",&rssi);
	  #ifdef DEBUG
	  printf("product test succ, the rssi is %d\r\n", rssi);
	  #endif
}
/******************URC命令调用函数*********************/
