#include "main.h"
#include "ADS8688.h"
#include "AD9959.h"
#include "hmi_driver.h"
#include "arm_math.h"
#include "arm_const_structs.h"
#include "fdacoefs.h"
#include "stdio.h"
/*!
 *  \brief  TIM1 配置 PRC, ARR
 *  \param  PRC 预分配系数
 *  \param  ARR 自动重装载值
 */
void TIM1_CONFIG(u32 PRC,u32 ARR)
{
	__HAL_TIM_DISABLE(&htim1);
	__HAL_TIM_SET_PRESCALER(  &htim1, PRC);
	__HAL_TIM_SET_AUTORELOAD( &htim1, ARR);
	__HAL_TIM_SET_COUNTER(    &htim1,   0);
	__HAL_TIM_ENABLE(&htim1);
}

/*!
 *  \brief  ADS8688 多通道 动态配置
 *  \param  CH 所选通道 (例 0b11011101)
 *  \param  range 通道范围
 *  \warn!  初始化后120ms左右内采样值不精确
 */
void ADS8688_MUL_CONFIG(u8 CH,u8 range)
{
	CH_NUM = 0;
	ADS8688_CONFIG(CH,range);
	while(CH)
	{
		CH_NUM += CH & 0x01;
		CH >>= 1;
	}
}

/*!
 *  \brief  AD9959 动态配置
 *  \param  freq 频率
 *  \param  amp  幅度
 *  \warn!  初始化后需要一定时间
 */
void AD9959_CONFIG(float freq,float mv)
{
	//只有通道1 和 通道3可用
	Out_freq(2, freq);
	Out_mV(2, mv);
}

/*!
 *  \brief  开启 TIM1 定时采样一定点数
 *  \param  point 每个通道的采样点数
 */
u32 ADS8688_SAMPLE(u16 point)
{
	extern SPI_HandleTypeDef hspi3;
	u8  rxbuf [4]    = {0};
	u8  txbuf [4]    = {0};
	u32 data=0;
	u16 i;
	for(i=0;i<point;i++)
	{
		SAMPLE_BEGIN;  //重新拉低CS，ADS8688开始运输
		HAL_SPI_TransmitReceive(&hspi3, txbuf, rxbuf, 2,20);
		SAMPLE_END;
		data += *(u16*)(&rxbuf[2]);
		delay_us(31); //30K
	}
	return data;
}



void Bubble(u16 r[], u16 n)
{
  u16 low = 0;
  u16 high= n -1;
  u16 tmp,j;
  while(low < high)
  {
    for(j=low; j<high; ++j) //正向冒泡,找到最大者
    {
      if(r[j]> r[j+1])
      {
        tmp = r[j];
        r[j]=r[j+1];
        r[j+1]=tmp;
      }
      --high;
      for(j=high; j>low; --j) //反向冒泡,找到最小者
      {
        if(r[j]<r[j-1])
        {
          tmp = r[j];
          r[j]=r[j-1];
          r[j-1]=tmp;
        }
        ++low;
      }
    }
  }
}

/*!
 *  \brief  主任务
 */
void MainTask_Start(void *argument)
{
  /* Infinite loop */
	//TIM1_CONFIG(25-1,210-1);								//采样率 32K
	ADS8688_MUL_CONFIG(0b00000001,0x06);		//ADS 1通道开启，0~5.12V
	osDelay(2000);

	//显示DEBUF信息
	SetTextValue(0, 21, (u8*)"采样值");
	SetTextValue(0, 22, (u8*)"实际值");
	SetTextValue(0, 1, Str("%d",Svar.ANTI_SHAKE_PHASE));
	SetTextValue(0, 2, Str("%ld",Svar.OFFSET_PHASE));
	SetTextValue(0, 3, Str("%f",Svar.a));
	SetTextValue(0, 4, Str("%f",Svar.b));
	SetTextValue(0, 5, Str("%f",Svar.c));
	SetTextValue(0, 6, Str("%f",Svar.d));
	SetTextValue(0, 7, Str("%f",Svar.e));

	SetTextValue(0, 9, Str("%f",Svar.ADS_AMP));

	for(;;){
		float value=0;
		value = ADS8688_SAMPLE(25000)/25000*5.12f/Svar.ADS_AMP*1000/0xffff;

//		value = ADS8688_BUF[0][20000]*5.12f/Svar.ADS_AMP*1000/0xffff;
		if(value != 0) //非反向
		{
			SetTextValue(0, 31, Str("%.2f mv",value));
			if(value < Svar.e)
				SetTextValue(0, 32, Str("%.2f uV", (value-Svar.a) / Svar.b));
			else
				SetTextValue(0, 32, Str("%.2f uV", (value-Svar.c) / Svar.d));
		}
		else
		{
			Svar.OFFSET_PHASE = (Svar.OFFSET_PHASE+42000)%84000;
		}

		osDelay(100);
	}
}
