
//#include "uart_my.h"
//#include "exit.h"
//#include "led.h"
#include "usart.h"
#define __DATA_CHANNEL_C__
#include "data_channel.h"


u8 Zigbee_Rx_Max =8;
u8 Wifi_Rx_num =0;
u8 Wifi_Rx_flag=0;

u8 Zigbee_Rx_num =0;
u8 Zigbee_Rx_flag=0;
u8 Zigbee_RxZt_flag =0;

u8 	rxd1_timer4_1ms,rxd1_timer4_flag;


/***************************************************************
** ���ܣ�     ��ʱ��4�жϷ�����
** ������	  �޲���
** ����ֵ��    ��
****************************************************************/
void TIM4_IRQHandler(void)
{ 	    		  			    
/*	if(TIM4->SR&0X0001)         // ����ж�
	{	 
		 rxd1_timer4_1ms++;
		 if(rxd1_timer4_1ms>5)	   // 5msʱ���ѵ�������֡�������
		 {
		 	  rxd1_timer4_flag=1;
			  TIM4->CR1&=~(0x01);       // ��ʹ�ܶ�ʱ��4
		     Wifi_Rx_flag =1;
		 }
    }	 					   
	TIM4->SR&=~(1<<0);          // ����жϱ�־λ 	    */
	
	if(TIM_GetITStatus(TIM4,TIM_FLAG_Update) == SET)
	{
		rxd1_timer4_1ms++;
		 if(rxd1_timer4_1ms>5)	   // 5msʱ���ѵ�������֡�������
		 {
		 	  rxd1_timer4_flag=1;
			 TIM_Cmd(TIM4,ENABLE);
		     Wifi_Rx_flag =1;
		 }
		TIM_ClearITPendingBit(TIM4,TIM_FLAG_Update);
	}
}

/***************************************************************
** ���ܣ�     ��ʱ��4�жϳ�ʼ��
** ������	  arr���Զ���װֵ
**			  psc��ʱ��Ԥ��Ƶ��
** ����ֵ��    ��
****************************************************************/
void Timer4_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM4��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM4�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM4�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���

	TIM_Cmd(TIM4, ENABLE);  //ʹ��TIMx		
}

void Wifi_data_Receive( u8 res)  // wifi ���ݽ��մ��� 
{
	
	rxd1_timer4_1ms=0;
	
	if(rxd1_timer4_flag==1) //  5msʱ�� ����֡���¿�ʼ
	{
		 rxd1_timer4_flag=0;
		 TIM4->CR1|=0x01;     //ʹ�ܶ�ʱ��4
		 Wifi_Rx_num =0;
		 Wifi_Rx_Buf[Wifi_Rx_num]=res;
	}
	else if(Wifi_Rx_num < WIFI_MAX_NUM )	
	{
		  Wifi_Rx_Buf[++Wifi_Rx_num]=res;	 // ��������֡
	}

	else   // ���ݳ������������ݳ���ʱ����ȥ��Ҫ
	{
			;
	}

}

void Zigbee_data_Receive( u8 res)  // zigbee ���ݽ��մ���
{
    u8 sum;

	if(Zigbee_Rx_num >0)
	{
	   Zigb_Rx_Buf[Zigbee_Rx_num]=res;
	   Zigbee_Rx_num++;
	}
	else if (res==0x55)		// Ѱ�Ұ�ͷ
	{
		
	   Zigb_Rx_Buf[0]=res;
	   Zigbee_Rx_num=1;
	}
	else Zigbee_Rx_num =0;
	
   if(Zigbee_Rx_num >= Zigbee_Rx_Max)
	{

		if((Zigb_Rx_Buf[Zigbee_Rx_Max -1]==0xbb)&&(Zigbee_RxZt_flag ==0)&&(Zigb_Rx_Buf[1]!=0xfd))	 // �жϰ�β	//change by ygm
		{									  
		    //��ָ������λ��ָ�������У��
			//ע�⣺��������ʱӦ�öԺ���256ȡ�ࡣ
			Zigbee_Rx_num=0;	  // ��������
			Zigbee_RxZt_flag =0;  // ����״̬����
			 sum=(Zigb_Rx_Buf[2]+Zigb_Rx_Buf[3]+Zigb_Rx_Buf[4]+Zigb_Rx_Buf[5])%256;
			if(sum==Zigb_Rx_Buf[6])
			{
					Zigbee_Rx_flag =1;	//   ָ����֤��ȷ����־λ��1
			}
			else {Zigbee_Rx_flag =0;}
		}
		else if((Zigb_Rx_Buf[1]==0xfd)&&(Zigbee_RxZt_flag ==0))
		 {
			if(Zigb_Rx_Buf[2]>8)	
			{ 
			   Zigbee_Rx_Max = Zigb_Rx_Buf[2];
			   Zigbee_RxZt_flag =1;  // ����״̬1  ���Ž�������
			 }
			 else 			 
			{		 
			   Zigbee_Rx_num=0;		// ��������
				if(Zigb_Rx_Buf[Zigbee_Rx_Max-1]==0xbb)
				 {
				   Zigbee_RxZt_flag =0;  // ����״̬����
				 //  send_Flag=1;			//change by ygm
			       Zigbee_Rx_Max =8;

				 }
			}	  

			
		  }
		else if((Zigbee_RxZt_flag ==1)&&(Zigb_Rx_Buf[Zigbee_Rx_Max -1]==0xbb))
		{
			Zigbee_Rx_num=0;	  // ��������
			Zigbee_RxZt_flag =0;  // ����״̬����
		//	send_Flag=1;
			Zigbee_Rx_Max =8;
		    
		}
		
		
		else {Zigbee_Rx_flag =0; Zigbee_Rx_num =0;}		 //���մ���ָ��򿪷�����
	}

}


