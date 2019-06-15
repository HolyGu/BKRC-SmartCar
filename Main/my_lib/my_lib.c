
#include <stdint.h>
#include "stm32f4xx.h"
#define __MY_LIB_C__
#include "my_lib.h"


uint32_t U8ToU32(uint8_t *datas)
{
	//return ((u32)datas[0])|((u32)datas[1]<<8 )|((u32)datas[2]<<16)|((u32)datas[3]<<24);			
	uint32_t temp=0;
	temp=datas[3];
	temp=(temp<<8)|datas[2];
	temp=(temp<<8)|datas[1];
	temp=(temp<<8)|datas[0];
	return temp;
}

void U32ToU8(uint8_t *Buf,uint32_t Datas)
{
	Buf[0]=Datas;
	Buf[1]=Datas>>8;
	Buf[2]=Datas>>16;
	Buf[3]=Datas>>24;
}

uint16_t U8ToU16(uint8_t *datas)
{
	uint32_t temp=0;
	temp=datas[1];
	temp=(temp<<8)|datas[0];
	return temp;
}

uint16_t U8ToU16_Big(uint8_t *datas)
{
	uint32_t temp=0;
	temp=datas[0];
	temp=(temp<<8)|datas[1];
	return temp;
}

void U16ToU8(uint8_t *Buf,uint16_t Datas)
{
	Buf[0]=Datas;
	Buf[1]=Datas>>8;
}


uint8_t MLib_GetSum(uint8_t *p,uint32_t l)
{
	uint8_t Rt = 0;
	while(l--)
		Rt += *p++;
	return Rt;
}

uint16_t MLib_GetShortSum(uint16_t *p,uint32_t dot)
{
	uint16_t Rt = 0;
	while(dot--)
		Rt += *p++;
	return Rt;
}

uint8_t MLib_FindFastBit(uint32_t d)
{
	uint8_t i;
	uint32_t f = 0x01;
	for(i = 0;i<32;i++,f<<=1)
	{
		if(d & f)
			break;
	}
	return i;
}

#if 0
void MLib_Gpio_Bits_Write(GPIO_TypeDef* GPIOx,u16 bits,u16 data)
{
	//GPIO_SetBits(GPIOx,bits & data);
	GPIOx->BSRRL = bits & data;
	//GPIO_ResetBits(GPIOx,bits & (~data));
	GPIOx->BSRRH = bits & (~data);
}
#endif

uint32_t MLib_GetDataSub(uint32_t d1,uint32_t d2)
{
	return (d1 > d2)? d1-d2:d2-d1;
}
	
void MLib_memcpy(void *d,void *s,uint32_t lb)
{
	uint8_t *r,*w;
	if(lb)
	{
		if(d < s)
		{
			r = (uint8_t *)s;
			w = (uint8_t *)d;
			while(lb--)
				*w++ = *r++;
		}
		else
		{
			r = ((uint8_t *)s)+lb-1;
			w = ((uint8_t *)d)+lb-1;
			while(lb--)
				*w-- = *r--;
		}
	}
}

void MLib_memset(void *b,uint8_t d,uint32_t lb)
{
	uint8_t *p = (uint8_t *)b;
	while(lb--)
		*p++ = d;
}

void MLib_memint(void *b,uint8_t d,uint32_t lb)
{
	uint8_t *p = (uint8_t *)b;
	while(lb--)
		*p++ = d++;	
}

int8_t MLib_memcmp(void *b1,void *b2,uint32_t lb)
{
	int8_t Rt = 0;
	uint8_t *s1 = (uint8_t *)b1;
	uint8_t *s2 = (uint8_t *)b2;
	while(lb--)
	{
		if(*s1 != *s2)
		{
			if(*s1 < *s2)
				Rt = -1;
			else
				Rt = 1;
			break;
		}
		s1++;
		s2++;
	}
	return Rt;
}

//end file

