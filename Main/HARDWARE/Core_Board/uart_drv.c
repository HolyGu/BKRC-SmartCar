
#include <stdint.h>
#include <string.h>
#include "stm32lib.h"
#include "delay.h"
#include "my_lib.h"

#define __UART_DRV_C__
#include "uart_drv.h"

static volatile uint8_t RxData;
static volatile uint8_t Uart_RxTemp;

#define DEF_BPS	115200	//256000	//	//115200


USART_TypeDef* uart_prot_buf[]=
{
	NULL,
	USART1,
	USART2,
	USART3,
	UART4,
	UART5,
};

//void USART1_IRQHandler(void)
//{
//	if(uart_struct[0].run_flag)
//		Uart_Irq(&uart_struct[0]);
//	else
//		USART_Cmd(USART1, DISABLE);
//}

/*
void USART2_IRQHandler(void)
{
	if(uart_struct[1].run_flag)
		Uart_Irq(&uart_struct[1]);
	else
		USART_Cmd(USART2, DISABLE);
}
*/

void Uart_Struct_Clear(Uart_Buf_Struct *p)
{
	memset(p,0,sizeof(Uart_Buf_Struct));
}

void Uart_Init(void)
{
	memset(uart_struct,0,sizeof(uart_struct));
}

void Uart_Irq(Uart_Buf_Struct *p)
{
	
	if(USART_GetITStatus(uart_prot_buf[p->uart_port], USART_IT_TXE) != RESET)
	{
		p->tx_curr++;
		if(p->tx_curr < p->tx_leng)
			uart_prot_buf[p->uart_port]->DR=p->tx_buf[p->tx_curr];
		else
			USART_ITConfig(uart_prot_buf[p->uart_port], USART_IT_TXE, DISABLE);
			
		USART_ClearITPendingBit(uart_prot_buf[p->uart_port], USART_IT_TXE);		
	}
	
	if(USART_GetITStatus(uart_prot_buf[p->uart_port], USART_IT_RXNE) != RESET)
	{
		p->rx_times = gt_get();
		RxData = uart_prot_buf[p->uart_port]->DR;
		USART_ClearITPendingBit(uart_prot_buf[p->uart_port], USART_IT_RXNE);
		if(p->rx_leng < p->rx_size)
			p->rx_buf[p->rx_leng++] = RxData;
		else if(p->rx_overs < CONST_OVERMAX)
			p->rx_overs++;

	}
	
	if(USART_GetFlagStatus(uart_prot_buf[p->uart_port], USART_FLAG_ORE) != RESET)
	{
		Uart_RxTemp = uart_prot_buf[p->uart_port]->DR;
	}
	
}

void UartTx_Clear(Uart_Buf_Struct *p)
{
	USART_ITConfig(uart_prot_buf[p->uart_port], USART_IT_TXE, DISABLE);
	p->tx_leng = 0;
	p->tx_curr = 0;
	p->tx_overs = 0;
	USART_ClearITPendingBit(uart_prot_buf[p->uart_port], USART_IT_TXE);
}

void UartTx_AddStr(uint8_t *b,uint8_t ml,Uart_Buf_Struct *p)
{
	uint8_t i;
	for(i = 0;i<ml;i++)
	{
		if(b[i] == 0)
			break;
		if(p->tx_leng<p->tx_size)
			p->tx_buf[p->tx_leng++] = b[i];
		else if(p->tx_overs < CONST_OVERMAX)
			p->tx_overs++;
	}
}

void UartTx_AddBuf(uint8_t *b,uint8_t ml,Uart_Buf_Struct *p)
{
	uint8_t i;
	for(i = 0;i<ml;i++)
	{
		if(p->tx_leng<p->tx_size)
			p->tx_buf[p->tx_leng++] = b[i];
		else if(p->tx_overs < CONST_OVERMAX)
			p->tx_overs++;
	}
}

void UartTx_AddChar(uint8_t d,Uart_Buf_Struct *p)
{
	if(p->tx_leng < p->tx_size)
		p->tx_buf[p->tx_leng++] = d;
	else if(p->tx_overs < CONST_OVERMAX)
		p->tx_overs++;
}


void UartTx_Start(Uart_Buf_Struct *p)
{
	p->tx_curr = 0;
	uart_prot_buf[p->uart_port]->DR=p->tx_buf[0];
	USART_ITConfig(uart_prot_buf[p->uart_port], USART_IT_TXE, ENABLE);

}

uint8_t UartTx_EndCheck(Uart_Buf_Struct *p)
{
	return (p->tx_curr == p->tx_leng)? 1:0;
}

void UartRx_Clear(Uart_Buf_Struct *p)
{
	USART_ITConfig(uart_prot_buf[p->uart_port], USART_IT_RXNE, DISABLE);
	p->rx_leng = 0;
	p->rx_overs = 0;
	p->rx_times = 0;
	USART_ITConfig(uart_prot_buf[p->uart_port], USART_IT_RXNE, ENABLE);
}



uint32_t UartRx_GetData(uint8_t *pr,uint16_t ml,Uart_Buf_Struct *p)
{
	if(ml > p->rx_leng)
		ml = p->rx_leng;
	if((pr != NULL)&&(ml != 0))
		memcpy(pr,p->rx_buf,ml);
	return ml;
}

uint32_t UartRx_GetLostTime(Uart_Buf_Struct *p)
{
	return MLib_GetDataSub(p->rx_times,gt_get());
}






//______________________________________________________________________________



