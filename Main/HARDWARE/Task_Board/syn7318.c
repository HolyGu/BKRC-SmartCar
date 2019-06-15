#include "stm32f4xx.h"
#include "delay.h"
#include <string.h>
#include "cba.h"
#include "infrared.h"
#include "data_base.h"
#include "syn7318.h"
#include "canp_hostcom.h"

uint8_t Back[4] = {0};   //��������ش�������
uint8_t ASR[6] = {0};    //����ʶ�����ش�������
uint8_t S[4] = {0};      //����ģ�鵱ǰ����״̬�ش�������

uint8_t Send_SYN[8] = {0xAF,0x06,0x00,0x02,0x00,0x00,0x01,0xBF};

unsigned char Wake_Up[] = {0xfd,0x00,0x02,0x51,0x1F};
unsigned char Stop_Wake_Up[] = {0xFD,0x00,0x01,0x52};

unsigned char Start_ASR_Buf[] = {0xFD,0x00,0x02,0x10,0x03};
unsigned char Stop_ASR_Buf[] = {0xFD,0x00,0x01,0x11};

unsigned char Play_MP3[] ={ 0xFD,0x00,0x1E,0x01,0x01,0xC6,0xF4,0xB6,0xAF,0xD3, 0xEF ,0xD2, 0xF4,
                            	0xBF, 0xD8, 0xD6 ,0xC6 ,0xBC ,0xDD ,0xCA ,0xBB ,0xA3 ,0xAC, 0xC7, 0xEB,
                             	0xB7, 0xA2, 0xB3, 0xF6 ,0xD6, 0xB8, 0xC1, 0xEE };
unsigned char Ysn7813_flag=0;

#define SYN7318_RST_H GPIO_SetBits(GPIOB,GPIO_PinSource9)
#define SYN7318_RST_L GPIO_ResetBits(GPIOB,GPIO_PinSource9)

void Yu_Yin_Asr(void);

void USART6_Init(uint32_t baudrate)
{
	GPIO_InitTypeDef  GPIO_TypeDefStructure;
	USART_InitTypeDef USART_TypeDefStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);
	
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_USART6);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_USART6);
	
	//PC6-Tx
	GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_AF;		//���ù���
	GPIO_TypeDefStructure.GPIO_OType = GPIO_OType_PP;   //�������
	GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;     //����
	GPIO_TypeDefStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOC,&GPIO_TypeDefStructure);
	
/*	//PC7-RX
	GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_AF;		//����
	GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP; 	//����
	GPIO_Init(GPIOC,&GPIO_TypeDefStructure);*/
	
	
	USART_TypeDefStructure.USART_BaudRate = baudrate;					   //������
	USART_TypeDefStructure.USART_HardwareFlowControl = 				       //��Ӳ��������
												 USART_HardwareFlowControl_None;  
	USART_TypeDefStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx; 		//�����뷢��ģʽ
	USART_TypeDefStructure.USART_Parity = USART_Parity_No; 		       		//��У��λ
	USART_TypeDefStructure.USART_StopBits = USART_StopBits_1;        		//ֹͣλ1
	USART_TypeDefStructure.USART_WordLength = USART_WordLength_8b;   		//����λ8λ
	USART_Init(USART6,&USART_TypeDefStructure);
	
	USART_Cmd(USART6,ENABLE);
	USART_ClearFlag(USART6, USART_FLAG_TC);						//���������ɱ�־λ
	USART_ClearFlag(USART6, USART_FLAG_RXNE);					//���������ɱ�־λ

}


//����һ���ַ�
int U6SendChar(int ch, unsigned int WaitTime) 
{
while(!(USART_GetFlagStatus(USART6,USART_FLAG_TXE)))
{
if(USART_GetFlagStatus(USART6,USART_FLAG_ORE)==SET)
        {
					Send_Debug_Info("ore\n",13);
                  USART_ClearFlag(USART6,USART_FLAG_ORE); //��SR��ʵ���������־
                  (uint8_t)USART_ReceiveData(USART6);
								
         }
};
	USART_SendData(USART6,ch&0x1FF);
	return (ch);

	
	

}
/*********************************************************************
���� �� ������SYN7318_Put_Char----����һ���ֽں���
������˵������txd---�����͵��ֽڣ�8λ��
����    ������SYN7318_Put_Char('d');  ���͡�d��
*********************************************************************/
void SYN7318_Put_Char(uint8_t txd)
{
	  U6SendChar( txd ,10000);
}

/*********************************************************************
���� �� ������SYN7318_Put_String----�����ַ�������
������˵������Pst������ַ�����������
              Length���ַ�������
����    ������uchar d[4] = {0x00,0x01,0x02,0x03};
              SYN7318_Put_String(d,4); ---��������d�е�Ԫ��
*********************************************************************/
void SYN7318_Put_String(uint8_t* Pst,uint8_t Length)
{ 
   	uint8_t i;
	for(i = 0; i < Length; i++)
	{
		  SYN7318_Put_Char( Pst[i]);
	}
}

/*********************************************************************
���� �� ������SYN7318_Get_char----����һ���ֽں���
������˵�������޲�
���� �� ֵ�������յ����ֽ�
����    ������uchar d;
              d = SYN7318_Get_char();
              SYN7318_Get_char(d); -----������յ����ֽ�
*********************************************************************/

uint8_t SYN7318_Get_char(void) 
{
 	unsigned int return_data=0;	
	while(!(USART_GetFlagStatus(USART6,USART_FLAG_RXNE)))
	{
		 if(USART_GetFlagStatus(USART6,USART_FLAG_ORE)==SET)
        {
					Send_Debug_Info("ore\n",13);
                  USART_ClearFlag(USART6,USART_FLAG_ORE); //��SR��ʵ���������־
                  (uint8_t)USART_ReceiveData(USART6);
								
         }
	};
	return_data = (uint8_t)USART_ReceiveData(USART6);
	if(USART_GetFlagStatus(USART6,USART_FLAG_RXNE) == SET){
		Send_Debug_Info("RXNE\n",6);
		 USART_ClearFlag(USART6,USART_FLAG_RXNE);
	}
	return return_data;	
}

/*********************************************************************
���� �� ������SYN7318_Get_String----�����ַ�������
������˵������Pst����Ž��յ����ַ�����������
              Length���ַ�������
����    ������uchar d[4] = {0};
              SYN7318_Get_String(d,4);
              SYN7318_Get_String(d,4);----������յ����ַ��� 
*********************************************************************/
void SYN7318_Get_String(unsigned char *Pst,unsigned char Length)
{
  unsigned char i;
  for(i=0;i<Length;i++)
  {
     Pst[i] = SYN7318_Get_char();
  }
}

uint8_t  SYN7318_Rst(void)  //����ģ�鸴λ
{
	SYN7318_RST_H;	
	delay_ms(10); 
	SYN7318_RST_L;	
	delay_ms(100);	
	SYN7318_RST_H;  
	
	while ( 0x55 == SYN7318_Get_char() );
	SYN7318_Get_String(Back ,3 );
	if(Back[2] ==0x4A )  return 1;
	else return 0;
	
}



void SYN7318_Init(void)
{
	USART6_Init(115200);
	
	GPIO_InitTypeDef  GPIO_TypeDefStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);

	//PB9 -- SYN7318_RESET
	GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_OUT;	//���ù���
	GPIO_TypeDefStructure.GPIO_OType = GPIO_OType_PP;   //�������
	GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;     //����
	GPIO_TypeDefStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&GPIO_TypeDefStructure);
	
	GPIO_SetBits(GPIOB,GPIO_Pin_9);						//Ĭ��Ϊ�ߵ�ƽ
	
}
/*********************************************************************
���� �� ������SYN_TTS----�����ϳɲ��ź���
������˵������Pst�����Ҫ�ϳɲ��ŵ��ı���������
����    ������uchar Data[] = {"��������"};
              SYN_TTS(Data); -----�ϳɲ��ű�������
*********************************************************************/
void SYN_TTS(uint8_t *Pst)
{
	uint8_t Length;
	uint8_t Frame[5];   //���淢�����������
	
	Length = strlen(Pst); 
	Frame[0] = 0xFD;      //֡ͷ
	Frame[1] = 0x00;
	Frame[2] = Length+2;
	Frame[3] = 0x01;      //�����ϳɲ�������
	Frame[4] = 0x00;      //���ű����ʽΪ��GB2312��
	
	SYN7318_Put_String(Frame, 5);	
	SYN7318_Put_String(Pst, Length);
	
	SYN7318_Get_String(Back,4);	
	//���ճɹ�
	while(!(Back[3] == 0x41))
	{
		SYN7318_Get_String(Back,4);
	}
		Send_Debug_Info("receive\n",9);
	//���м��
	SYN7318_Get_String(Back,4);
	while(!(Back[3] == 0x4f))
	{
		SYN7318_Get_String(Back,4);
	}	
	Send_Debug_Info("null\n",6);
}


/*********************************************************************
���� �� ������Start_ASR----��ʼ����ʶ����
������˵������Dict���ʵ���
����    ������Start_ASR(0x00); ----ʶ��0x00�ʵ��еĴ���
*********************************************************************/
void Start_ASR(uint8_t Dict)
{

	uint8_t Frame[5];   //���淢�����������
	
	Frame[0] = 0xFD;      //֡ͷ
	Frame[1] = 0x00;
	Frame[2] = 0x02;   
	Frame[3] = 0x10;      //��ʼ����ʶ������
	Frame[4] = Dict;      //�ʵ���  �������޸���Ҫʶ��Ĵʵ���
	
	ASR[3]=0;
	
	SYN7318_Put_String(Frame, 5);	
	SYN7318_Get_String(Back,4); 
	if(Back[3] == 0x41)
	{
		 SYN7318_Get_String(Back,3);  //����ʶ������ش����
		if(Back[0] == 0xfc)
		{
			 SYN7318_Get_String(ASR,Back[2]);
		}
	}
}

/*********************************************************************
���� �� ������Stop_ASR----ֹͣ����ʶ����
������˵�������޲�
*********************************************************************/
void Stop_ASR()
{
	uint8_t Frame[4];   //���淢�����������
	
	Frame[0] = 0xFD;      //֡ͷ
	Frame[1] = 0x00;
	Frame[2] = 0x01;   
	Frame[3] = 0x11;      //ֹͣ����ʶ������
	
  SYN7318_Put_String(Frame, 4);	
  SYN7318_Get_String(Back,4); 

}

/***************************************************************************
���� �� ������Status_Query----ģ��״̬��ѯ����
������˵�������޲�
����    ������Status_Query(); 
***************************************************************************/
void Status_Query()
{
	uint8_t Frame[4];   //���淢�����������
	
	Frame[0] = 0xFD;    //֡ͷ
	Frame[1] = 0x00;
	Frame[2] = 0x01;   
	Frame[3] = 0x21;   //״̬��ѯ����
	
	SYN7318_Put_String(Frame, 4);	
	Send_Debug_Info("query_put\n",13);
	SYN7318_Get_String(Back,4);
Send_Debug_Info("query_get\n",13);	
	if(Back[3] == 0x41)  
	{
		 SYN7318_Get_String(S,4);  //ģ�鵱ǰ����״̬�Ļش����
	}
}

void SYN7318_Test( void)  // ������������
{   	
	 Ysn7813_flag = 1;
	// SYN7318_Init();
		
		 SYN_TTS("����ʶ�����,�뷢�������Ѵʣ�������ʻ");
		  LED1 = 1;
		  Status_Query();  //��ѯģ�鵱ǰ�Ĺ���״̬
			if(S[3] == 0x4F)  //ģ����м���������
			{
				LED2 = 1;
				delay_ms(1);
				
				SYN7318_Put_String(Wake_Up,5);//���ͻ���ָ��
				SYN7318_Get_String(Back,4);   //���շ�����Ϣ
				if(Back[3] == 0x41)         //���ճɹ�
				{ 
					LED3 = 1;
					SYN7318_Get_String(Back,3); //����ǰ��λ�ش�����
					if(Back[0] == 0xfc)       //֡ͷ�ж�
					{
						LED4 = 1;
						SYN7318_Get_String(ASR,Back[2]);//���ջش�����
						if(ASR[0] == 0x21)            //���ѳɹ�
						{
							SYN7318_Put_String(Play_MP3,33);//���š������⡱ 

							SYN7318_Get_String(Back,4);
							SYN7318_Get_String(Back,4);
							while(!(Back[3] == 0x4f))    //�ȴ�����
							{
								LED2 = ~LED2;
								delay_ms(500);
							}
//							Ysn7813_flag = 1;
         
							//��ʼ����ʶ��
							while(Ysn7813_flag)
							{
								
								SYN7318_Put_String(Start_ASR_Buf,5);//������ʶ������
								SYN7318_Get_String(Back,4);     //���շ�����Ϣ
								if(Back[3] == 0x41)           //���ճɹ�
								{
									LED1 = ~LED1;             //LED1��ת
									SYN7318_Get_String(Back,3);  //����ʶ������ش����
									if(Back[0] == 0xfc)        //֡ͷ�ж�
									{
										LED2 = ~LED2;
										SYN7318_Get_String(ASR,Back[2]);//���ջش�����	                    
										Yu_Yin_Asr();
									}
								}
							}
							SYN7318_Put_String(Stop_Wake_Up,4);//����ֹͣ����ָ��
						}
						else                          //�����ڲ�����
						{
						
						}
					}
				}				
		  }
}


void Yu_Yin_Asr(void)  // ����ʶ������
{
			switch(ASR[0])
				 {
						case 0x01:
							{
									switch(ASR[3])
											{
												case 0x00:  // ��һ����һ�㲻����ʶ��
													{
										  				  SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
															  break;
													}
													case 0x01:  // ǰ��
													{
//															Go_Test( 50,50);
															SYN_TTS("С��ǰ�������");
														  SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
															break;
													}
													case 0x02:
													{

//															Back_Test(50,50);
													 		SYN_TTS("С����������ɣ��ȴ���һ��ָ��");
														  SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ�� 
															break;
													}
													case 0x03:
													{
 
//														  Left_Test( 80);
															SYN_TTS("С����ת��ɣ��ȴ���һ��ָ��");
														  SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��;
															break;
													}
													case 0x04:
													{
													   
													   
//															Right_Test( 80);
															SYN_TTS("С����ת��ɣ��ȴ���һ��ָ��");
														  SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
															break;
													}
													case 0x05:
													{
														  													    
//															 Stop_Test();
															 SYN_TTS("��ͣ�����ȴ���һ��ָ��");
														    SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
															 break;
													}
													case 0x06:
													{
//															  Track_Test( 50);
																SYN_TTS("Ѱ������ɣ��ȴ���һ��ָ��");
																SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
															  break;
													}
													case 0x07:
													{
															Infrared_Send(HW_K,6);	//�򿪲��Ժ��ⱨ��
														    SYN_TTS("�������Ѵ򿪣��ȴ���һ��ָ��");
														    SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
															break;
													}
													case 0x08:
													{
															Send_ZigbeeData_To_Fifo(DZ_K ,8);  // ������բ
														    SYN_TTS("��բ�Ѵ򿪣��ȴ���һ��ָ��");
														    SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
															 break;
													}
													case 0x09:
													{
														    
																Send_ZigbeeData_To_Fifo( DZ_G ,8);  // ������բ
														    SYN_TTS("��բ�ѹرգ��ȴ���һ��ָ��");
														    SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
															  break;
													}
													case 0x0a:
													{
																Send_ZigbeeData_To_Fifo( SMG_SHOW ,8);  // �������ʾ
														    SYN_TTS("LED��ʾ�ѿ����ȴ���һ��ָ��");
																SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��										
															  break;
													}
													case 0x0b:
													{
  															Send_ZigbeeData_To_Fifo( SMG_JSK ,8);  // ����ܼ�ʱ
														    SYN_TTS("��ʱϵͳ�Ѵ򿪣��ȴ���һ��ָ��");
 														    SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
															  break;
													}
													case 0x0c:
													{
														  
															Send_ZigbeeData_To_Fifo( SMG_JSG ,8);  // ����ܹر�
														  SYN_TTS("��ʱϵͳ�ѹرգ��ȴ���һ��ָ��");
                              SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
															break;
													}
													case 0x0d:
													{
														  
															Send_ZigbeeData_To_Fifo( SMG_JL ,8);  // �������ʾ����
														  SYN_TTS("LED��ʾ��������ɣ��ȴ���һ��ָ��");
                                  SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
															break;
													}
													case 0x0e:
													{
									//						LED_L =0;  // �����
														  SYN_TTS("�����ʾ���Ѵ򿪣��ȴ���һ��ָ��");
														SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��

															break;
													}
													case 0x0f:
													{
									//						LED_R =0;  // ���ҵ�
														  SYN_TTS("�Ҳ���ʾ���Ѵ򿪣��ȴ���һ��ָ��");
														  SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��														
															break;
													}
													case 0x10:
													{

									//						LED_L =1;  // �ر���ʾ��
									//						LED_R =1;
														  SYN_TTS("��ʾ���ѹرգ��ȴ���һ��ָ��");
                              SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
															break;
													}
													case 0x11:
													{
														  
									//						BEEP =0;  //��������
														  SYN_TTS("�������Ѵ򿪣��ȴ���һ��ָ��");
														  
															delay_ms(500);
									//						BEEP =1;
                              SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
															break;
													}
													case 0x12:
													{
														  
									//						BEEP =1;  //�ط�����
														  SYN_TTS("�������ѹرգ��ȴ���һ��ָ��");
														  SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
															break;
													}
													case 0x13:
													{
															Infrared_Send(CP_SHOW1,6);
															delay_ms(500);
															Infrared_Send(CP_SHOW2,6);
																
														  SYN_TTS("��������ʾ���ȴ���һ��ָ��");
															SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
															break;
													}
													case 0x14:
													{
															 Infrared_Send(H_SD,4);
																
														   SYN_TTS("����ŷ�ϵͳ�Ѵ򿪣��ȴ���һ��ָ��");
														  SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��

															break;
													}
													case 0x15:
													{
															Infrared_Send(H_S,4);
																
														  SYN_TTS("ͼƬ��ҳ��ɣ��ȴ���һ��ָ��");
														  SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��

															break;
													}
													case 0x16:
													{
														  
															Infrared_Send(H_1,4);
																
														  SYN_TTS("��[=tiao2]�⵵[=dang3]λ�Ѽ�1���ȴ���һ��ָ��");
                               SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
															break;
													}
													case 0x17:
													{
														  																										
														  SYN_TTS("��ӭʹ�ñ����ٿ���[=rong2]����������С��");
                               SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��					
															break;
													}
													case 0x18:
													{
														  																										
														  SYN_TTS("��ʼԭ�ص�ͷ�����Ժ�");
																			
/*															while(G_Flag);  //�ȴ�ǰ�����
															Left_Test( 80); 
															while(L_Flag);
															Left_Test( 80); 
															while(L_Flag);*/
															SYN_TTS("ԭ�ص�ͷ����ɣ��ȴ���һ��ָ��");
                              SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��	
															break;
													}
													case 0x19:
													{
														 
															SYN_TTS("�õģ�����������������"); 
														  SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
														  Ysn7813_flag = 0;
//Ysn7813_flag_cs =0;
															break;
													}
	                        case 0x1A:  // С��ǰ��
													{
														 

//															Go_Test( 80,10);
															SYN_TTS(" С��ǰ������ɣ��ȴ���һ��ָ��");
															SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
															break;
													}
													case 0x1B:   //  С������
													{
													  	SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ�� 

//															Back_Test(80,10);
															SYN_TTS("С����������ɣ��ȴ���һ��ָ��");	
															break;
													}
													case 0x1C:   //  С����ת
													{
														  
														  
//														   Left_Test( 80);
															 SYN_TTS("С����ת����ɣ��ȴ���һ��ָ��");
                               SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��;
															break;
													}
													case 0x1D:  // С����ת
													{
														  
//															Right_Test( 80);
															SYN_TTS("С����ת����ɣ��ȴ���һ��ָ��");
														SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��

															break;
													}
													case 0x1E:  // С��ֹͣ
													{
														  
														    
//															 Stop_Test();
															SYN_TTS("��ͣ�����ȴ���һ��ָ��");
														  SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
															break;
													}
													case 0x1F:   //  С��ѭ��
													{
														 

//															Track_Test( 50);
															SYN_TTS("С��Ѱ������ɣ��ȴ���һ��ָ��");
                              SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
															break;
													}	
                          case 0x20:   //  ����ת��
													{
														  

//															Right_Test( 80);
															SYN_TTS("��ת������ɣ��ȴ���һ��ָ��");
														  SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
															break;
													}														
                          case 0x21:   //  ��ֹ��ת
													{
														  
//															Left_Test( 80);
															SYN_TTS("��ֹ��ת��ִ����ת����ɣ��ȴ���һ��ָ��");
                              SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
															break;
														}
                          case 0x22:   //  ����ת��
													{
														  
//															Left_Test( 80);
															SYN_TTS("��ת������ɣ��ȴ���һ��ָ��");
                              SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
															break;
													}														
                          case 0x23:   //  ��ֹ��ת
													{
														  
//															Left_Test( 80);
															SYN_TTS("��ֹ��ת��ִ����ת����ɣ��ȴ���һ��ָ��");
                              SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
															break;													
													}
													default:
													{
														  
															SYN_TTS("�Բ�����û����");
													   	SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
															break;
													}
												}
                        SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
												break;
											}
											case 0x02: //ʶ��ɹ���������ID�ţ�
											{
												SYN_TTS("�Բ�����û����");
												SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
												break;
											}
											case 0x03://�û�������ʱ
											{
												SYN_TTS("�������ߣ���ʱΪ������");
												Ysn7813_flag = 0;
//												Ysn7813_flag_cs = 0;
												SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
												break;
											}
											case 0x04:
											{
												SYN_TTS("�밲�����鷳����˵һ��");
												SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
												break;
											}
											case 0x05:
											{
												SYN_TTS("�Բ�������˵һ��");
												SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
												break;
											}
											case 0x06:
											{
												SYN_TTS("ʶ���ڲ�����");
												SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
												break;
											}
											case 0x07:
											{
												SYN_TTS("�Բ�������˵һ��");
												SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
												break;
											}
											default:
											{
												SYN_TTS("����");
												SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
												break;
											}
									}

}


unsigned char Start_ASR_One_Buf[] = {0xFD,0x00,0x02,0x10,0x04};//  ���β��� ʹ���û��ʵ�2
static u8 YY_HXC[8]={0x55 ,0x06 ,0x10 ,0x01 ,0x00 ,0x00 ,0x11 ,0xbb};  // ���Ѵ�
static u8 YY_ZZW[8]={0x55 ,0x06 ,0x10 ,0x04 ,0x00 ,0x00 ,0x14 ,0xbb};  // ��ת��
static u8 YY_SJ [8]={0x55 ,0x06 ,0x20 ,0x01 ,0x00 ,0x00 ,0x21 ,0xbb};  // ��ת��

/****************************************************************
�������ܣ�ȫ�Զ�����ʶ��
��	  ��: mode =  1 ���ָ��  mode != 1 ָ������  num = ָ������ָ��
����  ֵ����
*****************************************************************/
void SYN_7318_One_test(u8 mode,u8 num)  // mode ģʽ���� 1 ���ָ�� 0 ָ��ָ��
{
  u8 error_num=0;
	Ysn7813_flag =1;
	while(Ysn7813_flag)
	{	  
		SYN7318_Init();
		delay_ms(700);
		Send_Debug_Info("initOk||",13);
		Status_Query();  //��ѯģ�鵱ǰ�Ĺ���״̬	
		Send_Debug_Info("query\n",13);
		Send_Debug_Info("initSuccess||",12);
		if(S[3] != 0x4F)  //ģ����м���������  
		{ 
			 SYN_TTS("ģ��æµ");	
			 while( !SYN7318_Rst());
			Send_Debug_Info("ml\n",4);
			 SYN7318_Init();				
			 delay_ms(1000);
			 SYN_TTS("ģ�鸴λ�ɹ���");
		}
		delay_ms(500);
		SYN7318_Put_String(Start_ASR_One_Buf,5);//������ʶ������  // �û��ʵ� 2  Start_ASR_One_Buf
		Send_Debug_Info("putSuccess||",11);
		SYN7318_Get_String(Back,4);     //���շ�����Ϣ
		Send_Debug_Info("receSuccess||",12);
		if(Back[3] == 0x41)           //���ճɹ�
		{
			LED1 = ~LED1;             //LED1��ת
			delay_ms(1000);
			if(mode) 
			{
				YY_ZZW[3] = num;
				Send_ZigBee_Info( YY_SJ,8); // �������
			}
			else  
			{
				YY_ZZW[3] = num;
				Send_ZigBee_Info( YY_ZZW,8); // ָ������
			}
				
			SYN7318_Get_String(Back,3);  //����ʶ������ش����
			Send_Debug_Info("rereceSuccess||",14);
			if(Back[0] == 0xfc)        //֡ͷ�ж�
			{
				LED2 = ~LED2;
				SYN7318_Get_String(ASR,Back[2]);//���ջش�����

				if(ASR[0] ==0x01) 
				{
					switch(ASR[5]) // ����ID
					{
						case 0x19:   //  ԭ�ص�ͷ
					    {
							  SYN_TTS("ʶ����ԭ�ص�ͷ");
							  SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��							 
							  Ysn7813_flag =0;
								
								Send_SYN[2] = 0x06;
								Send_ZigBee_Info(Send_SYN,8);
							 break;
						}														
						case 0x21:   //  ����ת��
						{								 
							 SYN_TTS("ʶ��������ת��");						  
							 SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��							 
							 Ysn7813_flag =0;
							
							 Send_SYN[2] = 0x02;
							 Send_ZigBee_Info(Send_SYN,8);
							
							 break;
						}														
                        case 0x22:   //  ��ֹ��ת
						{														  															
							SYN_TTS("ʶ������ֹ��ת");
              SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��							
							Ysn7813_flag =0;
							
							Send_SYN[2] = 0x03;
							Send_ZigBee_Info(Send_SYN,8);
							break;
						}
                        case 0x23:   //  �����ʻ
						{
							SYN_TTS("ʶ���������ʻ");
              SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��							
							Ysn7813_flag =0;
									
							Send_SYN[2] = 0x04;
							Send_ZigBee_Info(Send_SYN,8);
							break;
						}														
                        case 0x24:   //  ���б���
						{														  	
							SYN_TTS("ʶ�������б���");
              SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��							
							Ysn7813_flag =0;
							
							Send_SYN[2] = 0x05;
							Send_ZigBee_Info(Send_SYN,8);
							break;													
						}
						default:
						{														  
							SYN_TTS("ʶ��ʧ�ܣ���������");
							SYN7318_Put_String(Stop_ASR_Buf,4); //ֹͣ����ʶ��
							//Ysn7813_flag =0;
							break;
						}
					}
				}
				else SYN_TTS("���Ժ�ģ���쳣");     
			}
		}			
		/*************************************************/
		//SYN7318_Put_String(Stop_Wake_Up,4);//����ֹͣ����ָ��
		Send_Debug_Info("headFire||",10);
		delay_ms(500);
		LED3 = !LED3;
       error_num++;	
      if( error_num >5)  	Ysn7813_flag =0;	
	}			
}


























