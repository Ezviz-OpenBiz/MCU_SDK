#include "led.h"

void LED_GPIO_Config(void)
{
   GPIO_InitTypeDef LED;    
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);  
	 LED.GPIO_Pin=GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_8;      
	 LED.GPIO_Mode=GPIO_Mode_Out_PP;        
	 LED.GPIO_Speed=GPIO_Speed_50MHz;       
   GPIO_Init(GPIOE,&LED);                 
	 GPIO_SetBits(GPIOE,GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_8);                                           
}