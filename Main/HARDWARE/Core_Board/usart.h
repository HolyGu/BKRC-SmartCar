#ifndef __USART_H_
#define __USART_H_
#include "stm32f4xx.h"

#define RX_Length 2
extern uint8_t RX_OK;
extern uint8_t Length_num;
extern uint8_t RX_Data[RX_Length];

void USART1_Configure(uint32_t  baudrate);
void USART_TX_Byte(uint8_t data);
void USART1_TX_Data(uint8_t *data,uint8_t length);
void USART1_TX_String(uint8_t *src);

#endif
