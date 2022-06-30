#ifndef __USART3_H
#define	__USART3_H

#include "stm32f10x.h"
#include <stdio.h>

void USART3_Config(void);
void NVIC_usart3_Configuration(void);
void NVIC_usart3_Configuration_Disable(void);
int fputc(int ch, FILE *f);

#endif 