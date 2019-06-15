

#ifndef __FIFO_DRV_H__
#define __FIFO_DRV_H__

#ifdef __FIFO_DRV_C__
#define GLOBAL
#else
#define GLOBAL extern
#endif

                /* POINTER PROPERTIES */
#define _NULL           0       /* 0L if pointer same as long */


typedef struct _Fifo_Drv_Struct
{
	uint32_t ml;
	uint32_t rp;
	uint32_t wp;	
	uint8_t *buf;
}Fifo_Drv_Struct;

GLOBAL void FifoDrv_Init(Fifo_Drv_Struct *p,uint8_t c);
GLOBAL uint8_t FifoDrv_CheckWriteEn(Fifo_Drv_Struct *p);
GLOBAL uint8_t FifoDrv_WriteOne(Fifo_Drv_Struct *p,uint8_t d);
GLOBAL uint8_t FifoDrv_CheckReadEn(Fifo_Drv_Struct *p);
GLOBAL uint8_t FifoDrv_ReadOne(Fifo_Drv_Struct *p,uint8_t *d);

GLOBAL uint32_t FifoDrv_BufWrite(Fifo_Drv_Struct *p,uint8_t *buf,uint32_t l);
GLOBAL uint32_t FifoDrv_BufRead(Fifo_Drv_Struct *p,uint8_t *buf,uint32_t ml);

#undef GLOBAL

#endif //__FIFO_DRV_H__

