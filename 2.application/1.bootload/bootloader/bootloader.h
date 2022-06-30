#include "stdint.h"
#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#define APP_ADDR 0x08002000

typedef void (*APP_FUNC)();
extern APP_FUNC jump2app;
void Jump2APP(uint32_t app_address);

#endif



