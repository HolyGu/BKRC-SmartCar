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
1	电机
2	寻迹
3	导航
4	寻迹+导航
5	主机
6	WIFI
6	ZIGBEE
8	显示
9	WIFI+ZIGBEE+显示
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
函数功能：获取挂起邮箱中消息
		  判断邮箱索引号 向指定自定义Fifo中写入数据
		  统计邮箱索引号
参    数: 无
返 回 值: 无
*/
void CanP_CanRx_Irq(void)
{
	CanDrv_RxGetMeesage(&crm);							 //获取挂起邮箱中消息
	switch(crm.FMI)										 //判断消息邮箱索引
	{
	case 0:			//disp
		FifoDrv_BufWrite(&Fifo_Info,crm.Data,crm.DLC);	 //向Fifo_Info中写入消息
		break;
	case 1:			//wifi rx
		FifoDrv_BufWrite(&Fifo_WifiRx,crm.Data,crm.DLC); //向Fifo_WifiRx中写入消息
		break;
	case 2:			//zigbee rx
		FifoDrv_BufWrite(&Fifo_ZigbRx,crm.Data,crm.DLC); //向Fifo_ZigbRx中写入消息
		break;
	default:
		if((crm.FMI >= 3)&&(crm.FMI <= 6))				 //判断邮箱索引
			memcpy(crbuf[crm.FMI-3],crm.Data,8);		 //向crbuf二维数组中写入数据
		break;
	}
	if(crm.FMI <= 6)								
		CanP_RxFMI_Flag |= bit_tab[crm.FMI];			//统计邮箱索引号 
}


/**
函数功能：CAN接收检测
参    数: 无
返 回 值: 无
*/
void CanP_CanRx_Check(void)
{
	while(CanDrv_RxCheck())								//检测接收挂起邮箱不为空								
	{
		CanP_CanRx_Irq();								//从挂起邮箱中获取消息，并向指定自定义Fifo中写入消息
	}
}


/**
函数功能：判断指定自定义Fifo中获取数据并保存
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
				CanP_RxFMI_Flag &= bit_and_tab[i];				//清除邮箱索号 
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
		i = FifoDrv_BufRead(&Fifo_Info,ctbuf,8);// 调试信息
		
		if(i)
		{
			CanDrv_WhaitTxEmpty();
			f = 1;
		}
		i = FifoDrv_BufRead(&Fifo_WifiTx,ctbuf,8); // wifi信息
		if(i)
		{	
			CanDrv_WhaitTxEmpty();
			CanDrv_TxData(ctbuf,i,CAN_SID_HL(ID_WIFI,0),0,&tmbox);
			f = 1;
		}
		
		i = FifoDrv_BufRead(&Fifo_ZigbTx,ctbuf,8);// Zigbee信息
		if(i)
		{	
			CanDrv_WhaitTxEmpty();
			CanDrv_TxData(ctbuf,i,CAN_SID_HL(ID_ZIGBEE,0),0,&tmbox);		
			f = 1;
		}	
	}
	
}

/**
函数功能：上传电量信息
参    数：x1 左侧电量  x2右侧电量
返 回 值：无
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
	
	CanDrv_WhaitTxEmpty();									//判断并等待发送邮箱为空
	CanDrv_TxData(txbuf,3,CAN_SID_HL(ID_HOST,0),0,_NULL);	//向CAN总线发送数据
	
}

/**
未使用
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
函数功能：上传调试信息
参    数：*s 调试信息指针 len 信息长度
返 回 值：无
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
函数功能：发送电机转速
参    数：x1 左侧电机速度  x2 右侧电机转速
返 回 值：无
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

 /** 暂未使用
函数功能：发送电子罗盘数据
参    数：c 指定角度
返 回 值：无
*/
void Send_UpCompass(uint16_t c)
{
	uint8_t tbuf[8];
	tbuf[0] = (c>>8)&0x0ff;
	tbuf[1] = (c)&0x0ff;
	CanDrv_TxData(tbuf,2,CAN_SID_HL(ID_NAVIG,ID_NAVIG),0,_NULL);
	
}


/** 暂未使用
函数功能：CAN总线数据传输测试
参    数：无
返 回 值: 无
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
函数功能: 循迹数据接收
参    数: x1 低八位  x2 高七位
返 回 值: 无
*/
void Host_Receive_UpTrack( u8 x1, u8 x2)  // 循迹数据接收处理函数
{
	Track_buf[0] = x1;
	Track_buf[1] = x2;	 
}


/**
函数功能：获取、处理循迹数据
参    数：mode : TRACK_ALL 获取所有数据 TRACK_Q7 获取七位循迹数据 TRACK_H8 获取八位循迹数据
返 回 值：Rt 循迹数据
*/
u16  Get_Host_UpTrack( u8 mode)  // 获取循迹数据
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
函数功能：设置循迹板发射功率
参    数：power 待设置循迹板功率
返 回 值：无
*/
void Set_Track_Pwr( u16 power) // 设置寻迹板发射功率
{
	u8 txbuf[3];
	
	txbuf[0] = 0X03;  //命令关键字
	txbuf[1] = (power>> 8)&0xff;
	txbuf[2] = (power)&0xff;
	CanDrv_WhaitTxEmpty();
	CanDrv_TxData(txbuf,3,CAN_SID_HL(ID_TRACK,ID_HOST),0,_NULL);
}


/** 暂未使用
函数功能：发送设置循迹板参数
参    数：无
返 回 值：无
*/
void Set_Track_Yzbj(u8 addr, u16 ydata)
{
	u8 txbuf[4];
	
	txbuf[0] = 0X04;  //命令关键字
	txbuf[1] = addr;
	txbuf[2] = (ydata>> 8)&0xff;
	txbuf[3] = (ydata)&0xff;
	CanDrv_WhaitTxEmpty();
	CanDrv_TxData(txbuf,4,CAN_SID_HL(ID_TRACK,ID_HOST),0,_NULL);
	
}

/** 暂未使用
函数功能：设置循迹初始化
参	  数：无
返 回 值：无
*/
void Set_Track_Init( void)
{
	u8 i;          
#define T_POWER 820   // 功率初始化
#define T_BJYZ  7000  // 比较阈值初始化
	
	Set_Track_Pwr( T_POWER);
	delay_ms(100);
	
	for(i=0;i<15;i++)
	{
		Set_Track_Yzbj(i,T_BJYZ);
		delay_ms(30);
	}
}

/** 
函数功能：设置循迹板上传数据时间
参    数: 无
返 回 值: 无
*/
void Host_Set_UpTrack( u8 time)  // 设置循迹数据上传时间间隔
{
#if 1
	u8 txbuf[2];
	txbuf[0] = 0X02;  //命令关键字
	txbuf[1] = time;
	CanDrv_WhaitTxEmpty();
	CanDrv_TxData(txbuf,2,CAN_SID_HL(ID_TRACK,ID_HOST),0,_NULL);   
#endif
	
}

/**
函数功能：将数据发送至ZigBee模块
参    数：无
返 回 值：无
*/
void Send_ZigbeeData_To_Fifo( u8 *p ,u8 len)   
{
	FifoDrv_BufWrite( &Fifo_ZigbTx , p , len);
}

/**
函数功能：将数据发送至WIFI模块
参    数：无
返 回 值：无
*/
void Send_WifiData_To_Fifo( u8 *p ,u8 len)   
{
	FifoDrv_BufWrite( &Fifo_WifiTx , p , len);
}


void Send_InfoData_To_Fifo( u8 *p ,u8 len)   
{
	FifoDrv_BufWrite( &Fifo_Info , p , len);
}