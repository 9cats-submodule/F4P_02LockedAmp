#include "base.h"
#include "stdio.h"

const u32 SAVE_ADDR = 0x0000f000;//FLASH�����ַ
u8 STR_BUF[200]={0};

/*!
 *  \brief ����ɨ�躯��
 *  \param mode �Ƿ�֧������
 */
//����ɨ�躯��
//��ʹ��ʱע��
u8 KEY_Scan(u8 mode) {
	static u8 key_up = 1; //�������ɿ���־
	if (mode)
		key_up = 1; //������
	if (key_up && (KEY0 == 0 || KEY1 == 0)) {
		HAL_Delay(10); //ȥ����
		key_up = 0;
		if      (KEY0 == 0)
			return KEY0_PRES;
		else if (KEY1 == 0)
			return KEY1_PRES;
	} else if (KEY0 == 1 && KEY1 == 1)
		key_up = 1;
	return 0; // ��������
}

/*!
 *  \brief  ���ݱ��������0-д�� 1-���� 2-д��
 *  \param mode ����ģʽ
 */
void DATA_OP(u8 mode)
{
  u8 *VAR_ADDR   = (u8*)&Svar; //������ַ
  u32 FLASH_ADDR = SAVE_ADDR ; //FLASH�����׵�ַ
  u8  data;                    //�ݴ�����
  u16 size;                    //��ǰ�Ѿ������С

  for(size=0;size<sizeof(SVAR);size++,VAR_ADDR++,FLASH_ADDR++)
  {
		switch(mode)
		{
			case 0:W25QXX_Write(VAR_ADDR,FLASH_ADDR,1);break;
			case 1:W25QXX_Read (VAR_ADDR,FLASH_ADDR,1);break;
			case 2:{
					W25QXX_Read (&data,FLASH_ADDR,1);
				if(data != *VAR_ADDR) W25QXX_Write(VAR_ADDR,FLASH_ADDR,1);
			}
		}
  }
}

/*!
 *  \brief ���ݳ�ʼ����������KEY0��ָ�Ĭ��
 */
void DATA_INIT() {
  u8 key = KEY_Scan(0);
  if(key == KEY0_PRES)  DATA_OP(0);
  else                  DATA_OP(1);
}

/*!
 *  \brief ���ݸ���
 */
void DATA_UPDATE() {
    DATA_OP(2);
}

static u8 fac_us = 0; //us��ʱ������


/*!
 *  \brief  ��ʼ���ӳٺ���
 *  \param  SYSCLK   ϵͳʱ��Ƶ�ʣ���72Mhz����д72��
 *  \note   SYSTICK��ʱ�ӹ̶�ΪHCLKʱ�ӵ�1/8
 */
void delay_init(u8 SYSCLK) {
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK); //SysTickƵ��ΪHCLK
	fac_us = SYSCLK;
}


/*!
 *  \brief  �ӳ�ns
 *  \param  t ns��
 *  \note   delay_init��ʹ��
 */
void delay_ns(u8 t) {
	do {
		;
	} while (--t);
}


/*!
 *  \brief  �ӳ�us
 *  \param  t us��
 *  \note   delay_init��ʹ��
 */
void delay_us(u32 nus) {
	u32 ticks;
	u32 told, tnow, tcnt = 0;
	u32 reload = SysTick->LOAD; //LOAD��ֵ
	ticks = nus * fac_us;       //��Ҫ�Ľ�����
	told = SysTick->VAL;        //�ս�ʱ�ļ�����ֵ
	while (1) {
		tnow = SysTick->VAL;
		if (tnow != told) {
			if (tnow < told)
				tcnt += told - tnow; //��ע����SYSTICK�Ǹ��ݼ��ļ������Ϳ�����
			else
				tcnt += reload - tnow + told;
			told = tnow;
			if (tcnt >= ticks)
				break; //ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
		}
	};
}


/*!
 *  \brief  �ӳ�ms
 *  \param  t ms��
 *  \note   delay_init��ʹ��
 */
void delay_ms(u16 nms) {
	delay_us(1000*nms);
}
