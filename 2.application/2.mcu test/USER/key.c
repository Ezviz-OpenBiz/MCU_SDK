#include "key.h"
//static void NVIC_Configuration1(void)
//{
//  NVIC_InitTypeDef NIS;
//  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
//  NIS.NVIC_IRQChannel = EXTI4_IRQn;
//  NIS.NVIC_IRQChannelPreemptionPriority = 0;
//  NIS.NVIC_IRQChannelSubPriority = 0;
//  NIS.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NIS);
//}

//void EXTI_PE4_Config(void)
//{
//	GPIO_InitTypeDef GPIO; 
//	EXTI_InitTypeDef EI;
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO,ENABLE);										
//	NVIC_Configuration1();
//  GPIO.GPIO_Pin = GPIO_Pin_4;       
//  GPIO.GPIO_Mode = GPIO_Mode_IPU;	 
//  GPIO_Init(GPIOE, &GPIO);
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource4); 
//  EI.EXTI_Line = EXTI_Line4;
//  EI.EXTI_Mode = EXTI_Mode_Interrupt;
//  EI.EXTI_Trigger = EXTI_Trigger_Falling; 
//  EI.EXTI_LineCmd = ENABLE;
//  EXTI_Init(&EI); 
//}


static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NIS;
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  NIS.NVIC_IRQChannel = EXTI4_IRQn;
  NIS.NVIC_IRQChannelPreemptionPriority = 0;
  NIS.NVIC_IRQChannelSubPriority = 0;
  NIS.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NIS);
}

void EXTI_PE4_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO,ENABLE);										
	NVIC_Configuration();
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;       
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 
  GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource4); 
  EXTI_InitStructure.EXTI_Line = EXTI_Line4;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure); 
}

static void NVIC_Configuration2(void)
{
  NVIC_InitTypeDef NIS;
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  NIS.NVIC_IRQChannel = EXTI15_10_IRQn;
  NIS.NVIC_IRQChannelPreemptionPriority = 0;
  NIS.NVIC_IRQChannelSubPriority = 0;
  NIS.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NIS);
}

void EXTI_PE12_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO,ENABLE);										
	NVIC_Configuration2();
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;       
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 
  GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource12); 
  EXTI_InitStructure.EXTI_Line = EXTI_Line12;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure); 
}