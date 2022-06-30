#include "bootloader.h"
#include "main.h"
#include "stdio.h"

APP_FUNC jump2app;    //APP_FUNC����ָ��

void Jump2APP(uint32_t app_address)   //�û�������ʼ��ַ
{
	volatile uint32_t JumpAPPaddr;
	
	 if (((*(uint32_t*)app_address)&0x2FFE0000)==0x20000000)    //���ջ����ַ�Ƿ�Ϸ�
	 {
			JumpAPPaddr = *(volatile uint32_t*)(app_address + 4); //�û��������ڶ����ֽ�Ϊ����ʼ��ַ����λ��ַ��
			jump2app = (APP_FUNC) JumpAPPaddr;                    //������һ��϶�Ϊһ
			printf("APP��ת��ַ��%x\r\n",app_address);             
			__set_MSP(*(__IO uint32_t*) app_address);             //��ʼ��APP��ջ��ַ���û��������ĵ�һ����ַ���ڴ��ջ����ַ��
			jump2app();                                           //��ת��APP
	 }
	 else{
	 printf("�����ַ���Ϸ�\r\n");
	 }
	 	

}
