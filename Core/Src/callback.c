#include "main.h"
#include "stm32f4xx_it.h"

#include "usart.h"
#include "spi.h"
#include "cmsis_os.h"
#include "ADS8688.h"
#include "cmd_process.h"
#include "cmd_queue.h"
#include "esp8266.h"

//-------------------ADS8688接收和发送BUF-----------------
static u8  rxbuf [4]    = {0};
static u8  txbuf [2]    = {0};
//--------------------------------------------------------

//-------------------------------标志---------------------------------------------
u8 ADS8688_BUSY   = 0;     //ADS8688 DMA接收还未完成
u8 FreMeasure_STA = 0;     //等距测量状态 0-预备 1-初次捕获 2-正在 3-过阈值 4-完成
float RefFrequency = 84001168.0f;
u32 OFFSET = 79000;
//--------------------------------------------------------------------------------
void TIM1_PeriodElapsedCallback(void);  //TIM1 更新ISR--用于ADS采样

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM6) HAL_IncTick(); // FreeRTOS 系统时钟
	if (htim == &htim1) TIM1_PeriodElapsedCallback();
	if (htim == &htim2) {
		__HAL_TIM_DISABLE(&htim2);
		HAL_GPIO_WritePin(LOCK_PHASE_OUT_GPIO_Port, LOCK_PHASE_OUT_Pin, GPIO_PIN_SET);
	}
	if (htim == &htim5) {
		__HAL_TIM_DISABLE(&htim5);
		HAL_GPIO_WritePin(LOCK_PHASE_OUT_GPIO_Port, LOCK_PHASE_OUT_Pin, GPIO_PIN_RESET);
	}
	if (htim == &htim7) {
		if(ESP8266_ACK_STA == 1)
		{
			if(ESP8266_Timeout_Tick-- == 0)
			{
				ESP8266_ACK_STA=3;
				HAL_TIM_Base_Stop_IT(&htim7);
			}
		}
	}
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	static u8 CaptureDir = 1;
	if(htim == &htim8) {
		switch(CaptureDir)
		{
			//下降沿捕获
			case 0:{
				__HAL_TIM_SET_CAPTUREPOLARITY(&htim8, TIM_CHANNEL_4, TIM_INPUTCHANNELPOLARITY_FALLING);
				__HAL_TIM_SET_COUNTER(&htim2,OFFSET);
				__HAL_TIM_ENABLE(&htim2);
				CaptureDir=1;
			}break;
			//上升沿捕获
			case 1:{
				__HAL_TIM_SET_CAPTUREPOLARITY(&htim8, TIM_CHANNEL_4, TIM_INPUTCHANNELPOLARITY_RISING);
				__HAL_TIM_SET_COUNTER(&htim5,OFFSET);
				__HAL_TIM_ENABLE(&htim5);
				CaptureDir=0;
			}break;
		}
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart == &huart6)
	{
		huart6.RxState = HAL_UART_STATE_READY;
		__HAL_UNLOCK(&huart6);
		/*TFT
		queue_push(RxBuffer);
		if(queue_find_cmd(cmd_buffer,CMD_MAX_SIZE))
		{
			osMessageQueuePut(USART6_RXHandle,cmd_buffer,0,0);
		}
		*/
		//
		ESP8266_RX_PUSH(RxBuffer);
		if(ESP8266_RX_Find(ESP8266_ACK_BUF, ESP8266_ACK_Size, ESP8266_Response_Ptr))
			if(ESP8266_ACK_STA == 1)
			{
				ESP8266_ACK_STA = 2;
				HAL_TIM_Base_Stop_IT(&htim7);
			}
		//
		HAL_UART_Receive_IT(&huart6, &RxBuffer, 1);
	}
}

void TIM1_PeriodElapsedCallback(void) {
  static u16 i=0;
  static u8  IS_FIRST = 1; //是否第一次进入中断 （第一次进入中断无法获取到值）
  if(!ADS8688_BUSY) {
    //开启下一次扫描
    ADS8688_BUSY = 1;
    SAMPLE_BEGIN;  //重新拉低CS，ADS8688开始运输
  	if(IS_FIRST == YES) {
  		HAL_SPI_TransmitReceive_DMA(&hspi3, txbuf, rxbuf, 2);
  		IS_FIRST = NO;
  	}
  	else {
  		ADS8688_BUF[i%CH_NUM][i/CH_NUM] = *(u16*)(&rxbuf[2]); //将采样值储存在BUF中
  		HAL_SPI_TransmitReceive_DMA(&hspi3, txbuf, rxbuf, 2);

  		if(++i == SAMPLE_POINT) {
  			//定时器任务结束
  			i=0;
  			SAMPLE_FINISHED = IS_FIRST = YES;
  			__HAL_TIM_DISABLE_IT(&htim1, TIM_IT_UPDATE);
  		}
  	}
  }
  else
  {
  //正常情况无法到此处
    ADS8688_BUSY = ADS8688_BUSY;
  }
}

