#include "stm32f4xx.h"
#include "sys.h"

#ifndef __ULTRASONIC_H
#define __ULTRASONIC_H

#define INC PAout(15)

void Ultrasonic_Init(void);
void Ultrasonic_Ranging(void);

extern uint16_t dis;

#endif




