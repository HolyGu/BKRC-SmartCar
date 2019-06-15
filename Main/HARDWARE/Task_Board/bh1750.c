#include "stm32f4xx.h"
#include "delay.h"
#include "bh1750.h"


#define	 SlaveAddress   0x46 //����������IIC�����еĴӵ�ַ,����ALT  ADDRESS��ַ���Ų�ͬ�޸�
uint8_t  BUF[4];                  //�������ݻ�����


void BH1750_PortInit()
{	
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOG,ENABLE);
	//PB6-SCL 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	//PG15-ADDR
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);	
}

void SDA_OUT()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// PB7-SDA
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

}

void SDA_IN()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// PB7-SDA
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}


/***************************************************************
** ���ܣ�     ����IIC��ʼ�ź�
** ������	  �޲���
** ����ֵ��    ��
****************************************************************/
void BH1750_Start()
{
    SDA_OUT();     //sda�����
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(4);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}
/***************************************************************
** ���ܣ�     ����IICֹͣ�ź�
** ������	  �޲���
** ����ֵ��    ��
****************************************************************/
void BH1750_Stop()
{
	SDA_OUT();//sda�����
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
	IIC_SCL=1;
 	delay_us(4);
	IIC_SDA=1;//����I2C���߽����ź�
	delay_us(4);
}
/***************************************************************
** ���ܣ�     ����IICӦ���ź�
** ������	  �޲���
** ����ֵ��    ��
****************************************************************/
void BH1750_SendACK(u8 ack)
{
    SDA_OUT();          //sda�����
	if(ack)IIC_SDA=1;   //дӦ���ź�
	else IIC_SDA=0;          
    IIC_SCL=1;          //����ʱ����
    delay_us(2);        //��ʱ
    IIC_SCL=0;          //����ʱ����
    delay_us(2);        //��ʱ
}

/***************************************************************
** ���ܣ�     ����IIC�����ź�
** ������	  �޲���
** ����ֵ��    ��
****************************************************************/
u8 BH1750_RecvACK()
{
    u8 data;
	  SDA_IN();         //SDA����Ϊ����
    IIC_SCL=1;        //����ʱ����
    delay_us(2);      //��ʱ
    data = READ_SDA;  //��Ӧ���ź�
    IIC_SCL=0;        //����ʱ����
    delay_us(2);      //��ʱ
    return data;
}
/***************************************************************
** ���ܣ�     ��IIC���߷���һ���ֽ�����
** ������	  dat��һ�ֽ�����
** ����ֵ��    ��
****************************************************************/
void BH1750_SendByte(u8 dat)
{
    u8 i,bit;
	SDA_OUT();               //sda�����
    for (i=0; i<8; i++)      //8λ������
    {
	    bit=dat&0x80;
		if(bit) IIC_SDA=1;
		else IIC_SDA=0;
        dat <<= 1;           //�Ƴ����ݵ����λ
        IIC_SCL=1;           //����ʱ����
        delay_us(2);         //��ʱ
        IIC_SCL=0;           //����ʱ����
        delay_us(2);         //��ʱ
    }
    BH1750_RecvACK();
}
/***************************************************************
** ���ܣ�     ��IIC���߽���һ���ֽ�����
** ������	  �޲���
** ����ֵ��   dat������һ�ֽ�����
****************************************************************/
u8 BH1750_RecvByte()
{
    u8 i;
    u8 dat = 0;
	SDA_IN();               //SDA����Ϊ����
    IIC_SDA=1;              //ʹ���ڲ�����,׼����ȡ����,
    for (i=0; i<8; i++)     //8λ������
    {
        dat <<= 1;
        IIC_SCL=1;          //����ʱ����
        delay_us(2);        //��ʱ
		if(READ_SDA) dat+=1;               
        IIC_SCL=0;          //����ʱ����
        delay_us(2);        //��ʱ
    }
    return dat;
}

/***************************************************************
** ���ܣ�     ��bh1750д������
** ������	  �޲���
** ����ֵ��   ��
****************************************************************/
void Single_Write_BH1750(u8 REG_Address)
{
    BH1750_Start();                  //��ʼ�ź�
    BH1750_SendByte(SlaveAddress);   //�����豸��ַ+д�ź�
    BH1750_SendByte(REG_Address);    //�ڲ��Ĵ�����ַ��
    BH1750_Stop();                   //����ֹͣ�ź�
}

/***************************************************************
** ���ܣ�     ��������BH1750�ڲ����� 
** ������	  �޲���
** ����ֵ��   ��
****************************************************************/
void Multiple_Read_BH1750(void)
{   u8 i;	
    BH1750_Start();                          //��ʼ�ź�
    BH1750_SendByte(SlaveAddress+1);         //�����豸��ַ+���ź�
	
	for (i=0; i<3; i++)                      //������ȡ2����ַ���ݣ��洢��BUF
    {
        BUF[i] = BH1750_RecvByte();          //BUF[0]�洢0x32��ַ�е�����
        if (i == 3)
        {
           BH1750_SendACK(1);                //���һ��������Ҫ��NOACK
        }
        else
        {		
          BH1750_SendACK(0);                //��ӦACK
       }
   }
    BH1750_Stop();                          //ֹͣ�ź�
 //   delay_ms(150);
}

/***************************************************************
** ���ܣ�     ��ʼ��BH1750
** ������	  �޲���
** ����ֵ��   ��
****************************************************************/
void BH1750_Configure(void)
{
   BH1750_PortInit();
   Single_Write_BH1750(0x01);  
   ADDR = 0;  						//��ADDRλ��ʼ������
}

/***************************************************************
** ���ܣ�     ��ȡ���ն�
** ������	  �޲���
** ����ֵ��   data�����ع��ն�ֵ
****************************************************************/
uint16_t Get_Bh_Value(void)
{  
	static float temp;
	unsigned int data;
	int dis_data ;
    Single_Write_BH1750(0x01);   // power on
    Single_Write_BH1750(0x10);   // H- resolution mode
    delay_ms(200);              //��ʱ200ms
    Multiple_Read_BH1750();       //�����������ݣ��洢��BUF��
    dis_data=BUF[0];
    dis_data=(dis_data<<8)+BUF[1];//�ϳ����ݣ�����������
    temp=(float)dis_data/1.2;
	data=(int)temp;
    return data;        
}