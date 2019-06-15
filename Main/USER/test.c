
#include <stdio.h>
#include "stm32f4xx.h"
#include "delay.h"

int32_t test_times;
int32_t test_cmp;
uint32_t test_upmp_times;

extern uint16_t CanHost_Mp;
extern uint16_t CanHost_Mp;
/*
void set_Test_Times(void)
{
	test_times = gt_get();
	test_cmp = CanHost_Mp;
	test_upmp_times = 0;
}

void up_Test_Upmp(void)
{
	test_upmp_times++;
}

uint32_t getLost_Test_Times(void)
{
	test_times = gt_get() - test_times;
	test_cmp = CanHost_Mp - test_cmp;
	printf("t = %d, m = %d, up = %u\n",test_times,test_cmp,test_upmp_times);
}
*/

//end file


