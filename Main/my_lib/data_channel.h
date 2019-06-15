
#ifndef __DATA_CHANNEL_H__
#define __DATA_CHANNEL_H__
#include "stm32f4xx.h"

#ifdef __DATA_CHANNEL_C__
#define GLOBAL
#else
#define GLOBAL extern
#endif

#define  ZIGB_RX_MAX    200
#define  WIFI_MAX_NUM   200

GLOBAL u8 Wifi_Rx_Buf[ WIFI_MAX_NUM ];
GLOBAL u8 Zigb_Rx_Buf[ ZIGB_RX_MAX ];


GLOBAL u8 Wifi_Rx_num ;
GLOBAL u8 Wifi_Rx_flag ;  //接收完成标志位
GLOBAL u8 Zigbee_Rx_num ;
GLOBAL u8 Zigbee_Rx_flag ;  //接收完成标志位

GLOBAL void Wifi_data_Receive( u8 res);
GLOBAL void Zigbee_data_Receive( u8 res);

void Timer4_Init(u16 arr,u16 psc);


#undef GLOBAL

#endif //__DATA_CHANNEL_H__

