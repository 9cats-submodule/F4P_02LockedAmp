#include "hmi_user_uart.h"

//串口接收BUF
uint8_t TFT_RX_BUF;

/*!
 *  \brief    TFT串口屏初始化
 *  \detail   无
 */
void TFT_Init(void)
{
	HAL_UART_Receive_IT(&TFT_USART, &TFT_RX_BUF, 1);//使能接收中断
}

/*!
 *  \brief    TFT串口屏发送数据重定义
 *  \param    t 发送的字符串
 *  \detail   无
 */
void SendChar(uint8_t t)
{
	HAL_UART_Transmit(&TFT_USART, &t, 1, 1);
}

/*!
 *  \brief    TFT串口屏发送回调函数
 *  \detail   在帧尾发送完毕后调用此函数
 */
__weak void TFT_TxCallback(void)
{
	return;
}
