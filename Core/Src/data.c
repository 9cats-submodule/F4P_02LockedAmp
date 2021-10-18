#include "data.h"

typedef struct HeapRegion
{
	uint8_t *pucStartAddress;
	size_t xSizeInBytes;
} HeapRegion_t;
//_CCM u8 MEM_CCMRAM[48 ][1024] = {0};

//�ɴ������
SVAR Svar = {
  /*float ADS_AMP;         //����ADS�������� */0.990999997f,
  /*u16   ANTI_SHAKE_PHASE;//����ƫ��        */4000,
  /*u32   OFFSET_PHASE;    //��λƫ��        */43600,
  /*float a;               //�������ϵ��a   */47.689f,
  /*float b;               //�������ϵ��b   */2.9528f,
  /*float c;               //�������ϵ��c   */127.57f,
  /*float d;               //�������ϵ��d   */2.5288f,
  /*float e;               //�������ϵ��e   */343.42f,
};


//ȫ��ͨ�ñ���
u8 MODE = 0;   //��ǰģʽ
u8 CH_NUM = 0; //��ѡͨ������
u32 LP_Input_Freqency=1000; //��������Ƶ��

// ADS8688 ��������ź���
u8  SAMPLE_FINISHED = 0;
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
