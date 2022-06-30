#ifndef _DOWNLOAD_H_
#define _DOWNLOAD_H_
#define PACKET_SEQNO_INDEX (1)
#define PACKET_SEQNO_COMP_INDEX (2)

#define PACKET_HEADER (3)
#define PACKET_TRAILER (2)
#define PACKET_OVERHEAD (PACKET_HEADER + PACKET_TRAILER)
#define PACKET_SIZE (128)
#define PACKET_1K_SIZE (1024)

#define FILE_NAME_LENGTH (256)
#define FILE_SIZE_LENGTH (16)

#define SOH (0x01)   //128
#define STX (0x02)   //1024
#define EOT (0x04)   
#define ACK (0x06)   
#define NAK (0x15)  
#define CA (0x18)   
#define CRC16 (0x43) //'C' == 0x43

#define ABORT1 (0x41) //'A' == 0x41
#define ABORT2 (0x61) //'a' == 0x61

#define NAK_TIMEOUT (0x100000)
#define MAX_ERRORS (5)
#include <stdio.h>
#include <string.h>
#include <stdint.h>
int32_t Ymodem_Receive(uint8_t *);

typedef void (*pFunction)(void);

#define CMD_STRING_SIZE 128

#define IS_AF(c) ((c >= 'A') && (c <= 'F'))
#define IS_af(c) ((c >= 'a') && (c <= 'f'))
#define IS_09(c) ((c >= '0') && (c <= '9'))
#define ISVALIDHEX(c) IS_AF(c) || IS_af(c) || IS_09(c)
#define ISVALIDDEC(c) IS_09(c)
#define CONVERTDEC(c) (c - '0')

#define CONVERTHEX_alpha(c) (IS_AF(c) ? (c - 'A' + 10) : (c - 'a' + 10))
#define CONVERTHEX(c) (IS_09(c) ? (c - '0') : CONVERTHEX_alpha(c))

uint32_t Str2Int(uint8_t *inputstr, int32_t *intnum);
void Int2Str(uint8_t *str, int32_t intnum);
uint32_t uart_isread_data(uint8_t *ch);
void SerialPutChar(uint8_t c);
void SerialDownload(void);

#endif /* _YMODEM_H_ */
