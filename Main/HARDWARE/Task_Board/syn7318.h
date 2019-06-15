#ifndef __SYN7318_H
#define __SYN7318_H
#include "stm32f4xx.h"
#include "sys.h"

#define SYN7318RESET PBout(5)

void SYN7318_Init(void);
void SYN_TTS(uint8_t *Pst);
void SYN7318_Test( void);  // 开启语音测试
void SYN_7318_One_test(u8 mode,u8 num);  // mode 模式测试 1 随机指令 0 指定指令
	
#endif

