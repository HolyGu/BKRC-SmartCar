
#ifndef __MY_LIB_H__
#define __MY_LIB_H__

#ifdef __MY_LIB_C__
#define GLOBAL
#else
#define GLOBAL extern
#endif

GLOBAL uint8_t MLib_GetSum(uint8_t *p,uint32_t l);
GLOBAL uint16_t MLib_GetShortSum(uint16_t *p,uint32_t dot);
GLOBAL uint8_t MLib_FindFastBit(uint32_t d);
GLOBAL void MLib_Gpio_Bits_Write(GPIO_TypeDef* GPIOx,u16 bits,u16 data);


#ifndef BIT_SUM
#define BIT_SUM
#define B0	1
#define B1	2
#define B2	4
#define B3	8
#define B4	16
#define B5	32
#define B6	64
#define B7	128
#define B8	256
#define B9	512
#define B10	1024
#define B11	2048
#define B12	4096
#define B13	8192
#define B14	16384
#define B15	32768
#define B16 65536
#define B17 131072
#define B18 262144
#define B19 524288
#define B20 1048576
#define B21	2097152
#define B22 4194304
#define B23 8388608
#define B24	16777216
#define B25 33554432
#define B26 67108864
#define B27 134217728
#define B28 268435456
#define B29 536870912
#define B30 1073741824
#define B31 2147483648u
#endif

#ifdef __MY_LIB_C__
const uint32_t bit_tab[]={
	B0 ,B1 ,B2 ,B3 ,B4 ,B5 ,B6 ,B7 ,
	B8 ,B9 ,B10,B11,B12,B13,B14,B15,
	B16,B17,B18,B19,B20,B21,B22,B23,
	B24,B25,B26,B27,B28,B29,B30,B31
};

#define ANDB(d)	(~(uint32_t)d)

const uint32_t bit_and_tab[]={
	ANDB(B0) ,ANDB(B1) ,ANDB(B2) ,ANDB(B3) ,
	ANDB(B4) ,ANDB(B5) ,ANDB(B6) ,ANDB(B7) ,
	ANDB(B8) ,ANDB(B9) ,ANDB(B10),ANDB(B11),
	ANDB(B12),ANDB(B13),ANDB(B14),ANDB(B15),
	ANDB(B16),ANDB(B16),ANDB(B18),ANDB(B19),
	ANDB(B20),ANDB(B21),ANDB(B22),ANDB(B23),
	ANDB(B24),ANDB(B25),ANDB(B26),ANDB(B27),
	ANDB(B28),ANDB(B29),ANDB(B30),ANDB(B31)
};

#else
extern const uint32_t bit_tab[32];
extern const uint32_t bit_and_tab[32];
#endif

#define OBIT(b)	((0x00000001)<<(b))	


#ifdef __MY_LIB_C__
const uint8_t bit4_lowhig[16] = {
	0x00,0x08,0x04,0x0c,0x02,0x0a,0x06,0x0e,
	0x01,0x09,0x05,0x0d,0x03,0x0b,0x07,0x0f
};

#else
extern const uint8_t bit4_lowhig[16];
#endif

GLOBAL uint32_t U8ToU32(uint8_t *datas);
GLOBAL void U32ToU8(uint8_t *Buf,uint32_t Datas);

GLOBAL uint16_t U8ToU16(uint8_t *datas);
GLOBAL void U16ToU8(uint8_t *Buf,uint16_t Datas);

GLOBAL uint16_t U8ToU16_Big(uint8_t *datas);

GLOBAL uint32_t MLib_GetDataSub(uint32_t d1,uint32_t d2);
GLOBAL void MLib_memcpy(void *d,void *s,uint32_t lb);

GLOBAL void MLib_memset(void *b,uint8_t d,uint32_t lb);
GLOBAL void MLib_memint(void *b,uint8_t d,uint32_t lb);
GLOBAL int8_t MLib_memcmp(void *b1,void *b2,uint32_t lb);

#undef GLOBAL
#endif //__MY_LIB_H__

