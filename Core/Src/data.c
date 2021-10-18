#include "data.h"

typedef struct HeapRegion
{
	uint8_t *pucStartAddress;
	size_t xSizeInBytes;
} HeapRegion_t;
//_CCM u8 MEM_CCMRAM[48 ][1024] = {0};

//�ɴ������
SVAR Svar = {
  /*float ADS_AMP;         //����ADS�������� */0.965000033f,
  /*u16   ANTI_SHAKE_PHASE;//����ƫ��        */6000,
  /*u32   OFFSET_PHASE;    //��λƫ��        */39660,
  /*float a;               //�������ϵ��a   */40.915f,
  /*float b;               //�������ϵ��b   */1.3252f,
  /*float c;               //�������ϵ��c   */80.313f,
  /*float d;               //�������ϵ��d   */1.2131f,
  /*float e;               //�������ϵ��e   */506.6619188224806f,
};
//39660 + 42000 = 81660

//ȫ��ͨ�ñ���
u8 MODE = 0;   //��ǰģʽ
u8 CH_NUM = 0; //��ѡͨ������
u32 LP_Input_Freqency=1000; //��������Ƶ��

// ADS8688 ��������ź���
u8  SAMPLE_FINISHED = NO;
//���� ADS8688 ����
u32 SAMPLE_POINT      =  0;
u16 ADS8688_BUF[SAMPLE_CH_MAX][SAMPLE_POINT_MAX+200] = {0};

// FreeRTOS ��ջ��
_CCM u8 ucHeap[configTOTAL_HEAP_SIZE];
const HeapRegion_t xHeapRegions[] =
{
    { ucHeap, configTOTAL_HEAP_SIZE },
    { NULL, 0 }
};
