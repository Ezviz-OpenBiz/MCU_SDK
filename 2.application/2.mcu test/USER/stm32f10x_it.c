/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
//#include "at_chat.h"
//#include "module.h"
//#include "ringbuffer.h"
#include "mcu_at_system.h"
#include "at_parse.h"
#include "usart1.h"
#include "string.h"
#include "flash.h"
extern volatile u32 time,t;
extern void uart_receive_input(unsigned char Res);
extern  void distribution_network(void);
extern ring_buf_t rbsend, rbrecv;  
extern unsigned char rxbuf[256];         /*接收缓冲区 */
extern unsigned char txbuf[1024];         /*发送缓冲区 */
extern at_obj_t at;
extern void UART1_SendByte(unsigned char data);
extern void uart_send_data(unsigned char *in,unsigned short len);
/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
} 

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void TIM2_IRQHandler(void)
{
    systick_increase(10);
	if ( TIM_GetITStatus(TIM2 , TIM_IT_Update) != RESET ) 
	{	
		TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);    
//  		 time++;
//		{
//		  if(time%200==0)
//			{
//			  t++;
//				if(t==12)
//				{
//				t=0;
//				}
//			}
//		}
	}		 	
}

void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{

}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

//void USART1_IRQHandler(void)
//{ 
//	 unsigned char Res=0;
//	if((USART1->SR&USART_FLAG_RXNE) != 0)f
//    {
//        Res=USART1->DR;
//			  ring_buf_put(&rbrecv, &Res, 1);
//			
//        //uart_receive_input(Res);
//    }
////		else
////		{
////		
////		}
//	
//}
//void USART3_IRQHandler(void)
//{ 
//	 unsigned char Res=0;
//	if((USART3->SR&USART_FLAG_RXNE) != 0)
//    {
//        Res=USART3->DR;
//			  ring_buf_put(&rbrecv, &Res, 1);
//			  
//        //uart_receive_input(Res);
//    }
////		else
////		{
////		
////		}
//	
//}
//int i=0;
void EXTI4_IRQHandler(void)              //key3    PE4
{
	if(EXTI_GetITStatus(EXTI_Line4) != RESET) 
	{	
		  
		  Readflash(0x08008c00);
		  //Readflash(0x08000000+4*i);
		  //i++;
		  
		//WriteFlash(UPFLAGADDR,nbuff_5555,4);
//		NVIC_SystemReset();
//		if(i==1)
//		{
//		//at_send_singlline(&at, NULL, "AT+GPIO_TEST_ENSD=1\r\n");
//		uart_send_data((unsigned char *)"AT+ECTPP=Power,PowerSwitch,0,true\r\n",strlen("AT+ECTPP=Power,PowerSwitch,0,true\r\n"));	
//			i=0;
//		}
//		else
//		{
//		 //at_send_singlline(&at, NULL, "AT+GPIO_TEST_ENSD=2\r\n");
//		 uart_send_data((unsigned char *)"AT+ECTPP=Power,PowerSwitch,0,false\r\n",strlen("AT+ECTPP=Power,PowerSwitch,0,false\r\n"));	
//			i+=1;
		//}
				GPIO_WriteBit(GPIOE, GPIO_Pin_8, 
			(BitAction)((1-GPIO_ReadOutputDataBit(GPIOE, GPIO_Pin_8))));
		//printf("\n DEBUG in line (%d) and function (%s): the rbuf is (%s)\n",__LINE__,__func__,(char *)rbrecv.buf);
		//mcu_reset_wifi();
    //NVIC_SystemReset();
		//uart_send_data((unsigned char *)"AT+TMG?\r\n", strlen("AT+TMG?\r\n"));
		reset();
		//UART1_SendByte('b');
		EXTI_ClearITPendingBit(EXTI_Line4);     
	}  
//	if(EXTI_GetITStatus(EXTI_Line12) != RESET) 
//	{	
//		//mcu_get_system_time();
//		//mcu_start_connect_wifitest((unsigned char *)"VPN1",(unsigned char *)"19951227");
//		GPIO_WriteBit(GPIOE, GPIO_Pin_6, 
//			(BitAction)((1-GPIO_ReadOutputDataBit(GPIOE, GPIO_Pin_6))));
//		EXTI_ClearITPendingBit(EXTI_Line12);     
//	} 
	
}
void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line12) != RESET)
	{	
		GPIO_WriteBit(GPIOE, GPIO_Pin_6, 
			(BitAction)((1-GPIO_ReadOutputDataBit(GPIOE, GPIO_Pin_6))));
		//distribution_network();
		 product_test();
		//fn_event_RunModeComplate_sync();
		EXTI_ClearITPendingBit(EXTI_Line12);
  }		
}

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
