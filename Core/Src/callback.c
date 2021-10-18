#include "main.h"
#include "stm32f4xx_it.h"

#include "usart.h"
#include "spi.h"
#include "cmsis_os.h"
#include "ADS8688.h"
#include "cmd_process.h"
#include "cmd_queue.h"

//-------------------ADS8688���պͷ���BUF-----------------
static u8  rxbuf [4]    = {0};
static u8  txbuf [2]    = {0};
//--------------------------------------------------------

//-------------------------------��־---------------------------------------------
u8 ADS8688_BUSY   = 0;     //ADS8688 DMA���ջ�δ���
u8 FreMeasure_STA = 0;     //�Ⱦ����״̬ 0-Ԥ�� 1-���β��� 2-���� 3-����ֵ 4-���
float RefFrequency = 84001168.0f;
u8 CaptureDir = 1;
u8 CaptureDir_Locked = 0;
//--------------------------------------------------------------------------------
void TIM1_PeriodElapsedCallback(void);  //TIM1 ����ISR--����ADS����

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM6) HAL_IncTick(); // FreeRTOS ϵͳʱ��
	if (htim == &htim1) TIM1_PeriodElapsedCallback();
	if (htim == &htim2) {
		//����ߵ�ƽ
		HAL_GPIO_WritePin(LOCK_PHASE_OUT_GPIO_Port, LOCK_PHASE_OUT_Pin, GPIO_PIN_RESET);
	}
	if (htim == &htim5) {
		//����ߵ�ƽ
		HAL_GPIO_WritePin(LOCK_PHASE_OUT_GPIO_Port, LOCK_PHASE_OUT_Pin, GPIO_PIN_SET);
	}
	if (htim == &htim13) {
		//��ʱ���
		__HAL_TIM_SET_COUNTER(&htim2, Svar.OFFSET_PHASE);
		__HAL_TIM_ENABLE(&htim2);

		//��ʼ׼������������
		CaptureDir=1;
		__HAL_TIM_SET_CAPTUREPOLARITY(&htim8, TIM_CHANNEL_4, TIM_INPUTCHANNELPOLARITY_RISING);
		__HAL_TIM_CLEAR_IT(&htim8,TIM_IT_CC4);
		__HAL_TIM_ENABLE_IT(&htim8, TIM_IT_CC4);
	}
	if (htim == &htim14) {
		//��ʱ���
		__HAL_TIM_SET_COUNTER(&htim5, Svar.OFFSET_PHASE);
		__HAL_TIM_ENABLE(&htim5);

		//��ʼ׼�������½���
		CaptureDir=0;
		__HAL_TIM_SET_CAPTUREPOLARITY(&htim8, TIM_CHANNEL_4, TIM_INPUTCHANNELPOLARITY_FALLING);
		__HAL_TIM_CLEAR_IT(&htim8,TIM_IT_CC4);
		__HAL_TIM_ENABLE_IT(&htim8, TIM_IT_CC4);
	}
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim8) {
		//ʧ���ж�
		__HAL_TIM_DISABLE_IT(&htim8, TIM_IT_CC4);
		switch(CaptureDir)
		{
			//�½��ز���
			case 0:{
				//��������
				__HAL_TIM_SET_AUTORELOAD(&htim13, Svar.ANTI_SHAKE_PHASE);
				__HAL_TIM_ENABLE(&htim13);
			}break;
			//�����ز���
			case 1:{
				//��������
				__HAL_TIM_SET_AUTORELOAD(&htim14, Svar.ANTI_SHAKE_PHASE);
				__HAL_TIM_ENABLE(&htim14);
			}break;
		}
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
//TFT��������
#ifdef TFT_USART
	if (huart == &TFT_USART)
	{
		//������ն���
		queue_push(TFT_RX_BUF);
		if(queue_find_cmd(cmd_buffer,CMD_MAX_SIZE))
			osMessageQueuePut(USART6_RXHandle,cmd_buffer,0,0);
		//������һ�ν���
		HAL_UART_Receive_IT(&TFT_USART, &TFT_RX_BUF, 1);
	}
#endif
}

void TIM1_PeriodElapsedCallback(void) {
  static u16 i=0;
  static u8  IS_FIRST = 1; //�Ƿ��һ�ν����ж� ����һ�ν����ж��޷���ȡ��ֵ��
  if(!ADS8688_BUSY) {
    //������һ��ɨ��
    ADS8688_BUSY = 1;
    SAMPLE_BEGIN;  //��������CS��ADS8688��ʼ����
  	if(IS_FIRST == YES) {
  		HAL_SPI_TransmitReceive_DMA(&hspi3, txbuf, rxbuf, 2);
  		IS_FIRST = NO;
  	}
  	else {
  		ADS8688_BUF[i%CH_NUM][i/CH_NUM] = *(u16*)(&rxbuf[2]); //������ֵ������BUF��
  		HAL_SPI_TransmitReceive_DMA(&hspi3, txbuf, rxbuf, 2);

  		if(++i == SAMPLE_POINT) {
  			//��ʱ���������
  			i=0;
  			SAMPLE_FINISHED = IS_FIRST = YES;
  			__HAL_TIM_DISABLE_IT(&htim1, TIM_IT_UPDATE);
  		}
  	}
  }
  else
  {
  //��������޷����˴�
    ADS8688_BUSY = ADS8688_BUSY;
  }
}

void TFT_TxCallback(void)
{
	osSemaphoreRelease(TFT_TX_LEDHandle);
}

