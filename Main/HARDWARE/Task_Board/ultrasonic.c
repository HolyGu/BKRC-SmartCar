#include "stm32f4xx.h"
#include "ultrasonic.h"
#include "delay.h"
#include "cba.h"

float Ultrasonic_Value = 0;
uint32_t Ultrasonic_Num=0;						// 计数值
uint16_t dis =0 ;

void Ultrasonic_Port()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB,ENABLE);
	

	//GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_SWJ);
	//GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SWJ);
	//GPIO_PinAFConfig(GPIOA,GPIO_PinSource15,GPIO_AF_SWJ);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource14,GPIO_AF_SWJ);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource13,GPIO_AF_SWJ);
	
	//GPIOA15---INC--RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  //通用输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽输出 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   //上拉
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//GPIOB4---INT0--TX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   //浮空 
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
}


void Ultrasonic_TIM(uint16_t arr,uint16_t psc)
{
	//GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
	
	TIM_InitStructure.TIM_Period = arr;
	TIM_InitStructure.TIM_Prescaler = psc;
	TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
//此参数对基本定时器无效
//	TIM_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
//	TIM_InitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM6,&TIM_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 8;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM6, DISABLE);
}

void Ultrasonic_EXTI()
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB,EXTI_PinSource4);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 7;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void Ultrasonic_Init()
{
	Ultrasonic_Port();
	Ultrasonic_TIM(9,83);
	Ultrasonic_EXTI();
}


//超声波测距
void Ultrasonic_Ranging()
{
    INC = 1;            
	delay_us(3);
    INC = 0;

	TIM_Cmd(TIM6,ENABLE);
	//EXTI_ClearITPendingBit(EXTI_Line4);	
	TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
	
    Ultrasonic_Num  = 0;			 // 定时器清零

	delay_ms(30);			 //等待一段时间，等待发送超声波控制信号
	INC = 1;
	delay_ms(5);
}

void TIM6_DAC_IRQHandler()
{
	if(TIM_GetITStatus(TIM6,TIM_IT_Update) == SET)
	{
		Ultrasonic_Num++;
		
	}
	TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
}


void EXTI4_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line4) == SET)
	{
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4) == RESET)
		{	
			TIM_Cmd(TIM6,DISABLE);
			Ultrasonic_Value = Ultrasonic_Num;
			Ultrasonic_Value =(float)Ultrasonic_Value*1.72-20;       // 计算距离定时10us，S=Vt/2（减2是误差补尝）
			dis = (uint16_t) Ultrasonic_Value;
		}
		EXTI_ClearITPendingBit(EXTI_Line4);
	}
}




