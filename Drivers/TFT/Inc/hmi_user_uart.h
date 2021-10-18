#ifndef _USER_UART__
#define _USER_UART__

#include "base.h"

//重定义会用到的数据类型
#define uchar    unsigned char
#define uint8    unsigned char
#define uint16   unsigned short int
#define uint32   unsigned long
#define int16    short int
#define int32    long

//用户自定义修改
#define TFT_USART huart6           /*!< */
#define CRC16_ENABLE     0         /*!< 如果需要CRC16校验功能，修改此宏为1(此时需要在VisualTFT工程中配CRC校验)*/
#define CMD_MAX_SIZE     25        /*!<单条指令大小，根据需要调整，尽量设置大一些*/
#define QUEUE_MAX_SIZE   128       /*!< 指令接收缓冲区大小，根据需要调整，尽量设置大一些*/

//一个字节的接收BUF
extern uint8_t TFT_RX_BUF;

//用户需要使用的函数声明
void TFT_Init(void);       //TFT初始化
void SendChar(uint8_t t);          //TFT数据发送重定义
void TFT_TxCallback(void);         //TFT数据发送完成之后的回调函数
#endif
