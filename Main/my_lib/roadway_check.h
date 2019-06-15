#ifndef __ROADWAY_H
#define __ROADWAY_H
//#include "stm32f4xx.h"

void Roadway_Check(void);
void Go_With_Track_Check(void);
void Roadway_Flag_clean(void);
void Roadway_mp_syn(void);
void Roadway_nav_syn(void);
uint16_t Roadway_mp_Get(void);

extern uint8_t wheel_L_Flag,wheel_R_Flag;
extern uint8_t wheel_Nav_Flag;
extern uint8_t Go_Flag,Back_Flag;
extern uint8_t Track_Flag;
extern uint8_t Back_Track_Falg;
extern int Car_Spend;
extern uint16_t temp_MP;
extern uint16_t temp_Nav;
extern uint8_t Stop_Flag;
extern uint8_t Track_Special_Flag;
extern uint8_t Go_With_Track_Flag;
extern uint8_t RFID_Track_Flag;
extern uint8_t RFID_Write_Flag;
void Control(int L_Spend,int R_Spend);

extern uint8_t Roadway_GoBack_Check(void);
void roadway_check_TimInit(uint16_t arr,uint16_t psc);
#endif


