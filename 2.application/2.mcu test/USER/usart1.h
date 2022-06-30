/*
 * @Author: error: git config user.name && git config user.email & please set dead value or install git
 * @Date: 2022-04-01 13:58:03
 * @LastEditors: error: git config user.name && git config user.email & please set dead value or install git
 * @LastEditTime: 2022-06-21 22:36:53
 * @FilePath: \2.mcu test\USER\usart1.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef __USART1_H
#define	__USART1_H
#include <stdio.h>
#include "stm32f10x.h"

void USART1_Config(void);
void NVIC_usart1_Configuration(void);
void NVIC_usart1_Configuration_Disable(void);
void UART1_SendByte(unsigned char data);
#endif 
