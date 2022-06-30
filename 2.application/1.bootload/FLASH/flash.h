#ifndef FLASH_H
#define FLASH_H
#include "stdint.h"

#define ADDR_FLASH_BASE 0x08000000
#define ADDR_FLASH_1K   0x0400

//´ýÓÅ»¯

//#define ADDR_FLASH_PAGE_k    (ADDR_FLASH_BASE+ADDR_FLASH_1K) /* Base @ of Page 0, 1 Kbyte */

#define APP1  0x2000
#define APP2  0x8C00
#define UPFLAG1 0xF800

//int APP1ADDR;//(ADDR_FLASH_BASE + APP1)    // 0x08002000
//int APP2ADDR; //(ADDR_FLASH_BASE + APP2)   // 0x08008C00
//int UPFLAG;   //(ADDR_FLASH_BASE + UPFLAG1)   // 0x0800F800
//int startflag; //0x08010004
extern uint8_t UPFlagBuffer[];



uint8_t WriteFlashTest(uint32_t StartAddr,const uint8_t value[],unsigned long position,uint32_t lengh);

void writeFlashTest(uint32_t address,uint32_t *pBuffer,uint32_t NumToWrite);

void WriteFlash(uint32_t address,uint8_t *pBuffer,uint32_t Numlengh);

int ReadFlash(uint32_t address,uint8_t *Nbuffer ,uint32_t  length);

void update_firmware(uint32_t SourceAddress,uint32_t TargetAddress);

uint32_t ReadFlashTest(uint32_t addr);

int EarseFlash_1K(uint32_t address);

#endif

