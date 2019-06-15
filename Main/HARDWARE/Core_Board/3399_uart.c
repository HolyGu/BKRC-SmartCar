#include <stdint.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "delay.h"
#include "can_user.h"
#include "data_channel.h"
#include "Timer.h"

#define __HARD_UART_C__
#include "3399_uart.h"

/*
rfid com1
tx	pa9
rx	pa10

?????
tx  pa0	com4tx
rx	pd6 com2rx

????????? (0<->ap    1<->bt)
u_ctr pi8

????? com6
tx pc6
rx pc7
*/

//USART_TypeDef* uart_prot_buf[]=
//{
//	NULL,
//	USART1,
//	USART2,
//	USART3,
//	UART4,
//	UART5,
//};

static uint32_t HardUart_times;

void M3399_Uart_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_TypeDefStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA
							|RCC_AHB1Periph_GPIOD
							|RCC_AHB1Periph_GPIOI, ENABLE);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2
						  	|RCC_APB1Periph_UART4
							,ENABLE);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_UART4);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);
		
	//__________________________________________________________________________	

	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOD, &GPIO_InitStructure);


	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOI, &GPIO_InitStructure);

	Hard_Uart_MurtSel(0);
	
	//__________________________________________________________________________	

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

	USART_InitStructure.USART_Mode =  USART_Mode_Tx;
	USART_Init(UART4, &USART_InitStructure);
	USART_Cmd(UART4, ENABLE);	

	NVIC_TypeDefStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_TypeDefStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_TypeDefStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_TypeDefStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_TypeDefStructure);
	
	
	
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_Mode =  USART_Mode_Rx;
	USART_Init(USART2, &USART_InitStructure);
	
	
	NVIC_TypeDefStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_TypeDefStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_TypeDefStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_TypeDefStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_TypeDefStructure);
	
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	USART_Cmd(USART2, ENABLE);	
	
	HardUart_times = 1000;
}

void Hard_Uart_Test_C2tC4(void)
{
	static uint8_t rt;
	uint8_t txf;

	if(USART_GetFlagStatus(USART2,USART_FLAG_ORE))
	{
		rt = USART_ReceiveData(USART2) & 0x0ff;
		txf = 0;
	}
	
	if(USART_GetFlagStatus(USART2,USART_FLAG_RXNE))
	{
		rt = USART_ReceiveData(USART2) & 0x0ff;
		txf = 1;
	}
	else
		txf = 0;
	
	if(txf)
	{
		USART_SendData(UART4,rt);
	}
}

void USART2_IRQHandler()
{
/*	if(Wifi_Rx_flag == 0)
	{
		if(USART_GetITStatus(USART2,USART_IT_RXNE) == SET)
		{
			Wifi_Rx_Buf[Wifi_Rx_num]= USART_ReceiveData(USART2);
			Wifi_Rx_num++;
			if(Wifi_Rx_num > WIFI_MAX_NUM)	
			{
				Wifi_Rx_num = 0;
				Wifi_Rx_flag = 1;
			}
		}
	}
	USART_ClearITPendingBit(USART2,USART_IT_RXNE);*/
	
	
//	if(USART_GetITStatus(uart_prot_buf[p->uart_port], USART_IT_TXE) != RESET)
//	{
//		p->tx_curr++;
//		if(p->tx_curr < p->tx_leng)
//			uart_prot_buf[p->uart_port]->DR=p->tx_buf[p->tx_curr];
//		else
//			USART_ITConfig(uart_prot_buf[p->uart_port], USART_IT_TXE, DISABLE);
//			
//		USART_ClearITPendingBit(uart_prot_buf[p->uart_port], USART_IT_TXE);		
//	}
	
	if(USART_GetITStatus(USART2,USART_IT_RXNE) == SET)
	{
		if(Wifi_Rx_flag == 0)
		{
			canu_wifi_rxtime = gt_get()+10;
			Wifi_Rx_num =0;
			Wifi_Rx_Buf[Wifi_Rx_num]= USART_ReceiveData(USART2);
			Wifi_Rx_flag = 1;
		}
		else if(Wifi_Rx_num < WIFI_MAX_NUM )	
		{
			Wifi_Rx_Buf[++Wifi_Rx_num]= USART_ReceiveData(USART2);	 
		}
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
	}
}

void USART4_TX_Byte(uint8_t data)
{
	USART_SendData(UART4,data);
	while(USART_GetFlagStatus(UART4,USART_FLAG_TXE) == RESET);
}


void UART4_TX_Data(uint8_t *data,uint8_t length)
{
	uint8_t i = 0;
	do
	{
		USART4_TX_Byte(*(data+i));
		i++;
	}		
	while(i<length);
}

void M3399_UART_Tx(uint8_t *MFifo,uint8_t length)
{	uint8_t tp_num = 0;
	while(length)
	{
		if(USART_GetFlagStatus(UART4,USART_FLAG_TXE))
		{
			USART_SendData(UART4,MFifo[tp_num]);
		}
		tp_num++;
		length--;
	}
}




void Hard_Uart_MurtSel(uint8_t c)
{
	if(c)
		GPIO_SetBits(GPIOI,GPIO_Pin_8);
	else
		GPIO_ResetBits(GPIOI,GPIO_Pin_8);
}

void M3399_Uart_While(void)
{
	if(gt_get_sub(HardUart_times) == 0)
	{
		HardUart_times = gt_get()+1000;	
	}
	Hard_Uart_Test_C2tC4();
}



//end file
