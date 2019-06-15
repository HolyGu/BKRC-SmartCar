#include "stm32f4xx.h"
#include "CanP_Hostcom.h"
#include "delay.h"
#include "roadway_check.h"
#include "cba.h"

uint8_t wheel_L_Flag =0,wheel_R_Flag = 0;
uint8_t wheel_Nav_Flag = 0;
uint8_t Go_Flag = 0,Back_Flag = 0;
uint8_t Track_Flag = 0;

uint8_t Line_Flag = 0;
uint16_t count = 0;

uint8_t Stop_Flag = 0;
int LSpeed = 0,RSpeed = 0;
int Car_Spend = 0;
uint16_t temp_MP = 0;
uint16_t temp_Nav = 0;

uint8_t Wheel_flag = 0;

/**
特殊地形处理标志位
自定义   by --jwm
*/
uint8_t Track_Special_Flag = 0;
uint8_t Go_With_Track_Flag = 0;
uint8_t RFID_Track_Flag = 0;
uint8_t RFID_Write_Flag = 0;
/**OVER*/

void Track(uint8_t gd);
void RFID_Track(uint8_t gd);
//_________________________________________________________
int16_t Roadway_cmp;
extern int16_t CanHost_Mp;

void Roadway_mp_syn(void)
{
	Roadway_cmp = CanHost_Mp;
}

uint16_t Roadway_mp_Get(void)
{
	//return CanHost_Mp - Roadway_cmp;
	uint32_t ct;
	if(CanHost_Mp > Roadway_cmp)
		ct = CanHost_Mp - Roadway_cmp;
	else
		ct = Roadway_cmp - CanHost_Mp;
	
//	if(ct & 0x8000)
//		ct = 0xffff- ct;
	if(ct > 0x8000)
		ct = 0xffff - ct;
	
	return ct;
	
}

//_______________________________________________________________



//_________________________________________________________
uint16_t Roadway_Navig;
extern uint16_t CanHost_Navig;

void Roadway_nav_syn(void)
{
	Roadway_Navig = CanHost_Navig;
}

uint16_t Roadway_nav_Get(void)
{
	//return CanHost_Mp - Roadway_cmp;
	uint16_t ct;
	if(CanHost_Navig > Roadway_Navig)
		ct = CanHost_Navig - Roadway_Navig;
	else
		ct = Roadway_Navig - CanHost_Navig;
	
	while(ct >= 36000)
		ct -= 36000;
	
	return ct;
	
}

//_______________________________________________________________

void Roadway_Flag_clean(void)
{
	wheel_L_Flag =0;wheel_R_Flag = 0;
	Go_Flag = 0;Back_Flag = 0;
	Track_Flag = 0;
	Stop_Flag = 0;
	temp_MP = 0;
}

/**
	前进监测
*/
void Go_and_Back_Check(void)
{	
	if(Go_Flag == 1)
	{
		//uint16_t Mp_Value;
		//Mp_Value = Roadway_mp_Get();
		if(temp_MP <= Roadway_mp_Get())
		{
			//getLost_Test_Times();
			Go_Flag = 0;
			Stop_Flag = 3;
			Send_UpMotor(0,0);
		}
	} 
	else if(Back_Flag == 1)
	{
		//uint16_t Mp_Value;
		//Mp_Value = Roadway_mp_Get();
		if(temp_MP <= Roadway_mp_Get())
		{
			Back_Flag = 0;
			Stop_Flag=3;
			Send_UpMotor(0,0);
		}
	}
}
/**
	前进监测
*/
void Go_With_Track_Check(void)
{	
	if(Go_With_Track_Flag == 1)
	{
		if(temp_MP <= Roadway_mp_Get())
		{
			Go_With_Track_Flag = 0;
			Send_UpMotor(0,0);
		}else
			{
				u16	gd = Get_Host_UpTrack(TRACK_H8);
					Stop_Flag=0;
					if(gd==0XE7||gd==0XF7||gd==0XEF)//1、中间3/4传感器检测到黑线，全速运行
					{
						LSpeed=Car_Spend;
						RSpeed=Car_Spend;
					}
					if(Line_Flag!=2)
					{
						if(gd==0XF3||gd==0XFB) //2、中间4、3传感器检测到黑线，微右拐	
						{ 
							LSpeed=Car_Spend+30;
							RSpeed=Car_Spend-30;
							Line_Flag=0;
						}
						else if(gd==0XF9||gd==0XFD)//3、中间3、2传感器检测到黑线，再微右拐		
						{ 
							 LSpeed=Car_Spend+40;
							 RSpeed=Car_Spend-60;				 
							 Line_Flag=0;
						}
						else if(gd==0XFC)//4、中间2、1传感器检测到黑线，强右拐
						{ 
							LSpeed = Car_Spend+50;
							RSpeed = Car_Spend-90;
							 Line_Flag=0;
						}
						else if(gd==0XFE)//5、最右边1传感器检测到黑线，再强右拐			
						{ 
							 LSpeed = Car_Spend+60;
							 RSpeed = Car_Spend-120;			
							 Line_Flag=1;
						}
					}
					if(Line_Flag!=1)
					{
						if(gd==0XCF)//6、中间6、5传感器检测到黑线，微左拐			
						{ 
							 RSpeed = Car_Spend+30;
							 LSpeed = Car_Spend-30;
							 Line_Flag=0;
						} 
						else if(gd==0X9F||gd==0XDF)//7、中间7、6传感器检测到黑线，再微左拐		 
						{ 
							 RSpeed = Car_Spend+40;
							 LSpeed = Car_Spend-60;
							 Line_Flag=0;
						} 			  
						else if(gd==0X3F||gd==0XBF)//8、中间8、7传感器检测到黑线，强左拐		 
						{ 
							 RSpeed = Car_Spend+50;
							 LSpeed = Car_Spend-90;
							 Line_Flag=0;
						} 
						else if(gd==0X7F)//9、最左8传感器检测到黑线，再强左拐		 	
						{ 
							 RSpeed = Car_Spend+60;
							 LSpeed = Car_Spend-120;
							 Line_Flag=2;
						}			
					}
					if(gd==0xFF)   //循迹灯全亮
					{
						if(count>10)
						{
								count=0;
								Go_With_Track_Flag = 0;
								RFID_Write_Flag = 1;
						}	
						else 
							count++;				
					}
					else 
						count=0;
				
				if(Go_With_Track_Flag != 0)
				{
					static uint32_t gt_save = 0;
					uint32_t gt_temp;
					gt_temp = gt_get();
					if((gt_save + 2) < gt_temp)
					{
						Control(LSpeed,RSpeed);
						gt_save = gt_temp;
					}
				}
				else
				{
					Send_UpMotor(0,0);
				}
			}
	} 
}
uint8_t Roadway_GoBack_Check(void)
{
	return ((Go_Flag == 0)&&(Back_Flag == 0)&&(Track_Flag == 0)&&(wheel_L_Flag == 0)&&(wheel_R_Flag == 0))? 1:0;
	
}

/**
	角度监测转弯
*/
void wheel_Nav_check()
{ 	
	uint16_t Navig_Value = 0;
	uint16_t Mp_Value = 0;
	
	if(wheel_Nav_Flag)
	{
		Navig_Value = Roadway_nav_Get();
		Mp_Value = Roadway_mp_Get();
		
//		if(Navig_Value  >= temp_Nav) 	//到达指定角度
//		{	
//			wheel_Nav_Flag = 0;
//			Wheel_flag=0;
//			Stop_Flag=2;
//			Send_UpMotor(0,0);
//		} 
//		else 
		if(Mp_Value >=   temp_Nav)
		{
			wheel_Nav_Flag = 0;
			Wheel_flag=0;
			Stop_Flag=2;
			Send_UpMotor(0,0);
		}
	}
}


/**
	码盘监测转弯
*/
void wheel_mp_check()
{ 	
	uint16_t Track_Value;// = Get_Host_UpTrack(TRACK_H8);
	uint16_t Mp_Value = 0;
	if(wheel_L_Flag == 1)
	{
		Track_Value = Get_Host_UpTrack(TRACK_H8);
		if(Mp_Value > 550)				  //距离大于50CM
		{
			wheel_L_Flag = 0;
			Wheel_flag=0;
			Stop_Flag=2;
		    Send_UpMotor(0,0);
			//Host_Close_UpTrack( );  // 关闭寻迹数据上传
			Send_Debug_Info("MP550\n",7);
		}
		else
		{
			if(!(Track_Value &0X10)) //找到循迹线，停止
			{	
				if(Wheel_flag)
				{
					wheel_L_Flag = 0;
					Wheel_flag=0;
					Stop_Flag=2;
					//Host_Close_UpTrack( );  // 关闭寻迹数据上传
					Send_UpMotor(0,0);
				}
			}
			else if( Track_Value == 0Xff) 
				Wheel_flag=1;
		}
	} 
	else if(wheel_R_Flag == 1)
	{
		Track_Value = Get_Host_UpTrack(TRACK_H8);
		if(Mp_Value > 550)				  //距离大于50CM
		{
			wheel_R_Flag=0;
			Wheel_flag=0;
			Stop_Flag=2;
		    Send_UpMotor(0,0);
			//Host_Close_UpTrack( );  // 关闭寻迹数据上传
			Send_Debug_Info("MP550\n",7);
		}
		else
		{
		 if(!(Track_Value &0X08)) //找到循迹线，停止
			{	
				if(Wheel_flag)
				{
					wheel_R_Flag=0;
					Wheel_flag=0;
					Stop_Flag=2;
					Send_UpMotor(0,0);
					//Host_Close_UpTrack( );  // 关闭寻迹数据上传
				}
			}
			else if( Track_Value == 0Xff) 
				Wheel_flag=1;	
		}
	}
	
}
/**
	寻卡循迹检测 遇到白线或者黑线 停止check
*/
void RFID_Track_Check()
{	
	
	if(RFID_Track_Flag == 1)
	{
		RFID_Track(Get_Host_UpTrack(TRACK_H8));
	}	
}
/**
	循迹监测
*/
void Track_Check()
{	
	
	if(Track_Flag == 1 && Track_Special_Flag == 0)
	{
		Track(Get_Host_UpTrack(TRACK_H8));
	}	
}

void Roadway_Check(void)
{
	Go_and_Back_Check();
	Go_With_Track_Check();
	wheel_mp_check();
	wheel_Nav_check();
	Track_Check();
	RFID_Track_Check();
}
	

/***************************************************************
** 功能：     电机控制函数
** 参数：	  L_Spend：电机左轮速度
**            R_Spend：电机右轮速度
** 返回值：   无	  
****************************************************************/
void Control(int L_Spend,int R_Spend)
{
	if(L_Spend>=0)
	{	
		if(L_Spend>100)L_Spend=100;if(L_Spend<5)L_Spend=5;		//限制速度参数
	}
	else 
	{
		if(L_Spend<-100)L_Spend= -100;if(L_Spend>-5)L_Spend= -5;     //限制速度参数
	}	
	if(R_Spend>=0)
	{	
		if(R_Spend>100)R_Spend=100;if(R_Spend<5)R_Spend=5;		//限制速度参数
	}
	else
	{	
		if(R_Spend<-100)R_Spend= -100;if(R_Spend>-5)R_Spend= -5;		//限制速度参数		
	}
	Send_UpMotor(L_Spend ,R_Spend);	
}

/***************************************************************
** 功能：     循迹函数
** 参数：	  无参数
** 返回值：   无
****************************************************************/
void Track(uint8_t gd)
{
	if( gd == 0x00)	//循迹灯全灭 停止
	{	
		Track_Flag=0;
		Stop_Flag=1;
		Send_UpMotor(0,0);
		//Host_Close_UpTrack( );  // 关闭寻迹数据上传
		Send_Debug_Info("gd00\n",6);
	}
	else
	{
	   	Stop_Flag=0;
		if(gd==0XE7||gd==0XF7||gd==0XEF)//1、中间3/4传感器检测到黑线，全速运行
		{ 
			LSpeed=Car_Spend;
			RSpeed=Car_Spend;
		}

		if(Line_Flag!=2)
		{		

			if(gd==0XF3||gd==0XFB) //2、中间4、3传感器检测到黑线，微右拐	
			{ 
				LSpeed=Car_Spend+30;
				RSpeed=Car_Spend-30;
				Line_Flag=0;
			}
			else if(gd==0XF9||gd==0XFD)//3、中间3、2传感器检测到黑线，再微右拐		
			{ 
				 LSpeed=Car_Spend+40;
				 RSpeed=Car_Spend-60;				 
				 Line_Flag=0;
			}
			else if(gd==0XFC)//4、中间2、1传感器检测到黑线，强右拐
			{ 
				LSpeed = Car_Spend+50;
				RSpeed = Car_Spend-90;
				 Line_Flag=0;
			}
			else if(gd==0XFE)//5、最右边1传感器检测到黑线，再强右拐			
			{ 
				 LSpeed = Car_Spend+60;
				 RSpeed = Car_Spend-120;			
				 Line_Flag=1;
			}
		}
		if(Line_Flag!=1)
		{
			if(gd==0XCF)//6、中间6、5传感器检测到黑线，微左拐			
			{ 
				 RSpeed = Car_Spend+30;
				 LSpeed = Car_Spend-30;
				 Line_Flag=0;
			} 
			else if(gd==0X9F||gd==0XDF)//7、中间7、6传感器检测到黑线，再微左拐		 
			{ 
				 RSpeed = Car_Spend+40;
				 LSpeed = Car_Spend-60;
				 Line_Flag=0;
			} 			  
			else if(gd==0X3F||gd==0XBF)//8、中间8、7传感器检测到黑线，强左拐		 
			{ 
				 RSpeed = Car_Spend+50;
				 LSpeed = Car_Spend-90;
				 Line_Flag=0;
			} 
			else if(gd==0X7F)//9、最左8传感器检测到黑线，再强左拐		 	
			{ 
				 RSpeed = Car_Spend+60;
				 LSpeed = Car_Spend-120;
				 Line_Flag=2;
			}			
		}

		if(gd==0xFF)   //循迹灯全亮
		{
			if(count>50)
			{
					count=0;
					Track_Special_Flag =1;
					
					//Send_UpMotor(0,0);
					//Track_Flag=0;
					//if(Line_Flag ==0) Stop_Flag=4;
			}	
			else 
				count++;				
		}
		else 
			count=0;


	}
	
	if(Track_Flag != 0)
	{
		static uint32_t gt_save = 0;
		uint32_t gt_temp;
		gt_temp = gt_get();
		if((gt_save + 2) < gt_temp)
		{
			Control(LSpeed,RSpeed);
			gt_save = gt_temp;
		}
	}
	else
	{
		Send_UpMotor(0,0);
		Send_Debug_Info("Track_0\n",9);
		//getLost_Test_Times();
	}
	
	
/*	if(Track_Flag==0)
	{
		//Host_Close_UpTrack( );  // 关闭寻迹数据上传	
	}*/
	
}

/***************************************************************
** 功能：     寻卡循迹函数
** 参数：	  无参数
** 返回值：   无
****************************************************************/
void RFID_Track(uint8_t gd)
{
	if( gd == 0x00)	//循迹灯全灭 停止
	{	
		RFID_Track_Flag = 0;
		Stop_Flag=1;
		Send_UpMotor(0,0);
		Send_Debug_Info("gd00\n",6);
	}
	else
	{
	   	Stop_Flag=0;
		if(gd==0XE7||gd==0XF7||gd==0XEF)//1、中间3/4传感器检测到黑线，全速运行
		{ 
			LSpeed=Car_Spend;
			RSpeed=Car_Spend;
		}

		if(Line_Flag!=2)
		{		

			if(gd==0XF3||gd==0XFB) //2、中间4、3传感器检测到黑线，微右拐	
			{ 
				LSpeed=Car_Spend+30;
				RSpeed=Car_Spend-30;
				Line_Flag=0;
			}
			else if(gd==0XF9||gd==0XFD)//3、中间3、2传感器检测到黑线，再微右拐		
			{ 
				 LSpeed=Car_Spend+40;
				 RSpeed=Car_Spend-60;				 
				 Line_Flag=0;
			}
			else if(gd==0XFC)//4、中间2、1传感器检测到黑线，强右拐
			{ 
				LSpeed = Car_Spend+50;
				RSpeed = Car_Spend-90;
				 Line_Flag=0;
			}
			else if(gd==0XFE)//5、最右边1传感器检测到黑线，再强右拐			
			{ 
				 LSpeed = Car_Spend+60;
				 RSpeed = Car_Spend-120;			
				 Line_Flag=1;
			}
		}
		if(Line_Flag!=1)
		{
			if(gd==0XCF)//6、中间6、5传感器检测到黑线，微左拐			
			{ 
				 RSpeed = Car_Spend+30;
				 LSpeed = Car_Spend-30;
				 Line_Flag=0;
			} 
			else if(gd==0X9F||gd==0XDF)//7、中间7、6传感器检测到黑线，再微左拐		 
			{ 
				 RSpeed = Car_Spend+40;
				 LSpeed = Car_Spend-60;
				 Line_Flag=0;
			} 			  
			else if(gd==0X3F||gd==0XBF)//8、中间8、7传感器检测到黑线，强左拐		 
			{ 
				 RSpeed = Car_Spend+50;
				 LSpeed = Car_Spend-90;
				 Line_Flag=0;
			} 
			else if(gd==0X7F)//9、最左8传感器检测到黑线，再强左拐		 	
			{ 
				 RSpeed = Car_Spend+60;
				 LSpeed = Car_Spend-120;
				 Line_Flag=2;
			}			
		}

		if(gd==0xFF)   //循迹灯全亮
		{
			if(count>10)
			{
					count=0;
					RFID_Track_Flag = 0;
					RFID_Write_Flag = 1;
			}	
			else 
				count++;				
		}
		else 
			count=0;
	}
	if(RFID_Track_Flag != 0)
	{
		static uint32_t gt_save = 0;
		uint32_t gt_temp;
		gt_temp = gt_get();
		if((gt_save + 2) < gt_temp)
		{
			Control(LSpeed,RSpeed);
			gt_save = gt_temp;
		}
	}
	else
	{
		Send_UpMotor(0,0);
		Send_Debug_Info("Track_1\n",9);
	}
}




//______________________________________________


void roadway_check_TimInit(uint16_t arr,uint16_t psc)
{
	TIM_TimeBaseInitTypeDef TIM_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);
	
	TIM_InitStructure.TIM_Period = arr;
	TIM_InitStructure.TIM_Prescaler = psc;
	TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_InitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM9,&TIM_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM1_BRK_TIM9_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_ITConfig(TIM9,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM9, ENABLE);
}


void TIM1_BRK_TIM9_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM9,TIM_IT_Update) == SET)
	{
		if(Track_Special_Flag == 0)
		{
			Roadway_Check();								//路况检测
			LED1 = !LED1;
		}else if(Track_Special_Flag == 1)
				{
					Go_and_Back_Check();
				}
		
	}
	TIM_ClearITPendingBit(TIM9,TIM_IT_Update);
}













//end file



