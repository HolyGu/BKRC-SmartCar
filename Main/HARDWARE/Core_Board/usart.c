#include "stm32f4xx.h"
#include "usart.h"
#include "Timer.h"

uint8_t RX_OK = RESET;
uint8_t Length_num = 0;
uint8_t RX_Data[RX_Length];



void USART1_Configure(uint32_t  baudrate)
{
	GPIO_InitTypeDef  GPIO_TypeDefStructure;
	USART_InitTypeDef USART_TypeDefStructure;
	NVIC_InitTypeDef  NVIC_TypeDefStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
	
	//PA9-Tx
	GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;
	GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_AF;		//复用功能
	GPIO_TypeDefStructure.GPIO_OType = GPIO_OType_PP; //推挽输出
	GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;   //上拉
	GPIO_TypeDefStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA,&GPIO_TypeDefStructure);
	
	USART_TypeDefStructure.USART_BaudRate = baudrate;					       //波特率
	USART_TypeDefStructure.USART_HardwareFlowControl = 				       //无硬件控制流
												 USART_HardwareFlowControl_None;  
	USART_TypeDefStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx; //接收与发送模式
	USART_TypeDefStructure.USART_Parity = USART_Parity_No; 		       //无校验位
	USART_TypeDefStructure.USART_StopBits = USART_StopBits_1;        //停止位1
	USART_TypeDefStructure.USART_WordLength = USART_WordLength_8b;   //数据位8位
	USART_Init(USART1,&USART_TypeDefStructure);
	
	NVIC_TypeDefStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_TypeDefStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_TypeDefStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_TypeDefStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_TypeDefStructure);
	
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	USART_Cmd(USART1,ENABLE);
	
}

void USART2_Configure()
{


}

void UART4_Configure(uint32_t  baudrate)
{
	GPIO_InitTypeDef  GPIO_TypeDefStructure;
	USART_InitTypeDef USART_TypeDefStructure;
	NVIC_InitTypeDef  NVIC_TypeDefStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
	
	//PA9-Tx
	GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;
	GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_AF;		//复用功能
	GPIO_TypeDefStructure.GPIO_OType = GPIO_OType_PP; //推挽输出
	GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;   //上拉
	GPIO_TypeDefStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA,&GPIO_TypeDefStructure);
	
	USART_TypeDefStructure.USART_BaudRate = baudrate;					       //波特率
	USART_TypeDefStructure.USART_HardwareFlowControl = 				       //无硬件控制流
												 USART_HardwareFlowControl_None;  
	USART_TypeDefStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx; //接收与发送模式
	USART_TypeDefStructure.USART_Parity = USART_Parity_No; 		       //无校验位
	USART_TypeDefStructure.USART_StopBits = USART_StopBits_1;        //停止位1
	USART_TypeDefStructure.USART_WordLength = USART_WordLength_8b;   //数据位8位
	USART_Init(USART1,&USART_TypeDefStructure);
	
	NVIC_TypeDefStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_TypeDefStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_TypeDefStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_TypeDefStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_TypeDefStructure);
	
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	USART_Cmd(USART1,ENABLE);


}



void USART1_IRQHandler()
{
	if(RX_OK == RESET)
	{
		if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)
		{
				
				RX_Data[Length_num] = USART_ReceiveData(USART1);
				Length_num++;
			    if(Length_num >= RX_Length)
				{
					Length_num = 0;
					RX_OK = SET;
				}
				USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		}
	}
}


void USART_TX_Byte(uint8_t data)
{
	USART_SendData(USART1,data);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
}


void USART1_TX_Data(uint8_t *data,uint8_t length)
{
	uint8_t i = 0;
	do
	{
		USART_TX_Byte(*(data+i));
		i++;
	}		
	while(i<length);
}

void USART1_TX_String(uint8_t *src)
{
	uint8_t i = 0;
	do
	{
		USART_TX_Byte(*(src+i));
		i++;
	}
	while(*(src+i) != '\0');
}



