#ifndef __FLASH_H__
#define __FLASH_H__

#include "stm32f10x.h"

void FlashWrite(uint16_t len,uint8_t  *data,uint32_t addr_start);
//void FlashRead(uint16_t len,uint8_t *outdata,uint32_t addr_start);
uint32_t Readflash(uint32_t i);
void FlashErase(uint32_t start, uint32_t end);
void Writeflash(uint32_t addr,uint32_t data);
uint32_t ReadFlashTest(uint32_t addr);

#define ADDR_FLASH_BASE 0x08000000
#define ADDR_FLASH_1K   0x0400

//´ýÓÅ»¯

//#define ADDR_FLASH_PAGE_k    (ADDR_FLASH_BASE+ADDR_FLASH_1K) /* Base @ of Page 0, 1 Kbyte */

#define APP1  0x2000
#define APP2  0x9000
#define UPFLAG1 0x1c00

#define APP1ADDR (ADDR_FLASH_BASE + APP1)    // 0x08002000
#define APP2ADDR (ADDR_FLASH_BASE + APP2)   // 0x08009000
#define UPFLAG   (ADDR_FLASH_BASE + UPFLAG1)   // 0x08001c00

#endif



