#include "download.h"
#include "tsl_protocol.h"
#include "mcu_at_system.h"
/************测试示例代码************/
#include "flash.h"
#include "usart1.h"
/*************************************/
static uint8_t tab_1024[1024] = {0};
static uint8_t file_name[FILE_NAME_LENGTH];
static uint32_t FlashDestination;

/**
  * @brief  Convert an Integer to a string
  * @param  str: The string
  * @param  intnum: The intger to be converted
  * @retval None
  */
void Int2Str(uint8_t *str, int32_t intnum)
{
    uint32_t i, Div = 1000000000, j = 0, Status = 0;

    for (i = 0; i < 10; i++)
    {
        str[j++] = (intnum / Div) + 48;

        intnum = intnum % Div;
        Div /= 10;
        if ((str[j - 1] == '0') & (Status == 0))
        {
            j = 0;
        }
        else
        {
            Status++;
        }
    }
}

/**
  * @brief  Convert a string to an integer
  * @param  inputstr: The string to be converted
  * @param  intnum: The intger value
  * @retval 1: Correct
  *         0: Error
  */
uint32_t Str2Int(uint8_t *inputstr, int32_t *intnum)
{
    uint32_t i = 0, res = 0;
    uint32_t val = 0;

    if (inputstr[0] == '0' && (inputstr[1] == 'x' || inputstr[1] == 'X'))
    {
        if (inputstr[2] == '\0')
        {
            return 0;
        }
        for (i = 2; i < 11; i++)
        {
            if (inputstr[i] == '\0')
            {
                *intnum = val;
                /* return 1; */
                res = 1;
                break;
            }
            if (ISVALIDHEX(inputstr[i]))
            {
                val = (val << 4) + CONVERTHEX(inputstr[i]);
            }
            else
            {
                /* return 0, Invalid input */
                res = 0;
                break;
            }
        }
        /* over 8 digit hex --invalid */
        if (i >= 11)
        {
            res = 0;
        }
    }
    else /* max 10-digit decimal input */
    {
        for (i = 0; i < 11; i++)
        {
            if (inputstr[i] == '\0')
            {
                *intnum = val;
                /* return 1 */
                res = 1;
                break;
            }
            else if ((inputstr[i] == 'k' || inputstr[i] == 'K') && (i > 0))
            {
                val = val << 10;
                *intnum = val;
                res = 1;
                break;
            }
            else if ((inputstr[i] == 'm' || inputstr[i] == 'M') && (i > 0))
            {
                val = val << 20;
                *intnum = val;
                res = 1;
                break;
            }
            else if (ISVALIDDEC(inputstr[i]))
            {
                val = val * 10 + CONVERTDEC(inputstr[i]);
            }
            else
            {
                /* return 0, Invalid input */
                res = 0;
                break;
            }
        }
        /* Over 10 digit decimal --invalid */
        if (i >= 11)
        {
            res = 0;
        }
    }

    return res;
}

static int32_t Receive_Byte(uint8_t *ch, uint32_t timeout)
{
    while (timeout-- > 0)
    {
        if (uart_isread_data(ch) == 1)
        {
            return 0;
        }
    }
    return 0;
}

static uint32_t Send_Byte(uint8_t c)
{
    SerialPutChar(c);
    return 0;
}

/**
* @brief  Receive a packet from sender
* @param  data
* @param  length
* @param  timeout
*     0: end of transmission
*    -1: abort by sender
*    >0: packet length
* @retval 0: normally return
*        -1: timeout or packet error
*         1: abort by user
*/
static int32_t Receive_Packet(uint8_t *data, int32_t *length, uint32_t timeout)
{
    uint16_t i, packet_size;
    uint8_t ch;
    *length = 0;
    if (Receive_Byte(&ch, timeout) != 0)
    {
        return -1;
    }
    switch (ch)
    {
    case SOH:
        packet_size = PACKET_SIZE;
        break;
    case STX:
        packet_size = PACKET_1K_SIZE;
        break;
    case EOT:
        *length = 0;
        return 0;
    case CA:
        if ((Receive_Byte(&ch, timeout) == 0) && (ch == CA))
        {
            *length = -1;
            return 0;
        }
        else
        {
            return -1;
        }
    case ABORT1:
    case ABORT2:
        return 1;                                //ABORT没啥用可以直接return 2一直发送'C'
    default:
        return -1;
    }
    *data = ch;
    for (i = 1; i < (packet_size + PACKET_OVERHEAD); i++)
    {
        if (Receive_Byte(data + i, timeout) != 0)
        {
            return -1;
        }
    }
    if (data[PACKET_SEQNO_INDEX] != ((data[PACKET_SEQNO_COMP_INDEX] ^ 0xff) & 0xff))
    {
        return -1;
    }
    *length = packet_size;
    return 0;
}

void Write2Flash(uint16_t len, uint8_t *data, uint32_t addr_start)
{
    /***********根据自己的单片机flash函数适配*************/
    FlashWrite(len, data, addr_start);
    /*******************************************************/
}

void SetNextAPPAddress(void)
{
    /***********根据自己的单片机中断向量表偏移修改*************/
    if (SCB->VTOR == 0x08002000)
    {
        FlashDestination = 0x08008C00;
    }
    else if (SCB->VTOR == 0x08008C00)
    {
        FlashDestination = 0x08002000;
    }
    /*******************************************************/
}

/**
  * @brief  Receive a file using the ymodem protocol
  * @param  buf: Address of the first byte
  * @retval The size of the file
  * 此示例Ymodem，仅供参考，需要根据具体单片机调试
  */

int32_t Ymodem_Receive(uint8_t *buf)
{
    uint8_t packet_data[PACKET_1K_SIZE + PACKET_OVERHEAD], file_size[FILE_SIZE_LENGTH], *file_ptr, *buf_ptr;
    int32_t i, packet_length, session_done, file_done, packets_received, errors, session_begin, size = 0;
    int cnt = 0;
    /* Initialize FlashDestination variable */
    SetNextAPPAddress();
    
    for (session_done = 0, errors = 0, session_begin = 0;;)
    {
        for (packets_received = 0, file_done = 0, buf_ptr = buf;;)
        {
            switch (Receive_Packet(packet_data, &packet_length, NAK_TIMEOUT))
            {
            case 0:
                errors = 0;
                switch (packet_length)
                {
                /* Abort by sender */
                case -1:
                    Send_Byte(ACK);
                    return 0;
                /* End of transmission */
                case 0:
                    cnt++;
                    if (cnt == 1)
                    {
                        Send_Byte(NAK);
                    }
                    else
                    {
                        Send_Byte(ACK);
						Send_Byte(CRC16);
                    }
                    break;
                /* Normal packet */
                default:
                    if (cnt > 1)
                    {
                        Send_Byte(ACK);
                        file_done = 1;
                        session_done = 1;
                        break;
                    }
                    else if ((packet_data[PACKET_SEQNO_INDEX] & 0xff) != (packets_received & 0xff))
                    {
                        Send_Byte(NAK);
                    }
                    else 
                    {
                        if (packets_received == 0)
                        {
                            /* Filename packet */
                            if (packet_data[PACKET_HEADER] != 0)
                            {
                                /* Filename packet has valid data */
                                for (i = 0, file_ptr = packet_data + PACKET_HEADER; (*file_ptr != 0) && (i < FILE_NAME_LENGTH);)
                                {
                                    file_name[i++] = *file_ptr++;
                                }
                                file_name[i++] = '\0';
                                for (i = 0, file_ptr++; (*file_ptr != ' ') && (i < FILE_SIZE_LENGTH);)
                                {
                                    file_size[i++] = *file_ptr++;
                                }
                                file_size[i++] = '\0';
                                Str2Int(file_size, &size);

                                /* Test the size of the image to be sent */
                                /* Image size is greater than Flash size */
                                if (size > (OTA_FLASH_SIZE - 1))
                                {
                                    /* End session */
                                    Send_Byte(CA);
                                    Send_Byte(CA);
                                    return -1;
                                }
                                //FlashErase(FlashDestination+4,FlashDestination+26*1024);  //循环升级时第二次擦除有问题，注释也正常运行
                                Send_Byte(ACK);
                                Send_Byte(CRC16);
                            }
                            /* Filename packet is empty, end session */
                        }
                        /* Data packet */
                        else 
                        {
                            memset(buf_ptr, 0, packet_length);
                            memcpy(buf_ptr, packet_data + PACKET_HEADER, packet_length);
                            Write2Flash(packet_length, buf_ptr, FlashDestination);
                            FlashDestination += packet_length;
                            Send_Byte(ACK);
                        }
                        packets_received++;
                        session_begin = 1;
                    }
                } 
                break;
            case 1:
                Send_Byte(CA);
                Send_Byte(CA);
                return -3;
            default:
                if (session_begin > 0)
                {
                    errors++;
                }
                if (errors > MAX_ERRORS)
                {
                    Send_Byte(CA);
                    Send_Byte(CA);
                    return 0;
                }
                Send_Byte(CRC16);
                break;
            }
            if (file_done != 0)
            {
                break;
            }
        }
        if (session_done != 0)
        {
            break;
        }
    }
    return (int32_t)size;
}

/**
  * @brief  Update CRC16 for input byte
  * @param  CRC input value 
  * @param  input byte
   * @retval None
  */
uint16_t UpdateCRC16(uint16_t crcIn, uint8_t byte)
{
    uint32_t crc = crcIn;
    uint32_t in = byte | 0x100;
    do
    {
        crc <<= 1;
        in <<= 1;
        if (in & 0x100)
            ++crc;
        if (crc & 0x10000)
            crc ^= 0x1021;
    } while (!(in & 0x10000));
    return crc & 0xffffu;
}

/**
  * @brief  Cal CRC16 for YModem Packet
  * @param  data
  * @param  length
   * @retval None
  */
uint16_t Cal_CRC16(const uint8_t *data, uint32_t size)
{
    uint32_t crc = 0;
    const uint8_t *dataEnd = data + size;
    while (data < dataEnd)
        crc = UpdateCRC16(crc, *data++);

    crc = UpdateCRC16(crc, 0);
    crc = UpdateCRC16(crc, 0);
    return crc & 0xffffu;
}

/**
  * @brief  Cal Check sum for YModem Packet
  * @param  data
  * @param  length
   * @retval None
  */
uint8_t CalChecksum(const uint8_t *data, uint32_t size)
{
    uint32_t sum = 0;
    const uint8_t *dataEnd = data + size;
    while (data < dataEnd)
        sum += *data++;
    return sum & 0xffu;
}

/*******************************END OF FILE**********************************/

/**
  * @brief  Download a file via serial port
  * @param  None
  * @retval None
  */
void SerialDownload(void)
{
    uint8_t Number[10] = "          ";
    int32_t Size = 0;

    printf("Waiting for the file to be sent ... (press 'a' to abort)\n\r");
    Size = Ymodem_Receive(&tab_1024[0]);

    if (Size > 0)
    {
        printf("\n\n\r Programming Completed Successfully!\n\r\r\n[ Name: ");
        printf(file_name);
        Int2Str(Number, Size);
        printf(",Size: ");
        printf(Number);
        printf(" Bytes]\r\n");
    }
    else if (Size == -1)
    {
        printf("\n\n\rThe image size is higher than the allowed space memory!\n\r");
			  //uart_send_data((unsigned char *)"AT+RST\r\n", strlen("AT+RST\r\n"));
			  /***************测试示例代码***************/
			  NVIC_SystemReset();
			  /*****************************************/
    }
    else if (Size == -2)
    {
        printf("\n\n\rVerification failed!\n\r");
			  //uart_send_data((unsigned char *)"AT+RST\r\n", strlen("AT+RST\r\n"));
			  /***************测试示例代码***************/
			  NVIC_SystemReset();
			  /******************************************/
    }
    else if (Size == -3)
    {
        printf("\r\n\nAborted by user.\n\r");
			  //uart_send_data((unsigned char *)"AT+RST\r\n", strlen("AT+RST\r\n"));
			  /***************测试示例代码***************/
			  NVIC_SystemReset();
			  /******************************************/
    }
    else
    {
        printf("\n\rFailed to receive the file!\n\r");
			  //uart_send_data((unsigned char *)"AT+RST\r\n", strlen("AT+RST\r\n"));
			/***************测试示例代码***************/
			  NVIC_SystemReset();
			/*****************************************/
    }
}

uint32_t uart_isread_data(uint8_t *ch)
{
    if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
    {
        *ch = (uint8_t)USART_ReceiveData(USART1);
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
  * @brief  Print a character on the HyperTerminal
  * @param  c: The character to be printed
  * @retval None
  */
void SerialPutChar(uint8_t c)
{
    one_send_byte(c);
}

/****************************************END OF FILE****************************************/