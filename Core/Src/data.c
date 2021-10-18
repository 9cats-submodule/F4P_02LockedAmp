#include "data.h"

typedef struct HeapRegion
{
	uint8_t *pucStartAddress;
	size_t xSizeInBytes;
} HeapRegion_t;
//_CCM u8 MEM_CCMRAM[48 ][1024] = {0};

//可储存变量
SVAR Svar = {
  /*float ADS_AMP;         //所有ADS倍数补偿 */0.965000033f,
  /*u16   ANTI_SHAKE_PHASE;//消抖偏移        */6000,
  /*u32   OFFSET_PHASE;    //相位偏移        */39660,
  /*float a;               //线性拟合系数a   */40.915f,
  /*float b;               //线性拟合系数b   */1.3252f,
  /*float c;               //线性拟合系数c   */80.313f,
  /*float d;               //线性拟合系数d   */1.2131f,
  /*float e;               //线性拟合系数e   */506.6619188224806f,
};
//39660 + 42000 = 81660

//全局通用变量
u8 MODE = 0;   //当前模式
u8 CH_NUM = 0; //所选通道总数
u32 LP_Input_Freqency=1000; //锁相输入频率

// ADS8688 采样完成信号量
u8  SAMPLE_FINISHED = NO;
//用于 ADS8688 采样
u32 SAMPLE_POINT      =  0;
u16 ADS8688_BUF[SAMPLE_CH_MAX][SAMPLE_POINT_MAX+200] = {0};

// FreeRTOS 堆栈区
_CCM u8 ucHeap[configTOTAL_HEAP_SIZE];
const HeapRegion_t xHeapRegions[] =
{
    { ucHeap, configTOTAL_HEAP_SIZE },
    { NULL, 0 }
};
