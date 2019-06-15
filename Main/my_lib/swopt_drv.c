
#include <stdint.h>

//#include "pt_drv.h"

#include "fifo_drv.h"

#define __SWOPT_DRV_C__
#include "swopt_drv.h"

/*********************************************************************
*
*       Defines for Cortex-M debug unit
*/
#define ITM_STIM_U32 (*(volatile unsigned int*)0xE0000000)    // Stimulus Port Register word acces
#define ITM_STIM_U8  (*(volatile         char*)0xE0000000)    // Stimulus Port Register byte acces
#define ITM_ENA      (*(volatile unsigned int*)0xE0000E00)    // Trace Enable Ports Register
#define ITM_TCR      (*(volatile unsigned int*)0xE0000E80)    // Trace control register

uint8_t SwoPt_c;
uint8_t SwoPt_f;

#define SWOPT_BUF_SIZE	100
uint8_t SwoPt_Buf[SWOPT_BUF_SIZE];
Fifo_Drv_Struct SwoPt_Fifo;

void SwoPT_Init(void)
{
	SwoPt_Fifo.buf = SwoPt_Buf;
	SwoPt_Fifo.ml = SWOPT_BUF_SIZE;
	FifoDrv_Init(&SwoPt_Fifo,0);
}

uint8_t SwoPt_EnCheck(void)
{
	if ((ITM_TCR & 1) == 0) {
		return 0;
	}

	if ((ITM_ENA & 1) == 0) {
		return 0;
	}		
	return 1;
}


void SwoPt_while(void)
{
	if(SwoPt_f == 0)
		SwoPt_f = FifoDrv_ReadOne(&SwoPt_Fifo,&SwoPt_c);
	
	if(SwoPt_f)
	{
		if(ITM_STIM_U8 & 1)
		{
			ITM_STIM_U8 = SwoPt_c;
			SwoPt_f = 0;
		}		
	}
}

void SwoPt_PrintChar(char c)
{
	if(SwoPt_EnCheck())
		FifoDrv_WriteOne(&SwoPt_Fifo,(uint8_t)c);
}

void SwoPt_PrintString(const char *s)
{
	if(SwoPt_EnCheck())
	{
		while (*s) 
			FifoDrv_WriteOne(&SwoPt_Fifo,(uint8_t)(*s++));
	}
}

void SwoPt_PrintBuf(void *s,unsigned long ml,unsigned char endcheck)
{
	if(SwoPt_EnCheck())
	{
		char *p = s;
		while(ml)
		{
			if(endcheck)
				if(*p == 0)
					break;
			//SWO_PrintChar(*p++);
			FifoDrv_WriteOne(&SwoPt_Fifo,(uint8_t)(*p++));
			ml--;
		}
	}
}

void SWO_PrintChar  (char c)
{
	SwoPt_PrintChar(c);
	
}
void SWO_PrintString(const char *s)
{
	SwoPt_PrintString(s);
}



//end file

