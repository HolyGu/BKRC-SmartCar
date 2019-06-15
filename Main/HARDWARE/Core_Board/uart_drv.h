
#ifndef __UART_DRV_H__
#define __UART_DRV_H__

#ifdef __UART_DRV_C__
#define GLOBAL
#else
#define GLOBAL extern
#endif

#define CONST_OVERMAX 0x7ffffff0

typedef struct _Uart_Buf_Struct
{
	uint8_t *tx_buf;
	uint32_t tx_size;
	uint32_t tx_leng;
	uint32_t tx_curr;
	uint32_t tx_overs;
	
	uint8_t *rx_buf;
	uint32_t rx_size;
	uint32_t rx_leng;
	uint32_t rx_overs;
	uint32_t rx_times;
	
	uint8_t uart_port;

	uint8_t run_flag;
	
}Uart_Buf_Struct;

GLOBAL Uart_Buf_Struct uart_struct[5];

GLOBAL void Uart_Struct_Clear(Uart_Buf_Struct *p);
GLOBAL void Uart_Init(void);

GLOBAL void Uart_Irq(Uart_Buf_Struct *p);
GLOBAL void UartTx_Clear(Uart_Buf_Struct *p);
GLOBAL void UartTx_AddStr(uint8_t *b,uint8_t ml,Uart_Buf_Struct *p);
GLOBAL void UartTx_AddBuf(uint8_t *b,uint8_t ml,Uart_Buf_Struct *p);
GLOBAL void UartTx_AddChar(uint8_t d,Uart_Buf_Struct *p);
GLOBAL void UartTx_Start(Uart_Buf_Struct *p);
GLOBAL   uint8_t UartTx_EndCheck(Uart_Buf_Struct *p);

GLOBAL void UartRx_Clear(Uart_Buf_Struct *p);
//GLOBAL  uint32_t UartRx_GetLen(Uart_Buf_Struct *p);
GLOBAL  uint32_t UartRx_GetData(uint8_t *pr,uint16_t ml,Uart_Buf_Struct *p);
//GLOBAL   uint8_t UartRx_GetChar(uint32_t offset,Uart_Buf_Struct *p);
GLOBAL void UartRx_TimeCb(void);
GLOBAL  uint32_t UartRx_GetLostTime(Uart_Buf_Struct *p);

#define UartRx_GetBuf(p)	(p)->rx_buf
#define UartRx_GetLen(p)	(p)->rx_leng
#define UartRx_GetChar(o,p)	(p)->rx_buf[o]
//#define UartRx_GetLostTime(p)	(p)->rx_times
#define UartRx_GetBufSize(p)	(p)->rx_size

#define UartTx_GetBuf(p)	(p)->tx_buf
#define UartTx_GetLen(p)	(p)->tx_leng

GLOBAL void USART_Config(void);

GLOBAL void UartDrv_BaudRate_Set(USART_TypeDef* USARTx, uint32_t USART_BaudRate);
GLOBAL uint32_t UartDrv_BaudRate_Get(USART_TypeDef* USARTx);





//end file





















#undef GLOBAL
#endif //__UART_DRV_H__





