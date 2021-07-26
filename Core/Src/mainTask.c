#include "main.h"
#include "ADS8688.h"
#include "spi.h"
#include "hmi_driver.h"
//#include "cmsis_os.h"
//#include "arm_math.h"
//#include "arm_const_structs.h"
//#include "fdacoefs.h"

#define SAMPLE_BEGIN HAL_GPIO_WritePin(ADS8688_CS_GPIO_Port, ADS8688_CS_Pin, GPIO_PIN_RESET);
#define SAMPLE_END   HAL_GPIO_WritePin(ADS8688_CS_GPIO_Port, ADS8688_CS_Pin, GPIO_PIN_SET);
//
///*!
// *  \brief  TIM1 ���� PRC, ARR
// *  \param  PRC Ԥ����ϵ��
// *  \param  ARR �Զ���װ��ֵ
// */
void TIM1_CONFIG(u32 PRC,u32 ARR)
{
	__HAL_TIM_DISABLE(&htim1);
	__HAL_TIM_SET_PRESCALER(  &htim1, PRC);
	__HAL_TIM_SET_AUTORELOAD( &htim1, ARR);
	__HAL_TIM_SET_COUNTER(    &htim1,   0);
	__HAL_TIM_ENABLE(&htim1);
}
//
///*!
// *  \brief  ADS8688 ��ͨ�� ��̬����
// *  \param  CH ��ѡͨ�� (�� 0b11011101)
// *  \param  range ͨ����Χ
// *  \warn!  ��ʼ����120ms�����ڲ���ֵ����ȷ
// */
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
//
///*!
// *  \brief  AD9959 ��̬����
// *  \param  freq Ƶ��
// *  \param  amp  ����
// *  \warn!  ��ʼ������Ҫһ��ʱ��
// */
//void AD9959_CONFIG(float freq,float mv)
//{
//	//ֻ��ͨ��1 �� ͨ��3����
//	Out_freq(2, freq);
//	Out_mV(2, mv);
//}
//
///*!
// *  \brief  ���� TIM1 ��ʱ����һ������
// *  \param  point ÿ��ͨ���Ĳ�������
// */
void ADS8688_SAMPLE(u16 point)
{
	SAMPLE_POINT = CH_NUM * point;
	__HAL_TIM_SET_COUNTER( &htim1, 0);
	__HAL_TIM_ENABLE_IT(   &htim1, TIM_IT_UPDATE);
	while(SAMPLE_FINISHED == NO)
		osDelay(1);
	SAMPLE_FINISHED = NO;
}
//
///*!
// *  \brief    FFT�任
// *  \param    CH ��ѡͨ�� (0~CH_NUL)
// *  \detail   ִ�к� FFT_OUTPUT �� FFT_OUTPUT_REAL ���½��
// */
//void FFT(u8 CH)
//{
//	arm_rfft_fast_instance_f32 S;
//	u16 i;
//
//	for(i=0;i<2048;i++)
//	{
//		FFT_INPUT[i] = ((float)ADS8688_BUF[CH][i] - Svar.ADS_OFFSET_ALL)*Svar.ADS_AMP/0x10000;
//	}
//	arm_rfft_fast_init_f32(&S,2048);                    //FFT��ʼ��
//	arm_rfft_fast_f32(&S, FFT_INPUT, FFT_OUTPUT,0);     //FFT�仯
//	arm_cmplx_mag_f32(FFT_OUTPUT,FFT_OUTPUT_REAL,2048); //��ģ
//}
//
///*!
// *  \brief    THD ��ʧ���
// *  \param    index ��Ӧ������Ƶ��
// *  \detail   Ҫ�Ƚ��� FFT
// */
//float THD(u16 index)
//{
//  //����
//  float basic = FFT_OUTPUT_REAL[index];
//  //г��
//  float harmon[5],harmon_power,high;
//  u8 i;
//
//  for(i=0;i<5;i++)
//  {
//  	harmon[i] = FFT_OUTPUT_REAL[index*(i+2)];
//  }
//  arm_power_f32(harmon, 5, &harmon_power);
//  arm_sqrt_f32(harmon_power,&high);
//
//  return high / basic;
//}
//
///*!
// *  \brief    FFT�˲�
// *  \param    CH ��ѡͨ�� (0~CH_NUL)
// *  \param    point ����
// */
//void FIR(u8 CH,u32 point)
//{
//	u32 i;
//	arm_fir_instance_f32 S;
//	u16 numBlocks = point/blockSize; //��Ҫ����arm_fir_f32�Ĵ���
//
//	/* ��ʼ�����뻺�� */
//	for(i=0;i<point;i++)
//		FIR_INPUT[i] = ((float)ADS8688_BUF[CH][i] - Svar.ADS_OFFSET_ALL)*Svar.ADS_AMP/0x10000;
//
//	/* ��ʼ���ṹ��S */
//	arm_fir_init_f32(&S, firCoeffsNum,(float*)firCoeffs, FIR_STATE, blockSize);
//
//	/* ʵ��FIR�˲� */
//	for(i=0;i<numBlocks;i++)
//		arm_fir_f32(&S, FIR_INPUT + (i * blockSize), FIR_OUTPUT + (i * blockSize), blockSize);
//}
//
///*!
// *  \brief  ������
// */

void MainTask_Start(void *argument)
{
  /* Infinite loop */
	TIM1_CONFIG(25-1,210-1);						//������ 32K
	ADS8688_MUL_CONFIG(0b00000100,2);		//ADS 3ͨ����������5.12 V
	osDelay(300);

	for(;;){
		SetTextValue(1, 41, (u8*)"233");
		osDelay(1000);
	}
}