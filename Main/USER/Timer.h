#ifndef __TIMER_H__
#define __TIMER_H__


extern volatile uint32_t global_times;
extern volatile uint32_t delay_ms_const;

void Timer_Init(uint16_t arr,uint16_t psc);
u32 gt_get_sub(u32 c);
u32 gt_get(void);

#endif


