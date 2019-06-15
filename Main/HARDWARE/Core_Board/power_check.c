#include "stm32f4xx.h"
#include "delay.h"
#define __POWER_CHECK_C__
#include "power_check.h"
#include "CanP_HostCom.h"
#include "data_filtering.h"

#define POWER_CHECK_L    14 // 电量采集通道
#define POWER_CHECK_R    15
#define POWER_CHECK_NUM  3  // ADC采样次数

#define PWR_MAX  12000.0    // 12V
#define PWR_MIN   9000.0    // 9V
#define PWR_DV   (PWR_MAX - PWR_MIN)
u8 Electric_Buf[2];
float Pa =0.0,Pb =0.0;  // 计算参数

u8 pwr_ck_l =0;  // 电量数据
u8 pwr_ck_r =0;


//初始化ADC														   
void  ADC_Configure(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	ADC_CommonInitTypeDef ADC_CommonStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	
	//PA6
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	ADC_CommonStructure.ADC_Mode = ADC_Mode_Independent;	//独立模式
	ADC_CommonStructure.ADC_Prescaler = ADC_Prescaler_Div2;	//ADC 2分频
	ADC_CommonStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;	//两个采样间隔时间
	ADC_CommonStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;			//失能ADC DMA功能
	ADC_CommonInit(&ADC_CommonStructure);
	
	
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//复位结束	
	
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;			//单通道
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;	//12位精度
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;		//单次转换
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;	//无触发
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;						//数据右对齐	
	ADC_InitStructure.ADC_NbrOfConversion = 1;				//通道个数为1
	ADC_Init(ADC1,&ADC_InitStructure);
	
	ADC_RegularChannelConfig(ADC1,ADC_Channel_6,1,ADC_SampleTime_144Cycles);
	
	ADC_Cmd(ADC1,ENABLE);
}


/**
*函数功能: 获取ADC值
*参    数: 无
*返 回 值: ADC转换值
**/
uint16_t Get_ADC_Value()
{
	ADC_ClearFlag(ADC1,ADC_FLAG_EOC);
	ADC_SoftwareStartConv(ADC1);	
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));	
	return ADC_GetConversionValue(ADC1);
}


u16  Get_Electricity(u8 times )
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+= Get_ADC_Value();
		 delay_ms(5);
	}
	return temp_val/times;
}


void Parameter_Init(void) // 电量计算参数初始化
{
	 Pb =(float) (PWR_MIN / PWR_DV);
	 Pb *= 100; 
	 
	 Pa = (float)(3300*11)/4096 ;
	 
	 Pa = (float)((Pa *100) /PWR_DV);
}

void Electricity_Init(void)  // 电量检测初始化
{
    Parameter_Init();
	ADC_Configure();
}

uint32_t MLib_GetSub(uint32_t a,uint32_t b)
{
	return (a > b)? a-b:b-a;
}


u16 temp = 0;
u16 temp2 = 0;
uint8_t temp_value = 0;
void Power_Check(void)
{
	temp =  Get_Electricity(POWER_CHECK_NUM);
	temp =  Smoothing_Filtering(temp);
	temp_value = MLib_GetSub(temp,temp2);
	if(temp2 == 0)
	{
			temp = (Pa*temp); // 电量计算方法		
			if( temp < Pb ) pwr_ck_l =0;
			else 
			{
			   pwr_ck_l = (u8) ( temp - Pb);
					
					 if( pwr_ck_l >100)  pwr_ck_l =100;
			}  
			Electric_Buf[0] = pwr_ck_l;	
				
			Send_Electric( 1,Electric_Buf[0]);	
	}
	temp2 = temp;
	if(temp_value > 10)
	{
			temp = (Pa*temp); // 电量计算方法			
			if( temp < Pb ) pwr_ck_l =0;
			else 
			{
			   pwr_ck_l = (u8) ( temp - Pb);
					
					 if( pwr_ck_l >100)  pwr_ck_l =100;
			}  
			Electric_Buf[0] = pwr_ck_l;		
			Send_Electric( 1,Electric_Buf[0]);	
	}
	
}



