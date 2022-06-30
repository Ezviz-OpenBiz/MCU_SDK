/*
 * @Author: error: git config user.name && git config user.email & please set dead value or install git
 * @Date: 2022-04-01 13:58:03
 * @LastEditors: error: git config user.name && git config user.email & please set dead value or install git
 * @LastEditTime: 2022-06-27 20:26:50
 * @FilePath: \2.mcu test\USER\main.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "stm32f10x.h"
#include "usart1.h"
#include "usart3.h"
#include "key.h"
#include "led.h"
#include "Time_test.h"
#include "download.h"
#include "at_parse.h"
#include "flash.h"
#include "SysTick.h"
//#include "core_cm3.h"

volatile u32 time;
#define WIFI_RXBUF_SIZE 256
#define WIFI_TXBUF_SIZE 1024
extern ring_buf_t rbsend, rbrecv;
extern void SerialDownload(void);
extern void uart_send_data(unsigned char *in, unsigned short len);
unsigned char nbuff_AAAA[] = {0xAA, 0xAA, 0xAA, 0xAA};
int main(void)
{
	LED_GPIO_Config();
	USART1_Config();
	USART3_Config();
	//__enable_irq();
	EXTI_PE4_Config();
	EXTI_PE12_Config();
	NVIC_usart1_Configuration(); //命令通信串口，TX：PA9, RX：PA10
	NVIC_usart3_Configuration(); //信息打印串口，TX：PB10，RX:PB11
	TIM2_NVIC_Configuration();
	TIM2_Configuration();
	module_task_init(); /*模块初始化*/
	START_TIME;			//提供轮询任务的定时器
	if (SCB->VTOR == 0x08002000)
	{
		printf("this is app1\r\n");
	}
	else if (SCB->VTOR == 0x08008C00)
	{
		printf("this is app2\r\n");
	}
	while (1)
	{
		do_poll_handler();
	}
}
