#ifndef __3399_USART_H
#define __3399_USART_H
#include "stm32f4xx.h"

void M3399_Uart_Init(void);
void Hard_Uart_Test_C2tC4(void);
void Hard_Uart_MurtSel(uint8_t c);
void M3399_Uart_While(void);
void M3399_UART_Rx(uint8_t *MFifo,uint8_t length);
void M3399_UART_Tx(uint8_t *MFifo,uint8_t length);
void UART4_TX_Data(uint8_t *data,uint8_t length);

#endif
