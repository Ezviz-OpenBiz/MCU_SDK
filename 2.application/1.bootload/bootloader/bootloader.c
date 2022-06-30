#include "bootloader.h"
#include "main.h"
#include "stdio.h"

APP_FUNC jump2app;    //APP_FUNC函数指针

void Jump2APP(uint32_t app_address)   //用户代码起始地址
{
	volatile uint32_t JumpAPPaddr;
	
	 if (((*(uint32_t*)app_address)&0x2FFE0000)==0x20000000)    //检查栈顶地址是否合法
	 {
			JumpAPPaddr = *(volatile uint32_t*)(app_address + 4); //用户代码区第二个字节为程序开始地址（复位地址）
			jump2app = (APP_FUNC) JumpAPPaddr;                    //和上面一句合二为一
			printf("APP跳转地址：%x\r\n",app_address);             
			__set_MSP(*(__IO uint32_t*) app_address);             //初始化APP堆栈地址（用户代码区的第一个地址用于存放栈顶地址）
			jump2app();                                           //跳转到APP
	 }
	 else{
	 printf("输入地址不合法\r\n");
	 }
	 	

}
