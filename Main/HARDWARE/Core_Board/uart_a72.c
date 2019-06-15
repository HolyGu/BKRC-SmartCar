

#include "stm32f4xx.h"
//#include "fifo_drv.h"
#include "uart_drv.h"

#define __UART_A72_C__
#include "uart_a72.h"

#define PUART_A72TX	(&(uart_struct[4]))

uint8_t Uart_A72_TxEmpty;

//Fifo_Drv_Struct Fifo_UartA72_Tx;
#define FIFOSIZE_A72_TX	50
uint8_t FifoBuf_A72_Tx[FIFOSIZE_A72_TX];

#define FIFOSIZE_A72_SRX 2
uint8_t FifuBuf_A72_SRx[FIFOSIZE_A72_SRX];

//Fifo_Drv_Struct Fifo_UartA72_Rx;
//#define FIFOSIZE_A72_RX	50
//uint8_t FifoBuf_A72_Rx[FIFOSIZE_A72_RX];

void UartA72_Init(void)
{
//	Fifo_UartA72_Tx.ml = FIFOSIZE_A72_TX;
//	Fifo_UartA72_Tx.buf = FifoBuf_A72_Tx;
//	FifoDrv_Init(&Fifo_UartA72_Tx,0);
	
	
//typedef struct _Uart_Buf_Struct
//{
//	uint8_t *tx_buf;
//	uint32_t tx_size;
//	uint32_t tx_leng;
//	uint32_t tx_curr;
//	uint32_t tx_overs;
//	
//	uint8_t *rx_buf;
//	uint32_t rx_size;
//	uint32_t rx_leng;
//	uint32_t rx_overs;
//	uint32_t rx_times;
//	
//	uint8_t uart_port;

//	uint8_t run_flag;
//	
//}Uart_Buf_Struct;
	PUART_A72TX->tx_buf = FifoBuf_A72_Tx;
	PUART_A72TX->tx_size = FIFOSIZE_A72_TX;
	PUART_A72TX->tx_leng =  0;
	PUART_A72TX->tx_curr = 0;
	PUART_A72TX->tx_overs = 0;

	PUART_A72TX->rx_buf = FifuBuf_A72_SRx;
	PUART_A72TX->rx_size = FIFOSIZE_A72_SRX;
	PUART_A72TX->rx_leng =  0;
	PUART_A72TX->rx_overs = 0;
	PUART_A72TX->rx_times = 0;
	
	PUART_A72TX->uart_port = 4;
	
	Uart_A72_TxEmpty = 0;
	
//	Fifo_UartA72_Rx.ml = FIFOSIZE_A72_RX;
//	Fifo_UartA72_Rx.buf = FifoBuf_A72_Rx;
//	FifoDrv_Init(&Fifo_UartA72_Rx,0);
}

void UartA72_TxClear(void)
{
	UartTx_Clear(PUART_A72TX);
	//FifoDrv_Init(&Fifo_UartA72_Tx,0);
	Uart_A72_TxEmpty = 0;
}

void UartA72_TxAddChar(uint8_t d)
{
	UartTx_AddChar(d,PUART_A72TX);
	//FifoDrv_WriteOne(&Fifo_UartA72_Tx,d);
	Uart_A72_TxEmpty = 1;
}

void UartA72_TxAddStr(uint8_t *p,uint32_t l)
{
	UartTx_AddBuf(p,l,PUART_A72TX);
	//FifoDrv_BufWrite(&Fifo_UartA72_Tx,p,l);
	if(l)
		Uart_A72_TxEmpty = 1;
}

void UartA72_TxStart(void)
{
	UartTx_Start(PUART_A72TX);
}

void UART4_IRQHandler(void)
{
	Uart_Irq(PUART_A72TX);
	
}


void UartA72_TxWhileCheck(void)
{
//	if(Uart_A72_TxEmpty)
//	{
//		if(USART_GetFlagStatus(UART4,USART_FLAG_TXE) != RESET)
//		{
//			uint8_t temp;
//			if(FifoDrv_ReadOne(&Fifo_UartA72_Tx,&temp))
//			{
//				USART_SendData(UART4,temp);
//			}
//			else
//				Uart_A72_TxEmpty = 0;
//		}
//	}

	
}







//end file
