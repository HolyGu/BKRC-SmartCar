
#include <stdint.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "delay.h"
#include "CanP_HostCom.h"
#include "can_user.h"

#define __HARD_CAN_C__
#include "Hard_Can.h"


CanTxMsg TxMessage;
CanRxMsg RxMessage;


  
void Hard_Can_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;	

//	HardCan_times = 1000;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOF, ENABLE);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_CAN1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_CAN1);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(GPIOF, &GPIO_InitStructure);	
	Hard_Can_SpeedMode(0);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	CAN_DeInit(CAN1);
	
	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = ENABLE;//DISABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_NART = ENABLE;	//DISABLE;
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = ENABLE;//DISABLE;
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;//CAN_Mode_LoopBack;//CAN_Mode_Normal;	
	
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_2tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;
	CAN_InitStructure.CAN_Prescaler = 7;//5;
	CAN_Init(CAN1, &CAN_InitStructure);	
	

	
	CAN_FilterInitStructure.CAN_FilterNumber = 0;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;				//标识符屏蔽模式
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;			
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	TxMessage.StdId = 0x12;//0x321;
	TxMessage.ExtId = 0x12;//0x01;
	TxMessage.RTR = CAN_RTR_DATA;
	TxMessage.IDE = CAN_ID_STD;
	TxMessage.DLC = 1;	

	//Ccp_RxFlag = 0;
	CAN_ITConfig(CAN1, CAN_IT_FMP0, DISABLE);	
	
	CanP_Init();

}

void Hard_Can_SpeedMode(uint8_t f)
{
    if(f == 0)
      GPIO_ResetBits(GPIOF,GPIO_Pin_10);
    else
      GPIO_SetBits(GPIOF,GPIO_Pin_10);	
}


/**
函数功能：CAN设备循环检测
*/
void Hard_Can_While(void)
{
	Canuser_main();		
}

//end file

