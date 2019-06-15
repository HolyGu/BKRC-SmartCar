
#ifndef __HARD_CAN_H__
#define __HARD_CAN_H__

#ifdef __HARD_CAN_C__
#define GLOBAL
#else
#define GLOBAL extern
#endif

GLOBAL void Hard_Can_Init(void);
GLOBAL void Hard_Can_While(void);

GLOBAL void Hard_Can_SpeedMode(uint8_t f);

#undef GLOBAL

#endif //__HARD_CAN_H__


