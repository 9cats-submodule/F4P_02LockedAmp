#include "main.h"
#include "cmd_process.h"
//#include "cmsis_os.h"
//#include "semphr.h"
//
//
///*!
// *  \brief    �����Զ��洢
// *  \details  ÿ��0.1s����һ�����ݣ����ȼ����
// */
void FLASH_Data_AutoUpdate_Start(void *arguement)	{
	for(;;)
	{
		DATA_UPDATE();
		osDelay(100);
	}
}
//
///*!
// *  \brief    TFT����ָ���
// *  \details  ��USART1���ڽ��յ�TFT��ָ��ʱ�Ӵ�����
// */
void TFT_CMD_Process_Start(void *argument)	{
	CTRL_MSG   TFT_CMD_MSG = {0};

  for(;;)
  {
  	if(USART6_RXHandle != NULL)
  	{
  		if(osMessageQueueGet(USART6_RXHandle, &TFT_CMD_MSG, 0, 0) == osOK)
  		{
  				ProcessMessage(&TFT_CMD_MSG,0);
  				osSemaphoreRelease(TFT_RX_LEDHandle);
  		}
  		osDelay(20);
  	}
  	osDelay(20);
  }
}
//
///*!
// *  \brief    TFT-���� LED0 ��˸
// *  \details  ÿ�η���һ��ָ��, LED0 ��˸һ�Σ�����0.02 s
// */
void StartLED0Toggle(void const *argument) {
	for (;;) {
		if(osSemaphoreAcquire(TFT_TX_LEDHandle , 0) == osOK)
		{
			LED0_ON;
			osDelay(30);
			LED0_OFF;
		}
		osDelay(10);
	}
}
//
///*!
// *  \brief    TFT-���� LED1 ��˸
// *  \details  ÿ����һ��ָ��, LED1 ��˸һ��,���� 0.02 s
// */
void StartLED1Toggle(void const *argument) {
  for (;;) {
		if(osSemaphoreAcquire(TFT_RX_LEDHandle , 0) == osOK)
  	{
    	LED1_ON;
    	osDelay(30);
    	LED1_OFF;
  	}
		osDelay(10);
  }
}