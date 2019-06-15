
#ifndef __POWER_CHECK_H__
#define __POWER_CHECK_H__

#include "sys.h"
void Electricity_Init(void);
void Parameter_Init(void) ;
u16  Get_Electricity(u8 times );
void Power_Check(void);

extern u8 Electric_Buf[2];

#endif //__POWER_CHECK_H__


