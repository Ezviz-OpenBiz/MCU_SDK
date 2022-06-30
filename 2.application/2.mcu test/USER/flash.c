#include "flash.h"
#include "tsl_protocol.h"
////PAGE 0-127     0x0800 0000 - 0x0803 FFFF
////PAGE 0-28     0x0800 9000-0x0800 FFFF
#define FLASH_PAGE_SIZE  		   		 	PAGE_SIZE //1k
uint8_t cBuffer[1024];
//void FlashWrite(uint16_t len,uint8_t  *data,uint32_t addr_start)
//{	
//	uint16_t i;
//	uint16_t temp;
//	
//	fmc_state_enum  fmc_state=FMC_READY;
//	
//	fmc_unlock();
//	
//	fmc_flag_clear(FMC_FLAG_BANK0_END);
//	fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
//	fmc_flag_clear(FMC_FLAG_BANK0_PGERR);

//	for(i=0;i<len/2;i++) 
//	{
//		fmc_state=fmc_halfword_program(addr_start, *(uint16_t*)data);
//		
//		if(fmc_state!=FMC_READY)
//		{
//			return;
//		}
//		
//		fmc_flag_clear(FMC_FLAG_BANK0_END);
//		fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
//		fmc_flag_clear(FMC_FLAG_BANK0_PGERR);
//		
//		data += 2;
//		addr_start += 2;
//	}
//	if(len % 2)
//	{
//		temp = *data | 0xff00;
//		fmc_state=fmc_halfword_program(addr_start,temp);
//	}

//	fmc_lock();
//}
void FlashWrite(uint16_t len,uint8_t  *data,uint32_t addr_start)
{
  uint16_t i;
	uint16_t temp;
	FLASH_Status FLASHStatus = FLASH_COMPLETE;
	//解锁
	FLASH_Unlock();
	//清除所有中断标志
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR); 
	//写数据
	for(i=0;i<len/2;i++)
	{
	  FLASHStatus=FLASH_ProgramHalfWord(addr_start, *(uint16_t*)data);
	  if(FLASHStatus!=FLASH_COMPLETE)return;
		FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    data += 2;
		addr_start += 2;		
	}
	if(len % 2)
	{
		temp = *data | 0xff00;
		FLASHStatus=FLASH_ProgramHalfWord(addr_start,temp);
	}
	//上锁
	FLASH_Lock();
}


//void FlashRead(uint16_t len,uint8_t *outdata,uint32_t addr_start)
//{	
//	uint32_t addr;
//	uint16_t i;
//	
//	addr = addr_start;
//	
//	for(i=0;i<len;i++) 
//	{
//		*outdata = *(uint8_t*) addr;
//		addr = addr + 1;
//		outdata++;
//	}
//}

uint32_t Readflash(uint32_t i)
{
	uint32_t data;
	uint32_t *p = (uint32_t*)(i);
	data = *p;
	printf("%x\r\n", data);	
	return data;
}


//void FlashErase(uint32_t start, uint32_t end)
//{
//	uint32_t EraseCounter;
//	
//	fmc_state_enum fmc_state=FMC_READY;

//	  /* unlock the flash program/erase controller */
//    fmc_unlock();

//    /* clear all pending flags */
//    fmc_flag_clear(FMC_FLAG_BANK0_END);
//    fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
//    fmc_flag_clear(FMC_FLAG_BANK0_PGERR);
//    
//    /* erase the flash pages */
//    while(start < end)
//	{
//		EraseCounter = start/FLASH_PAGE_SIZE;
//        fmc_state=fmc_page_erase(EraseCounter*FLASH_PAGE_SIZE);
//		if(fmc_state!=FMC_READY)
//		{
//			return;
//		}
//        fmc_flag_clear(FMC_FLAG_BANK0_END);
//        fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
//        fmc_flag_clear(FMC_FLAG_BANK0_PGERR);
//			 
//		start += FLASH_PAGE_SIZE;
//    }

//    /* lock the main FMC after the erase operation */
//    fmc_lock();
//}
void FlashErase(uint32_t start, uint32_t end)
{
	uint32_t EraseCounter;
	FLASH_Status FLASHStatus = FLASH_COMPLETE;
	
//解锁
	FLASH_Unlock();
//清除所有中断标志
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR); 
//擦除页
	while(start<end)
	{
	   EraseCounter = start/FLASH_PAGE_SIZE;
		 FLASHStatus = FLASH_ErasePage(EraseCounter*FLASH_PAGE_SIZE);
		 if(FLASHStatus!=FLASH_COMPLETE)return;
		 FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR); 
		 start+=FLASH_PAGE_SIZE;
	}
//上锁
	FLASH_Lock();
}

void Writeflash(uint32_t addr,uint32_t data)
{
	FLASH_Status FLASHStatus = FLASH_COMPLETE;
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR); 
    FLASHStatus = FLASH_ErasePage(addr);

	if(FLASHStatus == FLASH_COMPLETE)
	{
		FLASHStatus = FLASH_ProgramWord(addr, data);
		printf("has save flag\r\n");
	}
}

uint32_t ReadFlashTest(uint32_t addr)
{
	uint32_t temp = *(__IO uint32_t*)(addr);
	printf("add = %x , value = %x\r\n",addr,temp);
	printf("test\r\n");
	return temp;
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
//	   ReadFlash(TargetAddress+i*0x0400,cBuffer,1024);
//	
//	   EarseFlash_1K(SourceAddress+i*0x0400);
//	
//	   WriteFlash(SourceAddress+i*0x0400,cBuffer,1024);	
	}

}



