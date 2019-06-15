#include <stdint.h>
#include "stm32f4xx.h"
#include "canP_HostCom.h"
#include "delay.h"
#include "data_channel.h"
#include "roadway_check.h"
#include "rc522.h"

#define __CAN_USER_C__
#include "can_user.h"
#include "tba.h"
#include "data_base.h"
#include "3399_uart.h"
#include "cba.h"
#include "infrared.h"
#include "syn7318.h"
#include <string.h>
#include "Timer.h"
#include "bh1750.h"
#include "uart_a72.h"
#include "ultrasonic.h"

uint8_t Host_AGV_Return_Flag = RESET;
uint8_t AGV_data_Falg = RESET;

uint32_t canu_wifi_rxtime = 0;
uint32_t canu_zibe_rxtime = 0;

//�Զ����ٶ�&����
u8 Go_Sp = 60;
u8 Back_Sp = 60;
u8 Track_Sp = 60;
u8 Whell_Sp = 90;
u16 Go_Mp = 380;
u16 Back_Mp = 50;

u16 csb_1 = 0;
u16 csb_2 = 0;
u16 csb_3 = 0;

u8 WiFi_Data[12];
u8 ZigBee_Data[12];

u8 Car_Data[6];
u8 XZ_Data[9];

u8 AGV_CT;

u8 TEST_AGV[1];

u8 Clean[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
//�Զ����־λ
uint32_t Full_Automatic_Flag =0;
uint32_t Full_Automatic_Start = 0;
uint16_t Full_Automatic_Delay = 300; //100
uint16_t Full_Automatic_Delay_back = 700;
uint16_t Full_Automatic_Delay_turn = 700;
u16 Full_Send_Delay = 500;



void Can_WifiRx_Save(uint8_t res)
{
	if(Wifi_Rx_flag == 0)
	{
		canu_wifi_rxtime = gt_get()+10;
		Wifi_Rx_num =0;
		Wifi_Rx_Buf[Wifi_Rx_num]=res;
		Wifi_Rx_flag = 1;
	}
	else if(Wifi_Rx_num < WIFI_MAX_NUM )	
	{
		Wifi_Rx_Buf[++Wifi_Rx_num]=res;	 
	}
}

uint8_t Rx_Flag ;

void Normal_data(void)	  // ��������8�ֽڿ���ָ��
{
	u8 sum=0;

	if(Wifi_Rx_Buf[7]==0xbb)	 // �жϰ�β
	 {									  
		//��ָ������λ��ָ�������У��
		//ע�⣺��������ʱӦ�öԺ���256ȡ�ࡣ
		 sum=(Wifi_Rx_Buf[2]+Wifi_Rx_Buf[3]+Wifi_Rx_Buf[4]+Wifi_Rx_Buf[5])%256;
		if(sum == Wifi_Rx_Buf[6])
		{
			   Rx_Flag =1;
		}
		else Rx_Flag =0;
	}
}

void Abnormal_data(void)	  //�����쳣����
{
	u8 i,j;
	u8 sum=0;
	
	if(Wifi_Rx_num <8)			// �쳣�����ֽ���С��8�ֽڲ�������
	{
	   Rx_Flag =0;
	}
	else {
		for(i=0;i<=(Wifi_Rx_num -7);i++)  
		{
			if(Wifi_Rx_Buf[i]==0x55)	   // Ѱ�Ұ�ͷ
			{
			   if(Wifi_Rx_Buf[i+7]==0xbb)	// �жϰ�β
			   {
			   	    sum=(Wifi_Rx_Buf[i+2]+Wifi_Rx_Buf[i+3]+Wifi_Rx_Buf[i+4]+Wifi_Rx_Buf[i+5])%256;

		           if(sum==Wifi_Rx_Buf[i+6])	 // �ж����
	              {
			         for(j=0;j<8;j++)
					 {
					 	 Wifi_Rx_Buf[j]=Wifi_Rx_Buf[j+i];	 // ���ݰ���
					 }
					    Rx_Flag =1;
		          }
	  	         else Rx_Flag =0;
		       }
			}
	    }

	}
}

uint8_t Infrared_Tab[6];			//�������ݴ������

void Can_WifiRx_Check(void)
{
	if(Wifi_Rx_flag)
	{	
		if(gt_get_sub(canu_wifi_rxtime) == 0)
		{
			if(Wifi_Rx_Buf[0]==0xFD)  
			{			
				Send_ZigbeeData_To_Fifo( Wifi_Rx_Buf , (Wifi_Rx_num +1)); 
			}
			else if(Wifi_Rx_Buf[0]==0x55)  
			{              
				Normal_data();  
			}
			else
			{
				Abnormal_data();      
			} 	
			Wifi_Rx_flag = 0;
		}
	}
	if(Rx_Flag ==1)	
	{    
		if(Wifi_Rx_Buf[1] == 0xAA) 	   
		{	
			switch(Wifi_Rx_Buf[2])
			{
			case 0x01:              	//ֹͣ
				Send_UpMotor(0 ,0);
				Roadway_Flag_clean();	//�����־λ״̬	
				break;
			case 0x02:              //ǰ��
				Roadway_mp_syn();	//����ͬ��
				Stop_Flag = 0; Go_Flag = 1; wheel_L_Flag = 0;wheel_R_Flag = 0;wheel_Nav_Flag = 0;
				Back_Flag = 0; Track_Flag = 0;
				temp_MP = Wifi_Rx_Buf[5];
				temp_MP <<=  8;
				temp_MP |= Wifi_Rx_Buf[4];
				Car_Spend = Wifi_Rx_Buf[3];
				//set_Test_Times();
				Control(Car_Spend ,Car_Spend);
				break;
			case 0x03:              //����
				Roadway_mp_syn();	//����ͬ��
				Stop_Flag = 0; Go_Flag = 0; wheel_L_Flag = 0;wheel_R_Flag = 0;wheel_Nav_Flag = 0;
				Back_Flag = 1; Track_Flag = 0;
				temp_MP = Wifi_Rx_Buf[5];
				temp_MP <<=  8;
				temp_MP |= Wifi_Rx_Buf[4];
				Car_Spend = Wifi_Rx_Buf[3];
				Control(-Car_Spend ,-Car_Spend);					
				break;
			case 0x04:              //��ת
				Stop_Flag = 0; Go_Flag = 0; wheel_L_Flag = 1;wheel_R_Flag = 0;wheel_Nav_Flag = 0;
				Back_Flag = 0; Track_Flag = 0;
				Car_Spend = Wifi_Rx_Buf[3];				
				Control(-Car_Spend ,Car_Spend);
				break;
			case 0x05:              //��ת
				Stop_Flag = 0; Go_Flag = 0; wheel_L_Flag = 0;wheel_R_Flag = 1;wheel_Nav_Flag = 0;
				Back_Flag = 0; Track_Flag = 0;
				Car_Spend = Wifi_Rx_Buf[3];
				Control(Car_Spend,-Car_Spend);
				break;
			case 0x06:              //ѭ��
				Stop_Flag = 0; Go_Flag = 0; wheel_L_Flag = 0;wheel_R_Flag = 0;wheel_Nav_Flag = 0;
				Back_Flag = 0; Track_Flag = 1;
				Car_Spend = Wifi_Rx_Buf[3];
				//set_Test_Times();
				break;
			case 0x07:				//��������
				break;
			case 0x08:				//��ת��--�Ƕ�											
				Roadway_nav_syn();	//�Ƕ�ͬ��
				Roadway_mp_syn();	//����ͬ��
				Stop_Flag = 0; Go_Flag = 0; wheel_L_Flag = 0;wheel_R_Flag = 0;wheel_Nav_Flag = 1;
				Back_Flag = 0; Track_Flag = 0;
				temp_Nav = Wifi_Rx_Buf[5];
				temp_Nav <<=  8;
				temp_Nav |= Wifi_Rx_Buf[4];
				Car_Spend = Wifi_Rx_Buf[3];				
				Send_UpMotor(-Car_Spend ,Car_Spend);					
				break;
			case 0x09:				//��ת��--�Ƕ�			
				Roadway_nav_syn();	//�Ƕ�ͬ��
				Roadway_mp_syn();	//����ͬ��
				Stop_Flag = 0; Go_Flag = 0; wheel_L_Flag = 0;wheel_R_Flag = 0;wheel_Nav_Flag = 1;
				Back_Flag = 0; Track_Flag = 0;
				temp_Nav = Wifi_Rx_Buf[5];
				temp_Nav <<=  8;
				temp_Nav |= Wifi_Rx_Buf[4];
				Car_Spend = Wifi_Rx_Buf[3];
				Send_UpMotor(Car_Spend,-Car_Spend);
				break;
			case 0x10:										//����ǰ��λ����
				Infrared_Tab[0]=Wifi_Rx_Buf[3];
				Infrared_Tab[1]=Wifi_Rx_Buf[4];
				Infrared_Tab[2]=Wifi_Rx_Buf[5];
				break;
			case 0x11:										//�������λ����
				Infrared_Tab[3]=Wifi_Rx_Buf[3];//���ݵ���λ
				Infrared_Tab[4]=Wifi_Rx_Buf[4];//��λУ����
				Infrared_Tab[5]=Wifi_Rx_Buf[5];//��λУ����
				break;
			case 0x12:										//֪ͨС����Ƭ�����ͺ�����
				Infrared_Send(Infrared_Tab,6);
				break;									
			case 0x20:	//ת��ƿ���
				Set_tba_WheelLED(L_LED,Wifi_Rx_Buf[3]);
				Set_tba_WheelLED(R_LED,Wifi_Rx_Buf[4]);
				break;					
			case 0x30:
				Set_tba_Beep(Wifi_Rx_Buf[3]);				//������
				break;
			case 0x40:										//��δʹ��
				break;
			case 0x50:  				//���ⷢ�������Ƭ�Ϸ� 
				Infrared_Send(H_S,4);
				break;
			case 0x51:    					//���ⷢ�������Ƭ�·� 
				Infrared_Send(H_X,4);
				break;
			case 0x61:    					//���ⷢ����ƹ�Դǿ�ȵ�λ��1 
				Infrared_Send(H_1,4);
				break;
			case 0x62:   	 				//���ⷢ����ƹ�Դǿ�ȵ�λ��2 
				Infrared_Send(H_2,4);
				break;
			case 0x63:    					//���ⷢ����ƹ�Դǿ�ȵ�λ��3 
				Infrared_Send(H_3,4);
				break;
		   case 0x80:						//�˶���־�����ݷ�������λ
			    Host_AGV_Return_Flag = Wifi_Rx_Buf[3];   //SET ���� / RESET ��ֹ
				break;
			 /**
			 �Զ���Э�鿪ʼ
			 */
			 case 0x66://��ʼȫ�Զ�-���峵��
				  AGV_CT = 1;
					TEST_AGV[0] = 0x01;
					Full_Automatic_Start = 1;
			 break;
			 
			 case 0x77://��ʼȫ�Զ�-�������
				  AGV_CT = 2;
					TEST_AGV[0] = 0x02;
					Full_Automatic_Start = 1;
			 break;
			 
			 case 0x88://��ʼȫ�Զ�-���̨
				  AGV_CT = 3;
					TEST_AGV[0] = 0x03;
					Full_Automatic_Start = 1;
			 break;
			 
			 case 0x99://��ʼȫ�Զ�-��ά��
				  AGV_CT = 4;
					TEST_AGV[0] = 0x04;
					Full_Automatic_Start = 1;
			 break;
			 
			 
		   default:
			   break;
			}
		}
		else  
		{
			Send_ZigbeeData_To_Fifo(Wifi_Rx_Buf,8); 
		}
		Rx_Flag = 0;
	}
}




/**
�������ܣ�����ZigBee����
��    ��: ��
�� �� ֵ����
*/
void Can_ZigBeeRx_Save(uint8_t res)
{
/*	if(Zigbee_Rx_flag == 0)
	{
		Zigb_Rx_Buf[Zigbee_Rx_num]=res;
		Zigbee_Rx_num++;
		if(Zigbee_Rx_num > ZIGB_RX_MAX )	
		{
			Zigbee_Rx_num = 0;
			Zigbee_Rx_flag = 1;
		} 
	} */

	if(Zigbee_Rx_flag == 0)
	{
		canu_zibe_rxtime = gt_get()+10;
		Zigbee_Rx_num =0;
		Zigb_Rx_Buf[Zigbee_Rx_num]=res;
		Zigbee_Rx_flag = 1;
	}
	else if(Zigbee_Rx_num < ZIGB_RX_MAX )	
	{
		Zigb_Rx_Buf[++Zigbee_Rx_num]=res;	 
	}
		
}


/**
�������ܣ�ZigBee���ݼ��
��    ������
�� �� ֵ����
*/
void Can_ZigBeeRx_Check(void)
{
	if(Zigbee_Rx_flag)
	{
		if(gt_get_sub(canu_zibe_rxtime) == 0)
		{
			if( (Zigb_Rx_Buf[1]==0x03) || (Zigb_Rx_Buf[1]==0x0c)) 	// ��բ �� ETC
			{
			   if(Zigb_Rx_Buf[2]==0x01)
			   {
					if(Zigb_Rx_Buf[3]==0x01)
					{
						Stop_Flag=Zigb_Rx_Buf[4] ;	  			 	// 0x05 ������բ���  0x06 ETC												
					}							
			   }		
			}
			else if((Zigb_Rx_Buf[1]==0x0E)&&(Zigb_Rx_Buf[2]==0x01))	//��ͨ�Ʊ�־��					
			{
				Stop_Flag = Zigb_Rx_Buf[4] ;	  						// 0x07����ʶ��ģʽ 0x08 δ����ʶ��ģʽ
			} 
			else if((Zigb_Rx_Buf[1]==0x0D)&&(Zigb_Rx_Buf[2]==0x03)) //���峵���־��
			{
				if(Zigb_Rx_Buf[3] == 0x01)							//��ȡ���峵�⵱ǰ����
				{
					switch(Zigb_Rx_Buf[4])							
					{
						case 1:
							Stop_Flag = 0x09;						//��һ��
							break;
						case 2:
							Stop_Flag = 0x0A;						//�ڶ���
							break;
						case 3:
							Stop_Flag = 0x0B;						//������
							break;
						case 4:
							Stop_Flag = 0x0C;						//���Ĳ�
							break;
					}
				} 
				else if(Zigb_Rx_Buf[3] == 0x02) 					//��ȡ���峵�⵱ǰ����״̬
				{
					if((Zigb_Rx_Buf[4] == 0x01) && (Zigb_Rx_Buf[5] == 0x01))
					{
						Stop_Flag = 0x11;							//ǰ����ⴥ���������ⴥ��
					} 
					else if((Zigb_Rx_Buf[4] == 0x02) && (Zigb_Rx_Buf[5] == 0x02))
					{
						Stop_Flag = 0x22;							//ǰ�����δ������������δ����
					}
					else if((Zigb_Rx_Buf[4] == 0x01) && (Zigb_Rx_Buf[5] == 0x02))
					{
						Stop_Flag = 0x12;							//ǰ����ⴥ����������δ����
					}
					else if((Zigb_Rx_Buf[4] == 0x02) && (Zigb_Rx_Buf[5] == 0x01))
					{
						Stop_Flag = 0x21;							//ǰ�����δ�����������ⴥ��
					}
				}
			}
			else if((Zigb_Rx_Buf[0] == 0x55)&&(Zigb_Rx_Buf[1] == 0x02))		//���شӳ�����
			{		
					memcpy(Follower_Tab,Zigb_Rx_Buf,50);
					AGV_data_Falg = SET;
			}
			else if(Zigb_Rx_Buf[1]==0x06)	 //������������
			{
				if(Zigb_Rx_Buf[2]==0x01)
				{
						Stop_Flag=Zigb_Rx_Buf[3] ;	  // ����оƬ״̬����
				}			
			}
			Zigbee_Rx_flag = 0;
		}	
	}
}


/** ��δʹ��
�������ܣ�����ѭ���ϴ�����ʱ��
��    ������
�� �� ֵ����
*/
void Canuser_upTrackTime(void)
{
	static uint8_t run_mode = 0;
	
	if(gt_get() < 1000)
		run_mode = 0;
	else if(run_mode == 0)
	{
		run_mode = 1;
		Host_Set_UpTrack(50);
	}
}

#define UART_or_CAN    0   //  0---UART  1---CAN

/**
�������ܣ�CAN��ѯ�����ͽ��ռ��
��    ������
�� �� ֵ����
*/
void Canuser_main(void)
{
	CanP_Host_Main();
	CanP_CanTx_Check();				//CAN���߷������ݼ��
}



//end file
/**
�������ܣ��������ݵ�����&WiFi
��    ����data ��Ҫ���͵�����
�� �� ֵ����
*/
void Send_Android_Info(uint8_t *data)
{
				UartA72_TxClear();
				UartA72_TxAddStr(data,12);
				UartA72_TxStart();
		
				Send_WifiData_To_Fifo(data,12);
}
/**
�������ܣ���ѭ����ǰ��
��    ������
�� �� ֵ����
*/
void Full_Automatic_GO_With_Track(uint16_t sp,uint16_t en)
{
		Roadway_mp_syn();	//����ͬ�� ǰ��
		Stop_Flag = 0; Go_Flag = 0; wheel_L_Flag = 0;wheel_R_Flag = 0;wheel_Nav_Flag = 0;
		Back_Flag = 0; Track_Flag = 0;
		Go_With_Track_Flag = 1;
		temp_MP = en;
		Car_Spend = sp;
		Control(Car_Spend,Car_Spend);
		while(Go_With_Track_Flag==1);
		delay_ms(Full_Automatic_Delay);
}
/**
�������ܣ�ǰ��
��    ������
�� �� ֵ����
*/
void Full_Automatic_GO(uint16_t sp,uint16_t en)
{
		Roadway_mp_syn();	//����ͬ�� ǰ��
		Stop_Flag = 0; Go_Flag = 1; wheel_L_Flag = 0;wheel_R_Flag = 0;wheel_Nav_Flag = 0;
		Back_Flag = 0; Track_Flag = 0;
		temp_MP = en;
		Car_Spend = sp;
		Control(Car_Spend,Car_Spend);
		while(Go_Flag==1);
		delay_ms(Full_Automatic_Delay);
}
/**
�������ܣ�������δ���
��    ������
�� �� ֵ����
*/
void Full_Automatic_Special()
{
		uint16_t Mp_Value_1 = CanHost_Mp;//��ʼ��¼����
		Full_Automatic_Track(50);
		delay_ms(200);
		uint16_t First_Mp_Value = CanHost_Mp -Mp_Value_1;//��ʼ��¼����
		if( First_Mp_Value >= 2300) //��ȥ��������ֵ
			{
				Full_Automatic_GO(Go_Sp,Go_Mp);
			}
		else
			{
				Full_Automatic_GO(50,20);
				Full_Automatic_Track(Track_Sp);
				Full_Automatic_GO(Go_Sp,Go_Mp);
			}
}
/**
�������ܣ�Ѱ��
��    ������
�� �� ֵ����
*/
void Full_Automatic_Track(uint16_t sp)
{
		Stop_Flag = 0; Go_Flag = 0; wheel_L_Flag = 0;wheel_R_Flag = 0;wheel_Nav_Flag = 0;
		Back_Flag = 0; Track_Flag = 1;
		Car_Spend = sp;
		while(Track_Flag==1)
		{
			if(Track_Special_Flag == 1)
			{
					Full_Automatic_GO(80,1350);
					Track_Flag = 1;
					Track_Special_Flag =0;
					Send_Debug_Info("SpeTrack\n",10);
			}
		}
		Send_Debug_Info("TrackEnd\n",10);
		delay_ms(Full_Automatic_Delay);
}
/**
�������ܣ�Ѱ��Ѱ��
��    ������
�� �� ֵ����
*/
void Full_Automatic_RFID_Track(uint16_t sp)
{
		Stop_Flag = 0; Go_Flag = 0; wheel_L_Flag = 0;wheel_R_Flag = 0;wheel_Nav_Flag = 0;
		Back_Flag = 0; Track_Flag = 0;
		RFID_Track_Flag = 1;
		Car_Spend = sp;
		while(RFID_Track_Flag == 1)
		{
//			if(RFID_Write_Flag == 1)
//			{
//					Go_Find_cards1();
//					Send_Debug_Info("RFIDTrack\n",11);
//			}
		}
		Send_Debug_Info("TrackEnd\n",10);
		delay_ms(Full_Automatic_Delay);
}
/**
�������ܣ���ת45
��    ������
�� �� ֵ����
*/
void Full_Right45()
{
		Roadway_mp_syn();	//����ͬ�� ǰ��
		Stop_Flag = 0; Go_Flag = 1; wheel_L_Flag = 0;wheel_R_Flag = 0;wheel_Nav_Flag = 0;
		Back_Flag = 0; Track_Flag = 0;
		temp_MP = 400;
		Car_Spend = 85;
		Control(Car_Spend,-Car_Spend);
		while(Go_Flag==1);
		delay_ms(Full_Automatic_Delay);
}
/**
�������ܣ���ת45
��    ������
�� �� ֵ����
*/
void Full_Left45()
{
		Roadway_mp_syn();	//����ͬ�� ǰ��
		Stop_Flag = 0; Go_Flag = 1; wheel_L_Flag = 0;wheel_R_Flag = 0;wheel_Nav_Flag = 0;
		Back_Flag = 0; Track_Flag = 0;
		temp_MP = 400;
		Car_Spend = 85;
		Control(-Car_Spend,Car_Spend);
		while(Go_Flag==1);
		delay_ms(Full_Automatic_Delay);
}
/**
�������ܣ���ת90
��    ������
�� �� ֵ����
*/
void Full_Right90()
{
		Roadway_mp_syn();	//����ͬ�� ǰ��
		Stop_Flag = 0; Go_Flag = 1; wheel_L_Flag = 0;wheel_R_Flag = 0;wheel_Nav_Flag = 0;
		Back_Flag = 0; Track_Flag = 0;
		temp_MP = 950;
		Car_Spend = 90;
		Control(Car_Spend,-Car_Spend);
		while(Go_Flag==1);
		delay_ms(Full_Automatic_Delay);
}
/**
�������ܣ���ת90
��    ������
�� �� ֵ����
*/
void Full_Left90()
{
		Roadway_mp_syn();	//����ͬ�� ǰ��
		Stop_Flag = 0; Go_Flag = 1; wheel_L_Flag = 0;wheel_R_Flag = 0;wheel_Nav_Flag = 0;
		Back_Flag = 0; Track_Flag = 0;
		temp_MP = 850;
		Car_Spend = 90;
		Control(-Car_Spend,Car_Spend);
		while(Go_Flag==1);
		delay_ms(Full_Automatic_Delay);
}
/**
�������ܣ�ǰ����ѭ��
��    ������
�� �� ֵ����
*/
void Full_Automatic_GO_And_Track(uint16_t go_sp,uint16_t go_en,uint16_t track_sp)
{
		Roadway_mp_syn();	//����ͬ�� ǰ��
		Stop_Flag = 0; Go_Flag = 1; wheel_L_Flag = 0;wheel_R_Flag = 0;wheel_Nav_Flag = 0;
		Back_Flag = 0; Track_Flag = 0;
		temp_MP = go_en;
		Car_Spend = go_sp;
		Control(Car_Spend ,Car_Spend);
		while(Go_Flag==1);
		delay_ms(500);
		Stop_Flag = 0; Go_Flag = 0; wheel_L_Flag = 0;wheel_R_Flag = 0;wheel_Nav_Flag = 0;
		Back_Flag = 0; Track_Flag = 1;
		Car_Spend = track_sp;
		while(Track_Flag==1)
		{
			if(Track_Special_Flag == 1)
			{
					Full_Automatic_GO(80,1100);
					Track_Special_Flag =0;
					Send_Debug_Info("A1B2C3\n",8);
			}
		}
		delay_ms(Full_Automatic_Delay);
}
/**
�������ܣ�����
��    ������
�� �� ֵ����
*/
void Full_Automatic_Back(uint16_t sp,uint16_t en)
{
		Roadway_mp_syn();	//����ͬ��
		Stop_Flag = 0; Go_Flag = 0; wheel_L_Flag = 0;wheel_R_Flag = 0;wheel_Nav_Flag = 0;
		Back_Flag = 1; Track_Flag = 0;
		temp_MP = en;
		Car_Spend = sp;
		Control(-(Car_Spend-10),-Car_Spend);					
		while(Back_Flag==1)
		delay_ms(Full_Automatic_Delay_back);
		delay_ms(Full_Automatic_Delay_back);
}
/**
�������ܣ���ת ֱ���亯�� 550 ����Ѱ������
��    ������
�� �� ֵ����
*/
void Full_Automatic_Left(uint16_t sp)
{
		Stop_Flag = 0; Go_Flag = 0; wheel_L_Flag = 1;wheel_R_Flag = 0;wheel_Nav_Flag = 0;
		Back_Flag = 0; Track_Flag = 0;
		Car_Spend = sp;				
		Control(-Car_Spend,Car_Spend);
		while(wheel_L_Flag == 1);
		delay_ms(Full_Automatic_Delay_turn);
		delay_ms(Full_Automatic_Delay_turn);
}
/**
�������ܣ���ת ֱ���亯�� 550 ����Ѱ������
��    ������
�� �� ֵ����
*/
void Full_Automatic_Right(uint16_t sp)
{
		Stop_Flag = 0; Go_Flag = 0; wheel_L_Flag = 0;wheel_R_Flag = 1;wheel_Nav_Flag = 0;
		Back_Flag = 0; Track_Flag = 0;
		Car_Spend = sp;
		Control(Car_Spend,-Car_Spend);
		while(wheel_R_Flag == 1);
		delay_ms(Full_Automatic_Delay_turn);
		delay_ms(Full_Automatic_Delay_turn);
}
/**********************
�������ܣ���ȡ��������
��    ������
�� �� ֵ����
**********************/
void Get_Car()
{
		Full_Automatic_Send_Android(0x04);	//��ȡ������Ϣ
	
		delay_ms(500);
	
		for(int i=0;i<12;i++)
			{
				WiFi_Data[i] = Wifi_Rx_Buf[i];
			}
		delay_ms(100);
			
		if(WiFi_Data[1] == 0x04)
		{
			CP_SJ[0] = WiFi_Data[2];
			CP_SJ[1] = WiFi_Data[3];
			CP_SJ[2] = WiFi_Data[4];
			CP_SJ[3] = WiFi_Data[5];
			CP_SJ[4] = WiFi_Data[6];
			CP_SJ[5] = WiFi_Data[7];
		}
		
		else
		{
			CP_SJ[0] = 0x43;
			CP_SJ[1] = 0x36;
			CP_SJ[2] = 0x37;
			CP_SJ[3] = 0x38;
			CP_SJ[4] = 0x47;
			CP_SJ[5] = 0x31;
		}
}
/**********************
�������ܣ����ͳ������ݸ���բ
��    ������
�� �� ֵ����
**********************/
void Send_Car()
{
	Get_Car();
	
	delay_ms(100);
		
	u8 Gate_data[8] = {0x55,0x03,0x00,0x00,0x00,0x00,0x00,0xBB};

	Gate_data[2] = 0x10;
	Gate_data[3] = CP_SJ[0];
	Gate_data[4] = CP_SJ[1];
	Gate_data[5] = CP_SJ[2];
	Gate_data[6] = (Gate_data[2]+Gate_data[3]+Gate_data[4]+Gate_data[5])%256;
	Send_ZigBee_Info(Gate_data,8);
	
	delay_ms(200);
	
	Gate_data[2] = 0x11;
	Gate_data[3] = CP_SJ[3];
	Gate_data[4] = CP_SJ[4];
	Gate_data[5] = CP_SJ[5];
	Gate_data[6] = (Gate_data[2]+Gate_data[3]+Gate_data[4]+Gate_data[5])%256;
	Send_ZigBee_Info(Gate_data,8);
	
}
/****************************************************************
�������ܣ�������ʾ����
��	  ����*data ��������ָ��
����  ֵ����
*****************************************************************/
void Send_Display()
{
	Get_Car();
	
	delay_ms(100);
	
	u8 Car_data[6] = {0xFF,0x12,0x01,0x00,0x00,0x00};

	Car_data[1] = 0x20;
	Car_data[2] = CP_SJ[0];
	Car_data[3] = CP_SJ[1];
	Car_data[4] = CP_SJ[2];
	Car_data[5] = CP_SJ[3];
	Infrared_Send(Car_data,6);
	delay_ms(500);
	Car_data[1] = 0x10;
	Car_data[2] = CP_SJ[4];
	Car_data[3] = CP_SJ[5];
////	Car_data[4] = data[6];
////	Car_data[5] = data[7];

	switch(RFID_Card_Position)
	{
		case 1:					//B2
			Car_data[4] = 0x42;
			Car_data[5] = 0x32;
			break;
		case 2:					//C2
			Car_data[4] = 0x43;
			Car_data[5] = 0x32;
			break;
		case 3:					//D2
			Car_data[4] = 0x44;
			Car_data[5] = 0x32;
			break;
		case 4:					//E2
			Car_data[4] = 0x45;
			Car_data[5] = 0x32;
			break;
		case 5:					//F2
			Car_data[4] = 0x46;
			Car_data[5] = 0x32;
			break;
	}
	//u8 Car_data1[6] = {0xFF,0x00,0x00,0x00,0x00,0x00};

	Infrared_Send(Car_data,6);
}
/*************************
�������ܣ�����׿�˷�����
					����λЭ�� 0x66
��    ������
�� �� ֵ����
*************************/
void Full_Automatic_Send_Android(u16 data)

{
		switch(data)
		{
				case 0x03:	//��ά��
						Send_To_Android[1] = 0x03;
						Send_Android_Info(Send_To_Android);
					break;
				
				case 0x04:	//����
						Send_To_Android[1] = 0x04;
						Send_Android_Info(Send_To_Android);
					break;
				
				case 0x05:	//��ͨ��
						Send_To_Android[1] = 0x05;
						Send_Android_Info(Send_To_Android);
					break;
				
				case 0x06:	//��״
						Send_To_Android[1] = 0x06;
						Send_Android_Info(Send_To_Android);
					break;
				
				case 0x07:	//TFT
						Send_To_Android[1] = 0x07;
						Send_Android_Info(Send_To_Android);
					break;
				
				case 0x08:	//�����ά������
						Send_To_Android[1] = 0x03;
						Send_To_Android[2] = 0x01;
						Send_Android_Info(Send_To_Android);
				break;
				
				default:
					break;
		}
}
/****************************************************************
�������ܣ���ͨ�ƴ���
��	  ����
����  ֵ��0x00ʶ��ʧ�� 0x01��ɫ 0x02��ɫ 0x03��ɫ
*****************************************************************/
void Full_RGB_Light()
{
				Send_ZigBee_Info(Light_Start,8);
				delay_ms(200);
				Full_Automatic_Send_Android(0x05);
				delay_ms(700);
				delay_ms(700);
				delay_ms(700);
				delay_ms(700);
				delay_ms(700);
				delay_ms(700);
				delay_ms(700);
				delay_ms(700);
				delay_ms(700);
				Full_WiFi_Data();	//��ȡWiFi����
					delay_ms(500);
			if (WiFi_Data[2] == 0x01)
			{
				Send_ZigBee_Info(Light_Red,8);
				Send_InfoData_To_Fifo("Red",3);
			}
			if (WiFi_Data[2] == 0x02)
			{
				Send_ZigBee_Info(Light_Green,8);
				Send_InfoData_To_Fifo("Green",5);
			}
			if (WiFi_Data[2] == 0x03)
			{
				Send_ZigBee_Info(Light_Yellow,8);
				Send_InfoData_To_Fifo("Yellow",6);
			}
			else
			{
				Send_ZigBee_Info(Light_Green,8);
				Send_InfoData_To_Fifo("Error",5);
			}
}
/**
�������ܣ�WiFi���ݽ���
��    ����WiFi_Data �յ������ݴ洢������
�� �� ֵ����
**/
void Full_WiFi_Data()
{
	for(int i=0;i<12;i++)
	{
	WiFi_Data[i] = Wifi_Rx_Buf[i];
	}
}
/*********
�������ܣ�WiFi�������
**********/
void Full_WiFi_Clean()
{
	WiFi_Data[0] = 0x00;
	WiFi_Data[1] = 0x00;
	WiFi_Data[2] = 0x00;
	WiFi_Data[3] = 0x00;
	WiFi_Data[4] = 0x00;
	WiFi_Data[5] = 0x00;
	WiFi_Data[6] = 0x00;
	WiFi_Data[7] = 0x00;
	WiFi_Data[8] = 0x00;
	WiFi_Data[9] = 0x00;
	WiFi_Data[10] = 0x00;
	WiFi_Data[11] = 0x00;
}
/*********
�������ܣ��������������
��    ������
�� �� ֵ����
**********/
void Full_Ultrasonic(u8 X)
{
  u8 Full_Ultrasonic_Data[8] = {0x55,0x04,0x00,0x00,0x00,0x00,0x00,0xBB};
  if(X == 1)					//��һ��
  {
	  Full_Ultrasonic_Data[2] = 0x01;  
  } else if( X == 2 ) {	 		//�ڶ��� 
	  Full_Ultrasonic_Data[2] = 0x02;  
  }
	
	csb_1 = dis/100;
	csb_2 = dis%100;
	csb_3 = csb_2/10*16+csb_2%10;
	
  Full_Ultrasonic_Data[3] = 0x00;
  Full_Ultrasonic_Data[4] = csb_1;
  Full_Ultrasonic_Data[5] = csb_3;
  Full_Ultrasonic_Data[6] = (u8)(Full_Ultrasonic_Data[2] + Full_Ultrasonic_Data[3] +Full_Ultrasonic_Data[4] + Full_Ultrasonic_Data[5])%256;
  Send_ZigBee_Info(Full_Ultrasonic_Data,8);
	Send_Debug_Info(Full_Ultrasonic_Data,16);
}
/*********
�������ܣ��ӳ����������
��    ������
�� �� ֵ��16���Ƶľ���ֵ
*********/
void Full_AGV_Ultrasonic()
{
	u16 temp;
	u8 Ultrasonic[2];
	u8 csb = 1;
	u8 time = 0;
	
	while(csb)
	{
		for(int i=0;i<12;i++)
		{
		WiFi_Data[i] = Zigb_Rx_Buf[i];
		}
		
		if(WiFi_Data[1] == 0x22)
		{
			Send_ZigBee_Info(AGV_Ultrasonic,8);
			delay_ms(700);
			delay_ms(700);
			delay_ms(700);
			delay_ms(700);
			Ultrasonic[0] = Zigb_Rx_Buf[5];
			Ultrasonic[1] = Zigb_Rx_Buf[4];
			Send_ZigBee_Info(AGV_Clean,8);
			temp = Ultrasonic[0] * 256 + Ultrasonic[1];
			temp = temp / 10;
			Ultrasonic[0] = temp / 256;
			Ultrasonic[1] = temp % 256;
			Send_ZigBee_Info(Ultrasonic,2);
			delay_ms(200);
		}
		
		else if(time == 60)
		{
			break;
		}
		
		else
		{
			time++;
			delay_ms(500);
			delay_ms(500);
		}
	}
}
/**
�������ܣ���ӳ�����բ
��    ����WiFi_Data �յ������ݴ洢������
�� �� ֵ����
**/
void AGV_DZ()
{
	u8 dz = 1;
	u8 time = 0;

	Full_WiFi_Clean();
	
	while(dz)
	{
		for(int i=0;i<12;i++)
		{
		WiFi_Data[i] = Zigb_Rx_Buf[i];
		}
		
		if(WiFi_Data[1] == 0x11)
		{
			Send_ZigBee_Info(DZ_K,8);
			dz = 0;
			break;
		}
		
		else if(time == 10)
		{
		Send_Car();		//��բ��ʾ����
		delay_ms(200);
		Send_ZigBee_Info(DZ_K,8); //��բ��
		delay_ms(200);
			break;
		}
		
		else
		{
			time++;
			delay_ms(500);
			delay_ms(500);
		}
	}
}
/********************************************
�������ܣ��ȴ��ӳ�����
*********************************************/
void AGV_Stop()
{
	u8 Stop = 1;
	u8 time = 0;
	
	Full_WiFi_Clean();
	
	while(Stop)
	{
		for(int i=0;i<12;i++)
		{
		WiFi_Data[i] = Zigb_Rx_Buf[i];
		}
		
		if(WiFi_Data[1] == 0x33)
		{
			Stop = 0;
			break;
		}
		
		else if(time == 30)
		{
			break;
		}
		
		else
		{
			time++;
			delay_ms(500);
			delay_ms(500);
		}
	}
}
/****************************************************************
�������ܣ���������
��	  ����End_Goal Ŀ�굵λ 1~4
����  ֵ����
*****************************************************************/
void Full_Light_Goal(u8 End_Goal)
{
	u16 Light_num[4];			//��ǿ����ֵ�������
	u16 Start_Light = 0;		//��ʼ��ǿ��ֵ
	u8  Light_Text[] = "0000";	//��ʾ��ǿ������
	int i=0, j=0;
	u16 temp = 0;
	
	if((End_Goal <= 8) && (End_Goal>4))		//��Ŀ�굵λ���ݴ���
	{
		End_Goal = End_Goal - 4;
	}
	if((End_Goal <= 16) && (End_Goal>8))	
	{
		End_Goal = End_Goal - 8;
	}
	
	Infrared_Send(H_1,4);
	delay_ms(700);
	delay_ms(700);
	delay_ms(700);
	delay_ms(700);
	Light_num[0] = Get_Bh_Value();	//�õ��ĸ���λ�Ĺ�ǿ��ֵ
	
	Infrared_Send(H_1,4);
	delay_ms(700);
	delay_ms(700);
	delay_ms(700);
	delay_ms(700);
	Light_num[1] = Get_Bh_Value();
	
	Infrared_Send(H_1,4);
	delay_ms(700);
	delay_ms(700);
	delay_ms(700);
	delay_ms(700);
	Light_num[2] = Get_Bh_Value();
	
	Infrared_Send(H_1,4);
	delay_ms(700);
	delay_ms(700);
	delay_ms(700);
	delay_ms(700);
	Light_num[3] = Start_Light = Get_Bh_Value();
	
	for(i=0;i<4;i++)
	{
		Light_Text[0] = Light_num[i]/1000+0x30;
		Light_Text[1] = Light_num[i]%1000/100+0x30;
		Light_Text[2] = Light_num[i]%100/10+0x30;
		Light_Text[3] = Light_num[i]%10+0x30;
		Send_Debug_Info(Light_Text,4);
		Send_Debug_Info("\n",2);
	}
	
	Send_Debug_Info("\n",2);
	
		for (j = 0; j < 3; j++)
		 {
		  for (i = 0; i < 3 - j; i++)
		  {
		   if (Light_num[i] > Light_num[i + 1])
		   {
			temp = Light_num[i]; 
			Light_num[i] = Light_num[i + 1]; 
			Light_num[i + 1] = temp;
		   }
		  }
		 } 
	
	for(i=0;i<4;i++)
	{
		Light_Text[0] = Light_num[i]/1000+0x30;
		Light_Text[1] = Light_num[i]%1000/100+0x30;
		Light_Text[2] = Light_num[i]%100/10+0x30;
		Light_Text[3] = Light_num[i]%10+0x30;
		Send_Debug_Info(Light_Text,4);
		Send_Debug_Info("\n",2);
	}
	
	for(i=0;i<4;i++)
	{
		if(Start_Light == Light_num[i])
			{
				if((End_Goal+3-i)%4!=0)
				{
					if(((End_Goal+3-i)%4) == 1)
					{
						Infrared_Send(H_1,4);
					}
					if(((End_Goal+3-i)%4) == 2)
					{
						Infrared_Send(H_2,4);
					}
					if(((End_Goal+3-i)%4) == 3)
					{
						Infrared_Send(H_3,4);
					}
				}
			}
	}
}
/****************************************************************
�������ܣ��жϹ⵵λ
��    ������
�� �� ֵ��Light_down   1/2/3/4  �ֱ�Ϊ���һ�����ڶ���������
*****************************************************************/
void Full_Light_Down()
{
	u16 Light_num[4];			//��ǿ����ֵ�������
	u16 Start_Light = 0;		//��ʼ��ǿ��ֵ
	u8  Light_Text[] = "0000";	//��ʾ��ǿ������
	int i=0, j=0;
	u16 temp = 0;
	u8 Light_down = 1;
	
	Infrared_Send(H_1,4);
	delay_ms(700);
	delay_ms(700);
	delay_ms(700);
	delay_ms(700);
	Light_num[0] = Get_Bh_Value();	//�õ��ĸ���λ�Ĺ�ǿ��ֵ
	
	Infrared_Send(H_1,4);
	delay_ms(700);
	delay_ms(700);
	delay_ms(700);
	delay_ms(700);
	Light_num[1] = Get_Bh_Value();
	
	Infrared_Send(H_1,4);
	delay_ms(700);
	delay_ms(700);
	delay_ms(700);
	delay_ms(700);
	Light_num[2] = Get_Bh_Value();
	
	Infrared_Send(H_1,4);
	delay_ms(700);
	delay_ms(700);
	delay_ms(700);
	delay_ms(700);
	Light_num[3] = Start_Light = Get_Bh_Value();
	
	for(i=0;i<4;i++)
	{
		Light_Text[0] = Light_num[i]/1000+0x30;
		Light_Text[1] = Light_num[i]%1000/100+0x30;
		Light_Text[2] = Light_num[i]%100/10+0x30;
		Light_Text[3] = Light_num[i]%10+0x30;
		Send_Debug_Info(Light_Text,4);
		Send_Debug_Info("\n",2);
	}
	
	Send_Debug_Info("\n",2);
	
		for (j = 0; j < 3; j++)
		 {
		  for (i = 0; i < 3 - j; i++)
		  {
		   if (Light_num[i] > Light_num[i + 1])
		   {
			temp = Light_num[i]; 
			Light_num[i] = Light_num[i + 1]; 
			Light_num[i + 1] = temp;
		   }
		  }
		 } 
	
	if(Start_Light == Light_num[0])
	{
		Light_down =1;
	}
	
	else if (Start_Light == Light_num[1])
	{
		Light_down =2;
	}
	
	else if (Start_Light == Light_num[2])
	{
		Light_down =3;
	}
	
	else if (Start_Light == Light_num[3])
	{
		Light_down =4;
	}
}
/****************************************************************
�������ܣ�TFT�ж�
��	  ������
����  ֵ��0x04���� 0x06��״
*****************************************************************/
void TFT_Data()
{
	u8 TFT_Flag = 0;
	while(1)
	{
		if(TFT_Flag == 12)
		{
			break;
		}
		
		for(int i=0;i<12;i++)
		{
			WiFi_Data[i] = Wifi_Rx_Buf[i];
		}
		
		switch(WiFi_Data[1])
		{
			case 0x04:
				Car_Data[0] = WiFi_Data[2];
				Car_Data[1] = WiFi_Data[3];
				Car_Data[2] = WiFi_Data[4];
				Car_Data[3] = WiFi_Data[5];
				Car_Data[4] = WiFi_Data[6];
				Car_Data[5] = WiFi_Data[7];
			
				Wifi_Rx_Buf[0] = 0x00;
				Wifi_Rx_Buf[1] = 0x00;
				TFT_Flag = 12;
			break;
			
			case 0x06:
				XZ_Data[0] = WiFi_Data[2];
				XZ_Data[1] = WiFi_Data[3];
				XZ_Data[2] = WiFi_Data[4];
				XZ_Data[3] = WiFi_Data[5];
				XZ_Data[4] = WiFi_Data[6];
				XZ_Data[5] = WiFi_Data[7];
				XZ_Data[6] = WiFi_Data[8];
				XZ_Data[7] = WiFi_Data[9];
				XZ_Data[8] = WiFi_Data[10];

				Wifi_Rx_Buf[0] = 0x00;
				Wifi_Rx_Buf[1] = 0x00;
				TFT_Flag = 12;
			break;
			
			default:
				delay_ms(500);
				delay_ms(500);
				TFT_Flag++;
			break;
		}
	}
}
/****************************************************************
�������ܣ����峵��
��	  ����1ѯ�ʵڼ��� 2�½�����һ�� 3���������½�
����  ֵ��0x01��һ�� 0x02�ڶ��� 0x03������ 0x04���Ĳ�
*****************************************************************/
void Car_3D(u8 Run)
{
		if(Run == 1)
		{
				Send_ZigBee_Info(Car_3D_Info,8);
				delay_ms(700);
				Car_Where[0] = Zigb_Rx_Buf[4];
		}
		if(Run == 2)
		{
				Send_ZigBee_Info(Car_3D_Down_1,8);
		}
		if(Run == 3)
		{
					switch(Car_Where[0])
					{
							case 0x01:
								Send_ZigBee_Info(Car_3D_Down_1,8);
								break;
							
							case 0x02:
								Send_ZigBee_Info(Car_3D_Down_2,8);
								break;
							
							case 0x03:
								Send_ZigBee_Info(Car_3D_Down_3,8);
								break;
							
							case 0x04:
								Send_ZigBee_Info(Car_3D_Down_4,8);
								break;
							
							default:
								Send_ZigBee_Info(Car_3D_Down_3,8);
								break;
					}
			}
}
/********************************************
�������ܣ�TFT��ʾָ��ͼƬ
*********************************************/
void TFT(u8 data)
{
		Send_TFT[2] = 0x10;		//��ΪͼƬ��ʾģʽ
		Send_TFT[3] = 0x00;
		Send_TFT[4] = data;
		Send_TFT[5] = 0x00;
		Send_TFT[6] = (Send_TFT[2]+Send_TFT[3]+Send_TFT[4]+Send_TFT[5])%256;
		Send_ZigBee_Info(Send_TFT,8);
}
/********************************************
�������ܣ�TFT��ʾ����
*********************************************/
void TFT_Car()
{
	u8 i=2;
	while(i)
	{
		Send_TFT[2] = 0x20;		//��ΪͼƬ��ʾģʽ
		Send_TFT[3] = CP_SJ[0];
		Send_TFT[4] = CP_SJ[1];
		Send_TFT[5] = CP_SJ[2];
		Send_TFT[6] = (Send_TFT[2]+Send_TFT[3]+Send_TFT[4]+Send_TFT[5])%256;
		Send_ZigBee_Info(Send_TFT,8);
			delay_ms(500);
		Send_TFT[2] = 0x21;		//��ΪͼƬ��ʾģʽ
		Send_TFT[3] = CP_SJ[3];
		Send_TFT[4] = CP_SJ[4];
		Send_TFT[5] = CP_SJ[5];
		Send_TFT[6] = (Send_TFT[2]+Send_TFT[3]+Send_TFT[4]+Send_TFT[5])%256;
		Send_ZigBee_Info(Send_TFT,8);
			delay_ms(500);
			i--;
	}
}
/********************************************
�������ܣ�TFT��ʾ��Ϣ
���磺TFT_cm(0x09,0x11); ��ʾ��Ϣ 911
*********************************************/
void TFT_cm(u8 data1,u8 data2)
{
		Send_TFT[2] = 0x50;
		Send_TFT[3] = 0x00;
		Send_TFT[4] = data1;
		Send_TFT[5] = data2;
		Send_TFT[6] = (Send_TFT[2]+Send_TFT[3]+Send_TFT[4]+Send_TFT[5])%256;
		Send_ZigBee_Info(Send_TFT,8);
}
/********************************************
�������ܣ�TFT��ʾ��Ϣ
���磺TFT_Display(0x11,0x11,0x11); ��ʾ��Ϣ111111
*********************************************/
void TFT_Display(u8 data1,u8 data2,u8 data3)
{
		Send_TFT[2] = 0x40;
		Send_TFT[3] = data1;
		Send_TFT[4] = data2;
		Send_TFT[5] = data3;
		Send_TFT[6] = (Send_TFT[2]+Send_TFT[3]+Send_TFT[4]+Send_TFT[5])%256;
		Send_ZigBee_Info(Send_TFT,8);
}
/********************************************
�������ܣ�TFT��ʼ��ʱ
���磺TFT_Time(0); ֹͣ��ʱ
*********************************************/
void TFT_Time(u8 data)
{
	if(data == 0)		//ֹͣ��ʱ
	{
		Send_TFT[2] = 0x30;
		Send_TFT[3] = 0x00;
		Send_TFT[4] = 0x00;
		Send_TFT[5] = 0x00;
		Send_TFT[6] = (Send_TFT[2]+Send_TFT[3]+Send_TFT[4]+Send_TFT[5])%256;
		Send_ZigBee_Info(Send_TFT,8);
	}
	
	if(data == 1)		//��ʼ��ʱ
	{
		Send_TFT[2] = 0x30;
		Send_TFT[3] = 0x01;
		Send_TFT[4] = 0x00;
		Send_TFT[5] = 0x00;
		Send_TFT[6] = (Send_TFT[2]+Send_TFT[3]+Send_TFT[4]+Send_TFT[5])%256;
		Send_ZigBee_Info(Send_TFT,8);
	}
	
	if(data == 2)		//��ռ�ʱ
	{
		Send_TFT[2] = 0x30;
		Send_TFT[3] = 0x02;
		Send_TFT[4] = 0x00;
		Send_TFT[5] = 0x00;
		Send_TFT[6] = (Send_TFT[2]+Send_TFT[3]+Send_TFT[4]+Send_TFT[5])%256;
		Send_ZigBee_Info(Send_TFT,8);
	}
}
/***********************
�������ܣ����̨�����㷨
************************/
static const u8 aucCRCHi[] = {

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,

    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,

    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40

};

static const u8 aucCRCLo[] = {

    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,

0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,

    0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,

    0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,

    0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,

    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,

    0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,

    0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,

    0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,

    0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,

    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,

    0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,

    0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB,

    0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,

    0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,

    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,

    0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,

    0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,

    0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,

    0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,

    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,

    0x41, 0x81, 0x80, 0x40

};

void usMBCRC16( u8 * pucFrame, u8 usLen,u8 * result )

{

    u8 ucCRCHi = 0xFF;

    u8 ucCRCLo = 0xFF;

    int iIndex;

    while( usLen-- )

    {

        iIndex = ucCRCLo ^ *( pucFrame++ );

        ucCRCLo = ( u8 )( ucCRCHi ^ aucCRCHi[iIndex] );

        ucCRCHi = aucCRCLo[iIndex];

    }
		result[0] = ucCRCHi;
		result[1] = ucCRCLo;
			//return (u8){ucCRCHi,ucCRCLo};
    //return ( u8 )( ucCRCHi << 8 | ucCRCLo );

}
/*****************
�������ܣ����̨����
******************/
void Full_FHT()
{
	u8 QR = 1;
	u8 fht_data[4] = {0x41,0x61,0x42,0x62};
	u8 resu[4];
	u8 time = 0;
	
	Full_Automatic_Send_Android(0x08);
	delay_ms(100);
	
	while(QR)
	{
		for(int i=0;i<12;i++)
		{
		WiFi_Data[i] = Wifi_Rx_Buf[i];
		}
		
		if(WiFi_Data[1] == 0x03 && WiFi_Data[2] == 0x01)
		{
			fht_data[0] = WiFi_Data[3];
			fht_data[1] = WiFi_Data[4];
			fht_data[2] = WiFi_Data[5];
			fht_data[3] = WiFi_Data[6];
			QR = 0;
		}
		
		else if(time == 3)
		{
				fht_data[0] = 0x41;
				fht_data[1] = 0x61;
				fht_data[2] = 0x42;
				fht_data[3] = 0x62;
			
				QR = 0;
		}
		
		else
		{
			time = time +1;
			delay_ms(500);
			delay_ms(500);
			QR = 1;
		}
	}
	usMBCRC16(fht_data,4,resu);
		
	u8 resss[6] = {resu[0],fht_data[0],fht_data[1],fht_data[2],fht_data[3],resu[1]};
	
		Infrared_Send(resss,6); //����������
		delay_ms(Full_Send_Delay);
		Infrared_Send(resss,6); //����������
}
/**
�������ܣ�ȫ�Զ�
��    ������
�� �� ֵ����
*/
void	Full_Automatic(void)
{
	if(Full_Automatic_Start ==0)
	{
		return;
	}
		
	Full_Automatic_Flag+=10;
switch(Full_Automatic_Flag)
{
	case 10:
		MP_SPK = 1;		//ϵͳ������ʾ����ʼ
		delay_ms(500);
		delay_ms(500);
		MP_SPK = 0;		//ϵͳ������ʾ������
		Full_Automatic_Track(Track_Sp);	//ǰ����ѭ����
		delay_ms(500);
		delay_ms(500);
		delay_ms(500);
		delay_ms(500);
		Full_Automatic_GO_With_Track(Track_Sp,Go_Mp); //Ѱ����·��
		delay_ms(500);
		delay_ms(500);
		delay_ms(500);
		delay_ms(500);
		Full_Automatic_Track(Track_Sp);	//ǰ����ѭ����
		delay_ms(500);
		delay_ms(500);
		delay_ms(500);
		delay_ms(500);
		Full_Automatic_GO_With_Track(Track_Sp,Go_Mp); //Ѱ����·��
		delay_ms(500);
		delay_ms(500);
		delay_ms(500);
		delay_ms(500);
		Full_Automatic_Track(Track_Sp);	//ǰ����ѭ����
		delay_ms(500);
		delay_ms(500);
		delay_ms(500);
		delay_ms(500);
		Full_Automatic_GO_With_Track(Track_Sp,Go_Mp); //Ѱ����·��
		delay_ms(500);
		delay_ms(500);
		delay_ms(500);
		delay_ms(500);
		Full_Automatic_Left(Whell_Sp); //��ת
	break;
	
	case 20:
		Full_Automatic_Track(Track_Sp);	//ǰ����ѭ����
		delay_ms(500);
		Full_Automatic_GO_With_Track(Track_Sp,Go_Mp); //Ѱ����·��
		delay_ms(200);
		Full_Automatic_Right(Whell_Sp); //��ת
		delay_ms(500);
		Full_Right45(); //��ת45��
		delay_ms(500);
		delay_ms(500);
		delay_ms(500);
		delay_ms(500);
	break;
	
	case 30:
		Full_Automatic_Left(Whell_Sp); //��ת
		delay_ms(500);
		Full_Automatic_Left(Whell_Sp); //��ת
		delay_ms(500);
		Full_Automatic_Track(Track_Sp);	//ǰ����ѭ����
		delay_ms(500);
		Full_Automatic_GO_With_Track(Track_Sp,Go_Mp); //Ѱ����·��
		delay_ms(200);
		Full_Automatic_Right(Whell_Sp); //��ת
		delay_ms(500);
		delay_ms(500);
		delay_ms(500);
		delay_ms(500);
		Full_Automatic_Left(Whell_Sp); //��ת
		delay_ms(500);
		Full_Automatic_Left(Whell_Sp); //��ת
		delay_ms(500);
		Full_Automatic_Back(Back_Sp,500);	//����
		delay_ms(500);
		Full_Automatic_Track(Track_Sp);	//ǰ����ѭ����
		delay_ms(500);
		delay_ms(500);
		delay_ms(500);
		delay_ms(500);
		delay_ms(200);
		Full_Automatic_GO_With_Track(Track_Sp,Go_Mp); //Ѱ����·��
	break;

	case 40:
		Full_Automatic_Track(Track_Sp);	//ǰ����ѭ����
		delay_ms(500);
		Full_Automatic_GO(60,2200);
		Full_Automatic_Back(Back_Sp,500);	//����
		delay_ms(500);
		Full_Automatic_RFID_Track(80);	//��ʼѰ��ѭ��
	Send_Debug_Info("xk1ks\n",8);
		delay_ms(100);
		Go_Find_cards1();	//��һ��Ѱ��
	Send_Debug_Info("xk1js\n",8);
		delay_ms(100);
		Full_Automatic_GO_With_Track(80,700);	//��ʼѰ��ѭ��
	Send_Debug_Info("xk2ks\n",8);
		delay_ms(100);
		Go_Find_cards2();	//�ڶ���Ѱ��
	Send_Debug_Info("xk2js\n",8);
		delay_ms(100);
		Full_Automatic_RFID_Track(80);	//��ʼѰ��ѭ��
	Send_Debug_Info("xk3ks\n",8);
		delay_ms(100);
		Go_Find_cards3();	//������Ѱ��
	Send_Debug_Info("xk3js\n",8);
		delay_ms(100);
	break;
	
	case 50:
			if(RFID_Card_Position == 3)
			{
				Full_Automatic_Left(Whell_Sp);
			}
			else
			{
				Full_Automatic_GO(60,430);
				Full_Automatic_Left(Whell_Sp);
			}
	break;
	
	case 60:
		delay_ms(100);
		Full_Automatic_GO_With_Track(80,550);	//��ʼѰ��ѭ��
	Send_Debug_Info("xk4ks\n",8);
		delay_ms(100);
		Go_Find_cards4();	//���Ĵ�Ѱ��
	Send_Debug_Info("xk4js\n",8);
		delay_ms(100);
		Full_Automatic_RFID_Track(80);	//��ʼѰ��ѭ��
	Send_Debug_Info("xk5ks\n",8);
		delay_ms(100);
		Go_Find_cards5();	//�����Ѱ��
	Send_Debug_Info("xk5js\n",8);
		delay_ms(100);
	break;
	
	case 70:
			if(RFID_Card_Position == 4)
			{
				Full_Automatic_GO(60,430);
				Full_Automatic_Left(Whell_Sp); //��ת
				delay_ms(500);
			}
			else
			{
				Full_Automatic_Left(Whell_Sp); //��ת
				delay_ms(500);
			}
	break;
	
	case 80:
	break;
	
	case 90:
		Full_Automatic_Special();	//������δ���
		delay_ms(500);
		Full_Automatic_Left(Whell_Sp); //��ת
		delay_ms(500);
		Full_Left45(); //��ת45��
		delay_ms(500);
	break;

	case 130:
		MP_SPK = 1;		//ϵͳ������ʾ����ʼ
		delay_ms(500);
		delay_ms(500);
		MP_SPK = 0;		//ϵͳ������ʾ������
		Set_tba_Beep(0x01);
		Set_tba_WheelLED(0x01,0x01);
		Set_tba_WheelLED(0x02,0x01);
	break;
	
	default:
		break;

}
}