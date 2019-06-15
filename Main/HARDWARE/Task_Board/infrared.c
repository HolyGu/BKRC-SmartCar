#include "stm32f4xx.h"
#include "infrared.h"
#include "delay.h"

void Infrared_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);

	//GPIOF11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  //ͨ�����
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //������� 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   //����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
	GPIO_Init(GPIOF,&GPIO_InitStructure);
}

/***************************************************************
** ���ܣ�     ���ⷢ���ӳ���
** ������	  *s��ָ��Ҫ���͵�����
**             n�����ݳ���
** ����ֵ��    ��
****************************************************************/
void Infrared_Send(u8 *s,int n)
{
   u8 i,j,temp;

    RI_TXD=0;
    delay_ms(9);
    RI_TXD=1;
    delay_ms(4);
	delay_us(560);

	for(i=0;i<n;i++)
	{
		for(j=0;j<8;j++)
		  {
		     temp=(s[i]>>j)&0x01;
		     if(temp==0)//����0
		        {
		           RI_TXD=0;
		           delay_us(500);//��ʱ0.5ms
		           RI_TXD=1;
		           delay_us(500);//��ʱ0.5ms
		         }
		     if(temp==1)//����1
		         {
		           RI_TXD=0;
		           delay_us(500);//��ʱ0.5ms
		           RI_TXD=1;
				   delay_ms(1);
		           delay_us(800);//��ʱ1.69ms  690
		
		         }
		  }
	}
    RI_TXD=0;//����
    delay_us(560);//��ʱ0.56ms
    RI_TXD=1;//�رպ��ⷢ��
}



