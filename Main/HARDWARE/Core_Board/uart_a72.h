

#ifndef __UART_A72_H__
#define __UART_A72_H__

#ifdef __UART_A72_C__
#define GLOBAL
#else
#define GLOBAL extern
#endif

GLOBAL void UartA72_Init(void);
GLOBAL void UartA72_TxClear(void);
GLOBAL void UartA72_TxAddChar(uint8_t d);
GLOBAL void UartA72_TxWhileCheck(void);
GLOBAL void UartA72_TxAddStr(uint8_t *p,uint32_t l);
GLOBAL void UartA72_TxStart(void);



#undef GLOBAL



#endif //__UART_A72_H__

