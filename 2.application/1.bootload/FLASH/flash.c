#include "flash.h"
#include "main.h"
#include "stdio.h"
#include <stdint.h>
#include <string.h>


uint8_t UPFlagBuffer[4] = {0xFF,0xFF,0xFF,0xFF};

uint8_t cBuffer[1024];

void WriteFlash(uint32_t address,uint8_t *pBuffer,uint32_t Numlengh)
{
	uint32_t i ,temp;
  HAL_FLASH_Unlock();

	for(i = 0; i < Numlengh;i+= 4)
	{
//		 temp = (uint32_t)pBuffer[i]<<24;
//		 temp |= (uint32_t)pBuffer[i+1]<<16;
//		 temp |= (uint32_t)pBuffer[i+2]<<8;
//		 temp |= (uint32_t)pBuffer[i+3];
		
		temp =  (uint32_t)pBuffer[i+3]<<24;
		temp |=  (uint32_t)pBuffer[i+2]<<16;
		temp |=  (uint32_t)pBuffer[i+1]<<8;
		temp |=  (uint32_t)pBuffer[i];
		
		
		
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address+i, temp);
	}
  HAL_FLASH_Lock();
	
}

uint8_t WriteFlashTest(uint32_t StartAddr,const uint8_t value[],unsigned long position,uint32_t lengh)
{
	
	uint32_t PageError = 0;
	uint32_t count ;
	static int coun1 = 0;
	uint32_t temp;
	
 	FLASH_EraseInitTypeDef *sFLASH_Erase;

	HAL_FLASH_Unlock();
	
	//以下下函数为  每1K就擦除flash
	if(position%1024 == 0)
	{
//		sFLASH_Erase->PageAddress = StartAddr+ADDR_FLASH_PAGE_k(position/1024);
	  sFLASH_Erase->NbPages = 1;
		HAL_FLASHEx_Erase(sFLASH_Erase, &PageError);
		coun1 = 0;
	}
for(count = 0;count <=256;count += 4)
	{
		 temp = (uint32_t)value[count]<<24;
		 temp |= (uint32_t)value[count+1]<<16;
		 temp |= (uint32_t)value[count+2]<<8;
		 temp |= (uint32_t)value[count+3];	
		  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,sFLASH_Erase->PageAddress+coun1*256+count, temp);
	}
	coun1++;
	HAL_FLASH_Lock();
}

int ReadFlash(uint32_t address,uint8_t *Nbuffer ,uint32_t  length)
{
	uint32_t temp = 0;
	uint32_t count = 0;
	while(count < length)
	{
		temp = *((volatile uint32_t *)address);
		*Nbuffer++ = (temp & 0xff);
		count++;
    if(count >= length)
      break;
	  *Nbuffer++ = (temp >> 8) & 0xff;
		count++;
		if(count >= length)
      break;
		*Nbuffer++ = (temp >> 16) & 0xff;
		count++;
		if(count >= length)
      break;
		*Nbuffer++ = (temp >> 24) & 0xff;
		count++;
		if(count >= length)
      break;
		address += 4;
	}
//	printf("add = %x , string = %s\r\n",address,Nbuffer);
}

uint32_t ReadFlashTest(uint32_t addr)
{
	uint32_t temp = *(__IO uint32_t*)(addr);
	printf("add = %x , value = %x\r\n",addr,temp);
	printf("test\r\n");
	return temp;
}
int EarseFlash_1K(uint32_t address)
{
	FLASH_EraseInitTypeDef f;
	uint32_t PageError;
	
	f.NbPages = 1;
	f.TypeErase = FLASH_TYPEERASE_PAGES;
	f.PageAddress = address;
	printf("test1\r\n");
  HAL_FLASH_Unlock();
  printf("test2\r\n");
	//检验输入的地址是不是1K的倍数
	if(address % 1024 == 0)
	{
		printf("test3\r\n");
		HAL_FLASHEx_Erase(&f, &PageError);
		printf("test5\r\n");
	}
	else
	{
		printf("test4\r\n");
		return 0;
	}
  HAL_FLASH_Lock();
	printf("test6\r\n");
	return 1;
}

void update_firmware(uint32_t SourceAddress,uint32_t TargetAddress)
{
//先读  后擦除  后写  
  uint16_t i;
	volatile uint32_t nK ;
	nK	= (TargetAddress - SourceAddress)/1024;
	for(i = 0;i < nK;i++)
	{
	memset(cBuffer,0xff,sizeof(cBuffer));
	ReadFlash(TargetAddress+i*0x0400,cBuffer,1024);
	
	EarseFlash_1K(SourceAddress+i*0x0400);
	
	WriteFlash(SourceAddress+i*0x0400,cBuffer,1024);	
	
	}

}

















