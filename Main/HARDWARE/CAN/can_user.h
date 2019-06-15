#include "stm32f4xx.h"
#ifndef __CAN_USER_H__
#define __CAN_USER_H__

#ifdef __CAN_USER_C__
#define GLOBAL
#else
#define GLOBAL extern
#endif

extern u8 Go_Sp;
extern u8 Back_Sp;
extern u8 Track_Sp;
extern u8 Whell_Sp;
extern u16 Go_Mp;
extern u16 Back_Mp;
extern u8 WiFi_Data[12];
extern u8 AGV_CT;

extern u8 TEST_AGV[1];

extern u16 csb_1;
extern u16 csb_2;
extern u16 csb_3;

extern uint8_t Host_AGV_Return_Flag;
extern uint8_t AGV_data_Falg;
extern uint32_t canu_wifi_rxtime;
extern uint32_t canu_zibe_rxtime;
extern uint32_t Full_Automatic_Start;
GLOBAL void Canuser_Init(void);
GLOBAL void Canuser_main(void);

GLOBAL void Can_WifiRx_Save(uint8_t res);
GLOBAL void Can_ZigBeeRx_Save(uint8_t res);

void Send_Android_Info(uint8_t *data);
void Can_WifiRx_Check(void);
void Can_ZigBeeRx_Check(void);
void Red_Card_Track(uint8_t sp);
extern uint8_t ETC_open_Flag;
extern void ETC_move(uint8_t sp,uint16_t mp);
void Full_End_Car(void);
void Full_Automatic(void);
void Full_Automatic_GO_With_Track(uint16_t sp,uint16_t en);
void Full_Automatic_GO(uint16_t sp,uint16_t en);
void Full_Automatic_Track(uint16_t sp);

void Full_Automatic_Send_Android(u16 data);
void Full_Automatic_Back(uint16_t sp,uint16_t en);
void Full_Automatic_Left(uint16_t sp);
void Full_Automatic_Right(uint16_t sp);
void Full_Automatic_GO_And_Track(uint16_t go_sp,uint16_t go_en,uint16_t track_sp);
void Full_Automatic_RFID_Track(uint16_t sp);
void Full_Light_Goal(u8 End_Goal);
void Full_Light_Down(void);
void Full_Right45(void);
void Full_Left45(void);
void Full_Right90(void);
void Full_Left90(void);
void Full_Automatic_Special(void);
void Full_WiFi_Data(void);
void Full_RGB_Light(void);
//void Full_Automatic_Wheel(uint16_t sp,uint16_t fx);
void Car_3D(u8 Run);
void Full_WiFi_Clean(void);
void Full_AGV_Ultrasonic(void);
void AGV_Stop(void);
void Get_Car(void);
void Send_Car(void);
void Send_Display(void);
void Full_Ultrasonic(u8 X);
void usMBCRC16( u8 * pucFrame, u8 usLen,u8 * result );
void Full_FHT();


#undef GLOBAL


#endif //__CAN_USER_H__

