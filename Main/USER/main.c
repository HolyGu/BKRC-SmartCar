/**
�������ƣ�2018�����ۺϳ���			
�޸�ʱ�䣺2018.3.20
*/
#include <stdio.h>
#include "stm32f4xx.h"
#include "delay.h"
#include "infrared.h"
#include "cba.h"
#include "ultrasonic.h"
#include "canp_hostcom.h"
#include "hard_can.h"
#include "bh1750.h"
#include "syn7318.h"
#include "power_check.h"
#include "can_user.h"
#include "data_base.h"
#include "roadway_check.h"
#include "tba.h"
#include "data_base.h"
#include "3399_uart.h"
#include "swopt_drv.h"
#include "uart_a72.h"
#include "Can_check.h"
#include "rc522.h"
#include "delay.h"
#include "can_user.h"
#include "Timer.h"
#include "data_channel.h"

RCC_ClocksTypeDef RCC_Clocks;

uint16_t main_cont;

static u8 YY_TEST [8]={0x55,0x0B,0x20,0x35,0x47,0x39,0xD5,0xbb};

/**
�������ܣ�Ӳ����ʼ��
��    ������
�� �� ֵ����
*/
void Hardware_Init()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);					//�жϷ���
	SwoPT_Init();

	delay_init(168);
	
	
	Tba_Init();														//������ʼ��
	Infrared_Init();												//�����ʼ��
	Cba_Init();														//���İ��ʼ��
	Ultrasonic_Init();												//��������ʼ��
	Hard_Can_Init();												//CAN���߳�ʼ��
	BH1750_Configure();												//BH1750��ʼ������
	SYN7318_Init();													//����ʶ���ʼ��
	Electricity_Init();												//��������ʼ��
	M3399_Uart_Init();												//M3399��ʼ��
	UartA72_Init();
	Can_check_Init(83,999);											//CAN���߶�ʱ����ʼ��
	roadway_check_TimInit(83,99);									//·�����
	Timer_Init(83,999);												//��������ͨѶʱ��֡
	
}

/**
�������ܣ��������
��    ������
�� �� ֵ����
*/
void KEY_Check()
{
	int Ls = 0,Rs = 0;
	if(S1 == 0)
	{
		delay_ms(10);
		if(S1 == 0)
		{
			LED1 = 1;



			while(LED1)
			{
			TFT_Time(2);


			}
		}
	}
	if(S2 == 0)
	{
		delay_ms(10);
		if(S2 == 0)
		{
			LED2 = !LED2;
				  AGV_CT = 2;
					TEST_AGV[0] = 1;
					Full_Automatic_Start = 1;
		}
	}
	if(S3 == 0)
	{
		delay_ms(10);
		if(S3 == 0)
		{
				  AGV_CT = 3;
					TEST_AGV[0] = 1;
					Full_Automatic_Start = 1;
		}
	}
	
	if(S4 == 0)
	{
		delay_ms(10);
		if(S4 == 0)
		{
				  AGV_CT = 4;
					TEST_AGV[0] = 1;
					Full_Automatic_Start = 1;
		}
	}
}

/**
�������ܣ�CPU ִ�����ڼ��
��    ������
�� �� ֵ����
*/
static uint32_t HCpuP_Lost_times;
static uint32_t HCpuP_Lost_sper;

/**
�������ܣ�CPU���
��    ������
�� �� ֵ����
*/

void HCpuP_Lost_while(void)
{
	if(gt_get_sub(HCpuP_Lost_times))
		HCpuP_Lost_sper++;
	else
	{
		uint32_t t;
		t = HCpuP_Lost_sper / 1000;
		printf("%u",t);
		
		
		if(HCpuP_Lost_sper < 1000000)
		{
			t = 1000000 / HCpuP_Lost_sper;
			printf("-%uus\n",t);
		}
		else
		{
			t = 1000000000 / HCpuP_Lost_sper;
			printf("-%uns\n",t);
		}
		HCpuP_Lost_sper = 0;
		//HCpuP_Lost_times = gt_get()+1000;
		HCpuP_Lost_times += 1000;
	}	
}


static uint32_t Power_check_times;		//�����������
static uint32_t LED_twinkle_times;		//LED��˸����
static uint32_t Roadway_Check_times;	//·���������
static uint32_t WIFI_Upload_data_times; //ͨ��Wifi�ϴ���������


int main(void)
{	
	uint16_t Light_Value = 0;				//��ǿ��ֵ	
	uint16_t CodedDisk_Value = 0;			//����
	uint16_t Nav_Value = 0;					//�Ƕ�
	
	Hardware_Init();						//Ӳ����ʼ��
	Readcard_daivce_Init();
	LED_twinkle_times =  gt_get() + 50;     //
	Power_check_times =  gt_get() + 200;

	Roadway_Check_times = 	gt_get() + 1;
	WIFI_Upload_data_times = gt_get() + 200;
	
	Principal_Tab[0] = 0x55;
	Principal_Tab[1] = 0xAA;	
	
	Follower_Tab[0] = 0x55;
	Follower_Tab[1] = 0x02;
	
	Send_UpMotor(0 ,0);
	Send_Debug_Info("TEST0\n",7);
	while(1)
	{
		KEY_Check();									//�������
		
		if(gt_get_sub(LED_twinkle_times) == 0) 			
		{
			LED_twinkle_times =  gt_get() + 50;			//LED4״̬ȡ��
			LED4 = !LED4;
		} 
		HCpuP_Lost_while();
		
		Can_WifiRx_Check();
		Full_Automatic();
		Can_ZigBeeRx_Check();
		
				
		if(gt_get_sub(Power_check_times) == 0)
		{
			Power_check_times =  gt_get() + 200;
			Power_Check();								//��ص������
		}
		
		
		if(gt_get_sub(WIFI_Upload_data_times) == 0)
		{
			WIFI_Upload_data_times =  gt_get() + 200;
		
			if(Host_AGV_Return_Flag == RESET)
			{
				Principal_Tab[2] = Stop_Flag;				//����״̬
				Principal_Tab[3] = Get_tba_phsis_value();	//����״ֵ̬����
				
				//if(Roadway_GoBack_Check())
				Ultrasonic_Ranging();						//����������						//����������
				Principal_Tab[4]=dis%256;        
				Principal_Tab[5]=dis/256;
				
				//if(Roadway_GoBack_Check())
				Light_Value = Get_Bh_Value();				//��ǿ�ȴ�����	
				Principal_Tab[6]=Light_Value%256;	    	//��������
				Principal_Tab[7]=Light_Value/256;
				
				CodedDisk_Value = CanHost_Mp;				//����
				Principal_Tab[8]=CodedDisk_Value%256;	    	
				Principal_Tab[9]=CodedDisk_Value/256;
				
				Nav_Value = CanHost_Navig;					//�Ƕ�
				Principal_Tab[10]=Nav_Value%256;	    	
				Principal_Tab[11]=Nav_Value/256;
				
/**				Send_WifiData_To_Fifo(Principal_Tab,12); **/
//				UartA72_TxClear();
//				UartA72_TxAddStr(Principal_Tab,12);
//				UartA72_TxStart();

			} else if((Host_AGV_Return_Flag == SET) && (AGV_data_Falg == SET)){
				
				UartA72_TxClear();
				UartA72_TxAddStr(Principal_Tab,50);
				UartA72_TxStart();
				Send_WifiData_To_Fifo(Follower_Tab,50);
				AGV_data_Falg = 0;
			}
		}
	}		
		SwoPt_while();
}



//			printf ����
//__________________________________________________________________________________________________

#if 0
#define ITM_STIM_U32 (*(volatile unsigned int*)0xE0000000)    // Stimulus Port Register word acces
#define ITM_STIM_U8  (*(volatile         char*)0xE0000000)    // Stimulus Port Register byte acces
#define ITM_ENA      (*(volatile unsigned int*)0xE0000E00)    // Trace Enable Ports Register
#define ITM_TCR      (*(volatile unsigned int*)0xE0000E80)    // Trace control register

uint8_t SwoPt_EnCheck(void)
{
	if ((ITM_TCR & 1) == 0) {
		return 0;
	}

	if ((ITM_ENA & 1) == 0) {
		return 0;
	}		
	return 1;
}

void SwoPt_sendByte(uint8_t SwoPt_c)
{
	if(SwoPt_EnCheck())
	{
		if(ITM_STIM_U8 & 1)
		{
			ITM_STIM_U8 = SwoPt_c;
		}
	}
}
#endif

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  //USART_SendData(EVAL_COM1, (uint8_t) ch);
	//SwoPt_sendByte((uint8_t) ch);
	SwoPt_PrintChar(ch);

  /* Loop until the end of transmission */
  //while (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_TC) == RESET)
  //{}

  return ch;
}

//__________________________________________________________________________________________________

//end main.c feil


