#include "stm32f4xx.h"
#include "tba.h"
/**
�������ܣ�����������������
��    ������
�� �� ֵ����
*/
void Tba_Photoresistance_Config(void)
{
	GPIO_InitTypeDef  GPIO_TypeDefStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);
	
	GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_IN;		//����
	GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;   	//����
	GPIO_Init(GPIOA,&GPIO_TypeDefStructure);
}

uint8_t Get_tba_phsis_value(void)
{
	return GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_8);
}

/**
�������ܣ���������������
��    ������
�� �� ֵ����
*/
void Tba_Beep_Config(void)
{
	GPIO_InitTypeDef  GPIO_TypeDefStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	
	GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;		//����
	GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;   		//����
	GPIO_Init(GPIOC,&GPIO_TypeDefStructure);
	GPIO_SetBits(GPIOC,GPIO_Pin_13);
}

/**
�������ܣ���������������״̬
��    ����swch SET ���� RESET �ر�
�� �� ֵ����
*/
void Set_tba_Beep(uint8_t swch)
{
	if(swch == SET)
	{
		GPIO_ResetBits(GPIOC,GPIO_Pin_13);
	} 
	else if(swch == RESET)
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_13);
	}
}


/**
�������ܣ������ת�������
��    ��: ��
�� �� ֵ����
*/
void Tba_WheelLED_Config(void)
{
	GPIO_InitTypeDef  GPIO_TypeDefStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH,ENABLE);
	
	// LED_L -- PH10  LED_R --PH11
	GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
	GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;		//����
	GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;   		//����
	GPIO_Init(GPIOH,&GPIO_TypeDefStructure);
	GPIO_SetBits(GPIOH,GPIO_Pin_10|GPIO_Pin_11);
}


/**
��������: �����ת��ƿ���
��    ����LorR  L_LED--���   R_LED--�Ҳ�
		  swch  SET -- ��     RESET--��
�� �� ֵ: ��
*/
void Set_tba_WheelLED(uint8_t LorR,uint8_t swch)
{
	switch(LorR)
	{
		case L_LED:
			if(swch)
				GPIO_ResetBits(GPIOH,GPIO_Pin_10);
			else	
				GPIO_SetBits(GPIOH,GPIO_Pin_10);
			break;
		case R_LED:
			if(swch)
				GPIO_ResetBits(GPIOH,GPIO_Pin_11);				
			else
				GPIO_SetBits(GPIOH,GPIO_Pin_11);
			break;
	}
}

/**
��������: ������ʼ��
��    ������
�� �� ֵ: ��
*/
void Tba_Init(void)
{
	Tba_Photoresistance_Config();
	Tba_Beep_Config();
	Tba_WheelLED_Config();
}

