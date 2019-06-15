#include <stdint.h>
#include <string.h>

#include "fifo_drv.h"

void FifoDrv_Init(Fifo_Drv_Struct *p,uint8_t c)
{
	if(p != _NULL)
	{
		if(p->ml > 1)
		{
			p->wp = 0;
			p->rp =  p->ml-1;
			memset(p->buf,c,p->ml);
		}
	}	
}

uint8_t FifoDrv_StructCheck(Fifo_Drv_Struct *p)
{
	uint8_t Rt = 0;
	if(p != _NULL)
	{
		if(p->ml > 1)
		{
			if((p->buf != _NULL)
			   &&(p->wp < p->ml)
				&&(p->rp < p->ml)
					)
				Rt = 1;
		}
	}
	return Rt;
}

uint8_t FifoDrv_CheckWriteEn(Fifo_Drv_Struct *p)
{
	uint8_t Rt = 0;
	if(FifoDrv_StructCheck(p))
	{
		if(p->wp != p->rp)
			Rt = 1;
	}
	return Rt;
}


uint8_t FifoDrv_WriteOne(Fifo_Drv_Struct *p,uint8_t d)
{
	uint8_t Rt = 0;
	if(FifoDrv_CheckWriteEn(p))
	{
		p->buf[p->wp++] = d;
		if(p->wp >= p->ml)
			p->wp = 0;
		Rt = 1;
	}
	return Rt;
}

uint8_t FifoDrv_CheckReadEn(Fifo_Drv_Struct *p)
{
	uint8_t Rt = 0;
	if(FifoDrv_StructCheck(p))
	{
		//if(p->wp != p->rp)
		//	Rt = 1;
		if(p->rp == p->ml-1)
		{
			if(p->wp != 0)
				Rt = 1;
		}
		else if(p->wp != (p->rp+1))
			Rt = 1;
	}
	return Rt;
}

uint8_t FifoDrv_ReadOne(Fifo_Drv_Struct *p,uint8_t *d)
{
	uint8_t Rt = 0;
	if(d != _NULL)
	{
		if(FifoDrv_CheckReadEn(p))
		{
			if(++p->rp >= p->ml)
				p->rp = 0;
			*d = p->buf[p->rp];
			Rt = 1;
		}
	}
	return Rt;
}

uint32_t FifoDrv_BufWrite(Fifo_Drv_Struct *p,uint8_t *buf,uint32_t l)
{
	uint32_t Rt = 0;
	while(l--)
	{
		if(FifoDrv_WriteOne(p,buf[Rt]) == 0)
			break;
		Rt++;
	}
	return Rt;
}

uint32_t FifoDrv_BufRead(Fifo_Drv_Struct *p,uint8_t *buf,uint32_t ml)
{
	uint32_t Rt = 0;
	if(buf != _NULL)
	{
		while(ml--)
		{
			if(FifoDrv_ReadOne(p,buf+Rt) == 0)
				break;
			Rt++;
		}
		
	}
	return Rt;
	
}

void FifoDrv_pClear(Fifo_Drv_Struct *p)
{
	if(FifoDrv_StructCheck(p))
	{
		p->wp = 0;
		p->rp =  p->ml-1;
	}
}

uint32_t FifoDrv_GetDataLeng(Fifo_Drv_Struct *p)
{
	uint32_t Rt = 0;
	if(FifoDrv_StructCheck(p))
	{
		
		
		
	}
	return Rt;
}

//end file

