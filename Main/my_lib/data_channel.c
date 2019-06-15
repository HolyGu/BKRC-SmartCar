
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
** 功能：     定时器4中断服务函数
** 参数：	  无参数
** 返回值：    无
****************************************************************/
void TIM4_IRQHandler(void)
{ 	    		  			    
/*	if(TIM4->SR&0X0001)         // 溢出中断
	{	 
		 rxd1_timer4_1ms++;
		 if(rxd1_timer4_1ms>5)	   // 5ms时间已到，数据帧接收完成
		 {
		 	  rxd1_timer4_flag=1;
			  TIM4->CR1&=~(0x01);       // 关使能定时器4
		     Wifi_Rx_flag =1;
		 }
    }	 					   
	TIM4->SR&=~(1<<0);          // 清除中断标志位 	    */
	
	if(TIM_GetITStatus(TIM4,TIM_FLAG_Update) == SET)
	{
		rxd1_timer4_1ms++;
		 if(rxd1_timer4_1ms>5)	   // 5ms时间已到，数据帧接收完成
		 {
		 	  rxd1_timer4_flag=1;
			 TIM_Cmd(TIM4,ENABLE);
		     Wifi_Rx_flag =1;
		 }
		TIM_ClearITPendingBit(TIM4,TIM_FLAG_Update);
	}
}

/***************************************************************
** 功能：     定时器4中断初始化
** 参数：	  arr：自动重装值
**			  psc：时钟预分频数
** 返回值：    无
****************************************************************/
void Timer4_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //时钟使能
	
	//定时器TIM4初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //使能指定的TIM4中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM4中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器

	TIM_Cmd(TIM4, ENABLE);  //使能TIMx		
}

void Wifi_data_Receive( u8 res)  // wifi 数据接收处理 
{
	
	rxd1_timer4_1ms=0;
	
	if(rxd1_timer4_flag==1) //  5ms时间 数据帧重新开始
	{
		 rxd1_timer4_flag=0;
		 TIM4->CR1|=0x01;     //使能定时器4
		 Wifi_Rx_num =0;
		 Wifi_Rx_Buf[Wifi_Rx_num]=res;
	}
	else if(Wifi_Rx_num < WIFI_MAX_NUM )	
	{
		  Wifi_Rx_Buf[++Wifi_Rx_num]=res;	 // 接收数据帧
	}

	else   // 数据超出最大接收数据长度时，舍去不要
	{
			;
	}

}

void Zigbee_data_Receive( u8 res)  // zigbee 数据接收处理
{
    u8 sum;

	if(Zigbee_Rx_num >0)
	{
	   Zigb_Rx_Buf[Zigbee_Rx_num]=res;
	   Zigbee_Rx_num++;
	}
	else if (res==0x55)		// 寻找包头
	{
		
	   Zigb_Rx_Buf[0]=res;
	   Zigbee_Rx_num=1;
	}
	else Zigbee_Rx_num =0;
	
   if(Zigbee_Rx_num >= Zigbee_Rx_Max)
	{

		if((Zigb_Rx_Buf[Zigbee_Rx_Max -1]==0xbb)&&(Zigbee_RxZt_flag ==0)&&(Zigb_Rx_Buf[1]!=0xfd))	 // 判断包尾	//change by ygm
		{									  
		    //主指令与三位副指令左求和校验
			//注意：在求和溢出时应该对和做256取余。
			Zigbee_Rx_num=0;	  // 计数清零
			Zigbee_RxZt_flag =0;  // 接收状态清零
			 sum=(Zigb_Rx_Buf[2]+Zigb_Rx_Buf[3]+Zigb_Rx_Buf[4]+Zigb_Rx_Buf[5])%256;
			if(sum==Zigb_Rx_Buf[6])
			{
					Zigbee_Rx_flag =1;	//   指令验证正确，标志位置1
			}
			else {Zigbee_Rx_flag =0;}
		}
		else if((Zigb_Rx_Buf[1]==0xfd)&&(Zigbee_RxZt_flag ==0))
		 {
			if(Zigb_Rx_Buf[2]>8)	
			{ 
			   Zigbee_Rx_Max = Zigb_Rx_Buf[2];
			   Zigbee_RxZt_flag =1;  // 接收状态1  接着接收数据
			 }
			 else 			 
			{		 
			   Zigbee_Rx_num=0;		// 计数清零
				if(Zigb_Rx_Buf[Zigbee_Rx_Max-1]==0xbb)
				 {
				   Zigbee_RxZt_flag =0;  // 接收状态清零
				 //  send_Flag=1;			//change by ygm
			       Zigbee_Rx_Max =8;

				 }
			}	  

			
		  }
		else if((Zigbee_RxZt_flag ==1)&&(Zigb_Rx_Buf[Zigbee_Rx_Max -1]==0xbb))
		{
			Zigbee_Rx_num=0;	  // 计数清零
			Zigbee_RxZt_flag =0;  // 接收状态清零
		//	send_Flag=1;
			Zigbee_Rx_Max =8;
		    
		}
		
		
		else {Zigbee_Rx_flag =0; Zigbee_Rx_num =0;}		 //接收错误指令，打开蜂鸣器
	}

}


