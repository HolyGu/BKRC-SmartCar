#include <stdint.h>
#include <string.h>
#include "stm32f4xx.h" 
#include "my_lib.h"
//#include "usart_drv.h"
#include "delay.h"
#include "can_drv.h"
#include "fifo_drv.h"
#include "can_user.h"

#define __CANP_HostCOM_C__
#include "canp_hostcom.h"


//uint8_t canrx_tbuf[8];
//uint8_t cantx_tbuf[8];
uint32_t CanP_RxFMI_Flag;
uint8_t ctbuf[8];
uint8_t Track_buf[2]={0,0};
int16_t CanHost_Mp;
uint16_t CanHost_Navig;

#define CAN_CURR_MODE	4
/*
1	���
2	Ѱ��
3	����
4	Ѱ��+����
5	����
6	WIFI
6	ZIGBEE
8	��ʾ
9	WIFI+ZIGBEE+��ʾ
*/


Can_Filter_Struct Fileter_Tab[]=
{
	/*
	{CAN_ID_DISP	,CAN_ID_DISP	,0,0},//CAN_ID_HOST,CAN_ID_HOST},
	{CAN_ID_WIFI	,CAN_ID_WIFI	,0,0},//CAN_ID_HOST,CAN_ID_HOST},
	{CAN_ID_ZIGBEE	,CAN_ID_ZIGBEE	,0,0},
	{CAN_ID_MOTOR	,CAN_ID_MOTOR	,0,0},
	{CAN_ID_TRACK	,CAN_ID_TRACK	,0,0},
	{CAN_ID_NAVIG	,CAN_ID_NAVIG	,0,0},
	{CAN_ID_HOST	,CAN_ID_HOST	,0,0},
	
	{0x12,0x12,0,0},
	{0,0,0,0},
	*/
	{CAN_SID_HL(ID_DISP,	ID_ZERO),CAN_SID_HL(ID_ALL,ID_ZERO)},
	{CAN_SID_HL(ID_WIFI,	ID_ZERO),CAN_SID_HL(ID_ALL,ID_ZERO)},
	{CAN_SID_HL(ID_ZIGBEE,	ID_ZERO),CAN_SID_HL(ID_ALL,ID_ZERO)},
	//	{CAN_SID_HL(ID_MOTOR,	ID_ZERO),CAN_SID_HL(ID_ALL,ID_ZERO)},
	{CAN_SID_HL(ID_TRACK,	ID_ZERO),CAN_SID_HL(ID_ALL,ID_ZERO)},
	{CAN_SID_HL(ID_NAVIG,	ID_ZERO),CAN_SID_HL(ID_ALL,ID_ZERO)},
	{CAN_SID_HL(ID_HOST,	ID_ZERO),CAN_SID_HL(ID_ALL,ID_ZERO)},
	
	//	{0x12,0x12},//0,0},
	//	
	{0,0,},
	
};

void CanP_Init(void)
{
	CanP_RxFMI_Flag = 0;
	//CanDrv_Hinit();
	Can_Filter_Flag = CAN_FLAG_MASK_16;	//CAN_FLAG_LIST_16 ;//| CAN_FLAG_IDE_EN;
	//Can_Filter_Flag = CAN_FLAG_LIST_16;	//CAN_FLAG_LIST_16 ;//| CAN_FLAG_IDE_EN;
	CanDrv_FiterUpdata(Fileter_Tab,sizeof(Fileter_Tab)/sizeof(Fileter_Tab[0]));
	//CanDrv_Sinit();
	
	CanP_FifoInit();
}

void CanP_test_tbuf_init(void)
{
	/*	
	static uint8_t c = 0;
	uint8_t i;
	for(i = 0;i<8;i++)
	cantx_tbuf[i] = c++;
	*/
}

void CanP_Check(void)
{
	static uint32_t canp_ct = 0;
	uint8_t rl;
	if(gt_get_sub(canp_ct) == 0)
	{
		canp_ct = gt_get() + 100;
		//CAN_Transmit(CAN1, &TxMessage);
		CanP_test_tbuf_init();
		//Can_Send_Msg(cantx_tbuf,8,0x12,0x12);
		//CanDrv_TxData(cantx_tbuf,8,0x40,0x50,_NULL);
		//CanP_CanTx_Check();
		
		CanP_TestFifo();
	}
	//if(Ccp_RxFlag)
	rl =CanDrv_RxCheck();
	if(rl)
	{
		CanRxMsg rme;
		//rl = CanDrv_RxData(canrx_tbuf);
		CanDrv_RxGetMeesage(&rme);
		//Ccp_RxFlag = 0;
		//UD_AddStrint((char *)RxMessage.Data,1,RxMessage.DLC);
		//UD_AddStrint((char *)rme.Data,1,rme.DLC);
	}
	
}

//______________________________________________________________________________
Fifo_Drv_Struct Fifo_Info;
Fifo_Drv_Struct Fifo_WifiRx;
Fifo_Drv_Struct Fifo_WifiTx;
Fifo_Drv_Struct Fifo_ZigbRx;
Fifo_Drv_Struct Fifo_ZigbTx;

#define FIFOSIZE_INFO	50
#define FIFOSIZE_WIFIRX	50
#define FIFOSIZE_WIFITX	50
#define FIFOSIZE_ZIGBRX	50
#define FIFOSIZE_ZIGBTX	50

uint8_t FifoBuf_Info[FIFOSIZE_INFO];
uint8_t FifoBuf_WifiRx[FIFOSIZE_WIFIRX];
uint8_t FifoBuf_WifiTx[FIFOSIZE_WIFITX];
uint8_t FifoBuf_ZigbRx[FIFOSIZE_ZIGBRX];
uint8_t FifoBuf_ZigbTx[FIFOSIZE_ZIGBTX];


void CanP_FifoInitS(Fifo_Drv_Struct *p,uint32_t l,uint8_t *buf)
{
	p->ml = l;
	p->buf = buf;
	FifoDrv_Init(p,0);
}

void CanP_FifoInit(void)
{
	CanP_FifoInitS(&Fifo_Info,FIFOSIZE_INFO,FifoBuf_Info);
	CanP_FifoInitS(&Fifo_WifiRx,FIFOSIZE_WIFIRX,FifoBuf_WifiRx);
	CanP_FifoInitS(&Fifo_WifiTx,FIFOSIZE_WIFITX,FifoBuf_WifiTx);
	CanP_FifoInitS(&Fifo_ZigbRx,FIFOSIZE_ZIGBRX,FifoBuf_ZigbRx);
	CanP_FifoInitS(&Fifo_ZigbTx,FIFOSIZE_ZIGBTX,FifoBuf_ZigbTx);
}


CanRxMsg crm;
uint8_t crbuf[4][8];

void CanP_CanRx_TrackUp(void)
{
	//Hard_Track_CB();
	//Moveuser_TrackCB();
	Host_Receive_UpTrack(crbuf[0][0], crbuf[0][1]);
	//AvrComm_upTrack(crbuf[0][0]);
}


/**
�������ܣ���ȡ������������Ϣ
		  �ж����������� ��ָ���Զ���Fifo��д������
		  ͳ������������
��    ��: ��
�� �� ֵ: ��
*/
void CanP_CanRx_Irq(void)
{
	CanDrv_RxGetMeesage(&crm);							 //��ȡ������������Ϣ
	switch(crm.FMI)										 //�ж���Ϣ��������
	{
	case 0:			//disp
		FifoDrv_BufWrite(&Fifo_Info,crm.Data,crm.DLC);	 //��Fifo_Info��д����Ϣ
		break;
	case 1:			//wifi rx
		FifoDrv_BufWrite(&Fifo_WifiRx,crm.Data,crm.DLC); //��Fifo_WifiRx��д����Ϣ
		break;
	case 2:			//zigbee rx
		FifoDrv_BufWrite(&Fifo_ZigbRx,crm.Data,crm.DLC); //��Fifo_ZigbRx��д����Ϣ
		break;
	default:
		if((crm.FMI >= 3)&&(crm.FMI <= 6))				 //�ж���������
			memcpy(crbuf[crm.FMI-3],crm.Data,8);		 //��crbuf��ά������д������
		break;
	}
	if(crm.FMI <= 6)								
		CanP_RxFMI_Flag |= bit_tab[crm.FMI];			//ͳ������������ 
}


/**
�������ܣ�CAN���ռ��
��    ��: ��
�� �� ֵ: ��
*/
void CanP_CanRx_Check(void)
{
	while(CanDrv_RxCheck())								//�����չ������䲻Ϊ��								
	{
		CanP_CanRx_Irq();								//�ӹ��������л�ȡ��Ϣ������ָ���Զ���Fifo��д����Ϣ
	}
}


/**
�������ܣ��ж�ָ���Զ���Fifo�л�ȡ���ݲ�����
*/
void CanP_Host_Main(void)
{
	uint8_t t8;
	uint8_t i;
	CanP_CanRx_Check();
	
	static uint32_t i_prt;
	
	while(CanP_RxFMI_Flag)
	{
		CanP_CanRx_Check();
		for(i = 0;i<=6;i++)
		{
			if(CanP_RxFMI_Flag & bit_tab[i])
			{
				switch(i)
				{
				case 0:
					while(FifoDrv_ReadOne(&Fifo_Info,&t8));
						break;
				case 1:					
					while(FifoDrv_ReadOne(&Fifo_WifiRx,&t8))
					{
						Can_WifiRx_Save(t8);
					}
					break;
				case 2:
					while(FifoDrv_ReadOne(&Fifo_ZigbRx,&t8))
					{
						Can_ZigBeeRx_Save(t8);
					}							
					break;
				case 3:
					CanP_CanRx_TrackUp();
					//up_Test_Upmp();
					break;
				case 4:
					CanHost_Navig = U8ToU16_Big(crbuf[1]);
					while(CanHost_Navig >= 3600)
						CanHost_Navig -= 3600;
					break;
				case 5:	
					if(crbuf[2][0] == 0x02)
					{
						CanHost_Mp = U8ToU16(&crbuf[2][0]+1);		
						//up_Test_Upmp();
					}
					break;
				case 6:
					if(crbuf[3][0] == 0x01)
						
					break;
				}
				CanP_RxFMI_Flag &= bit_and_tab[i];				//����������� 
			}			
		}
	}
}


void CanP_WaitTx(uint8_t *tmbox,uint32_t m)
{
	while(m--)
	{
		if(CanDrv_TxStatus(*tmbox) == CAN_TxStatus_Ok)
			break;
	}
	
}

void CanP_CanTx_Check(void)
{
	uint8_t tmbox,i,f = 1;
	while(f)
	{
		f = 0;
		i = FifoDrv_BufRead(&Fifo_Info,ctbuf,8);// ������Ϣ
		
		if(i)
		{
			CanDrv_WhaitTxEmpty();
			f = 1;
		}
		i = FifoDrv_BufRead(&Fifo_WifiTx,ctbuf,8); // wifi��Ϣ
		if(i)
		{	
			CanDrv_WhaitTxEmpty();
			CanDrv_TxData(ctbuf,i,CAN_SID_HL(ID_WIFI,0),0,&tmbox);
			f = 1;
		}
		
		i = FifoDrv_BufRead(&Fifo_ZigbTx,ctbuf,8);// Zigbee��Ϣ
		if(i)
		{	
			CanDrv_WhaitTxEmpty();
			CanDrv_TxData(ctbuf,i,CAN_SID_HL(ID_ZIGBEE,0),0,&tmbox);		
			f = 1;
		}	
	}
	
}

/**
�������ܣ��ϴ�������Ϣ
��    ����x1 ������  x2�Ҳ����
�� �� ֵ����
*/
void Send_Electric(u8 x1, u8 x2)  
{
	u8 txbuf[3];
//	txbuf[0] =0x01;
//	txbuf[1] = x1;
//	txbuf[2] = x2;

	txbuf[0] =0x03;
	txbuf[1] = 1;
	txbuf[2] = x2;
	
	CanDrv_WhaitTxEmpty();									//�жϲ��ȴ���������Ϊ��
	CanDrv_TxData(txbuf,3,CAN_SID_HL(ID_HOST,0),0,_NULL);	//��CAN���߷�������
	
}

/**
δʹ��
*/
void Send_CodedCnt(void)
{
	u8 txbuf[5];
	txbuf[0] =0x02;
	//U16ToU8(txbuf+1,CodedDisc_Get(0));
	//U16ToU8(txbuf+3,CodedDisc_Get(1));
	
	CanDrv_WhaitTxEmpty();
	CanDrv_TxData(txbuf,5,CAN_SID_HL(ID_HOST,0),0,_NULL);
	
}

/**
�������ܣ��ϴ�������Ϣ
��    ����*s ������Ϣָ�� len ��Ϣ����
�� �� ֵ����
*/
void Send_Debug_Info(u8 *s,u8 len)
{	  
	CanDrv_WhaitTxEmpty();
	CanDrv_TxData( s,len,CAN_SID_HL(ID_DISP,0),0,_NULL);	
}

void Send_ZigBee_Info(u8 *s,u8 len)
{
	CanDrv_WhaitTxEmpty();
	CanDrv_TxData( s,len,CAN_SID_HL(ID_ZIGBEE,0),0,_NULL);
}

/**
�������ܣ����͵��ת��
��    ����x1 ������ٶ�  x2 �Ҳ���ת��
�� �� ֵ����
*/
void Send_UpMotor( int x1, int x2)
{
	u8 txbuf[4];
	txbuf[0] = x1;
	txbuf[1] = x1;
	txbuf[2] = x2;
	txbuf[3] = x2;	
	
	CanDrv_WhaitTxEmpty();
	CanDrv_TxData(txbuf,4,CAN_SID_HL(ID_MOTOR,0),0,_NULL);	
	
}

 /** ��δʹ��
�������ܣ����͵�����������
��    ����c ָ���Ƕ�
�� �� ֵ����
*/
void Send_UpCompass(uint16_t c)
{
	uint8_t tbuf[8];
	tbuf[0] = (c>>8)&0x0ff;
	tbuf[1] = (c)&0x0ff;
	CanDrv_TxData(tbuf,2,CAN_SID_HL(ID_NAVIG,ID_NAVIG),0,_NULL);
	
}


/** ��δʹ��
�������ܣ�CAN�������ݴ������
��    ������
�� �� ֵ: ��
*/
void CanP_TestFifo(void)
{
	static uint8_t c = '0';
	uint32_t i;
	uint8_t t;
	
	for(i = 0;i<10;i++)
	{
		if(FifoDrv_WriteOne(&Fifo_Info,c) == 0)
			break;
		if(++c > '9')
			c  = '0';
		//DispCar_InfoDebug(&c,1,1,0);
	}
	
	for(i = 0;i<7;i++)
	{
		if(FifoDrv_ReadOne(&Fifo_Info,&t) == 0)
			break;
		//UD_AddStrint((char *)&t,0,1);
		//		DispCar_InfoDebug(&t,1,1,0);
	}
}  


/**
��������: ѭ�����ݽ���
��    ��: x1 �Ͱ�λ  x2 ����λ
�� �� ֵ: ��
*/
void Host_Receive_UpTrack( u8 x1, u8 x2)  // ѭ�����ݽ��մ�����
{
	Track_buf[0] = x1;
	Track_buf[1] = x2;	 
}


/**
�������ܣ���ȡ������ѭ������
��    ����mode : TRACK_ALL ��ȡ�������� TRACK_Q7 ��ȡ��λѭ������ TRACK_H8 ��ȡ��λѭ������
�� �� ֵ��Rt ѭ������
*/
u16  Get_Host_UpTrack( u8 mode)  // ��ȡѭ������
{
	uint16_t Rt = 0;
	switch(mode)
	{
	case TRACK_ALL: 
		Rt = (u16)((Track_buf[0] <<8)+ Track_buf[1] );
		break;
	case TRACK_Q7:
		Rt = Track_buf[1] ;
		break;
	case TRACK_H8:
		Rt = Track_buf[0];
		break;
	}
	return Rt;
}

/**
�������ܣ�����ѭ���巢�书��
��    ����power ������ѭ���幦��
�� �� ֵ����
*/
void Set_Track_Pwr( u16 power) // ����Ѱ���巢�书��
{
	u8 txbuf[3];
	
	txbuf[0] = 0X03;  //����ؼ���
	txbuf[1] = (power>> 8)&0xff;
	txbuf[2] = (power)&0xff;
	CanDrv_WhaitTxEmpty();
	CanDrv_TxData(txbuf,3,CAN_SID_HL(ID_TRACK,ID_HOST),0,_NULL);
}


/** ��δʹ��
�������ܣ���������ѭ�������
��    ������
�� �� ֵ����
*/
void Set_Track_Yzbj(u8 addr, u16 ydata)
{
	u8 txbuf[4];
	
	txbuf[0] = 0X04;  //����ؼ���
	txbuf[1] = addr;
	txbuf[2] = (ydata>> 8)&0xff;
	txbuf[3] = (ydata)&0xff;
	CanDrv_WhaitTxEmpty();
	CanDrv_TxData(txbuf,4,CAN_SID_HL(ID_TRACK,ID_HOST),0,_NULL);
	
}

/** ��δʹ��
�������ܣ�����ѭ����ʼ��
��	  ������
�� �� ֵ����
*/
void Set_Track_Init( void)
{
	u8 i;          
#define T_POWER 820   // ���ʳ�ʼ��
#define T_BJYZ  7000  // �Ƚ���ֵ��ʼ��
	
	Set_Track_Pwr( T_POWER);
	delay_ms(100);
	
	for(i=0;i<15;i++)
	{
		Set_Track_Yzbj(i,T_BJYZ);
		delay_ms(30);
	}
}

/** 
�������ܣ�����ѭ�����ϴ�����ʱ��
��    ��: ��
�� �� ֵ: ��
*/
void Host_Set_UpTrack( u8 time)  // ����ѭ�������ϴ�ʱ����
{
#if 1
	u8 txbuf[2];
	txbuf[0] = 0X02;  //����ؼ���
	txbuf[1] = time;
	CanDrv_WhaitTxEmpty();
	CanDrv_TxData(txbuf,2,CAN_SID_HL(ID_TRACK,ID_HOST),0,_NULL);   
#endif
	
}

/**
�������ܣ������ݷ�����ZigBeeģ��
��    ������
�� �� ֵ����
*/
void Send_ZigbeeData_To_Fifo( u8 *p ,u8 len)   
{
	FifoDrv_BufWrite( &Fifo_ZigbTx , p , len);
}

/**
�������ܣ������ݷ�����WIFIģ��
��    ������
�� �� ֵ����
*/
void Send_WifiData_To_Fifo( u8 *p ,u8 len)   
{
	FifoDrv_BufWrite( &Fifo_WifiTx , p , len);
}


void Send_InfoData_To_Fifo( u8 *p ,u8 len)   
{
	FifoDrv_BufWrite( &Fifo_Info , p , len);
}