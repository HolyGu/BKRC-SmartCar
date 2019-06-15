

#ifndef __SWOPT_DRV_H__
#define __SWOPT_DRV_H__

#ifdef __SWOPT_DRV_C__
#define GLOBAL
#else
#define GLOBAL extern
#endif

GLOBAL void SwoPT_Init(void);
GLOBAL void SwoPt_while(void);
GLOBAL void SwoPt_PrintChar(char c);
GLOBAL void SwoPt_PrintString(const char *s);
GLOBAL void SwoPt_PrintBuf(void *s,unsigned long ml,unsigned char endcheck);


#undef GLOBAL

#endif //__SWOPT_DRV_H__

