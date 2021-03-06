#ifndef __DATA_H
#define __DATA_H
#include "base.h"
#include "cmsis_os2.h"
#include "FreeRTOSConfig.h"

extern u8 MEM_CCMRAM[][1024];

typedef struct _SVAR
{
  float ADS_AMP;         //所有ADS倍数补偿
  u16   ANTI_SHAKE_PHASE;//消抖偏移
  u32   OFFSET_PHASE;    //相位偏移
  float a;               //线性拟合系数a
  float b;               //线性拟合系数b
  float c;               //线性拟合系数c
  float d;               //线性拟合系数d
  float e;               //线性拟合系数e
}SVAR;
extern SVAR Svar;

//全局通用变量
extern u8 MODE;   //当前模式
extern u8 CH_NUM; //所选通道总数
extern u32 LP_Offset_Phase;//相位偏移值
extern u32 LP_Input_Freqency; //锁相输入频率

//FreeRTOS--任务信息量句柄
extern osMessageQueueId_t USART6_RXHandle;
extern osSemaphoreId_t    TFT_TX_LEDHandle;
extern osSemaphoreId_t    TFT_RX_LEDHandle;
// ADS8688 采样完成信号量
extern u8  SAMPLE_FINISHED;
//用于 ADS8688 采样
#define SAMPLE_POINT_MAX 40000
#define SAMPLE_CH_MAX 1
extern u32 SAMPLE_POINT;
extern u16 ADS8688_BUF[][SAMPLE_POINT_MAX+200];
//FFT变换用
extern float FFT_INPUT[];
extern float FFT_OUTPUT[];
extern float FFT_OUTPUT_REAL[];
//FIR滤波用
extern u16   blockSize; //调用一次fir处理的采样点个数
extern float FIR_INPUT[];
extern float FIR_OUTPUT[];
extern float FIR_STATE[];
//------以下基本不用动-------
#endif
