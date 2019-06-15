#include "sys.h"
#include "rc522.h"
#include "delay.h"
#include "string.h" 
#include "cba.h"
#include "can_user.h"
#include "canp_hostcom.h"
#include "Timer.h"
#include "roadway_check.h"

#define MAXRLEN 18  

#define MF522_RST   PFout(1)

/*RFID全局变量*/
#define  card_addr          0x00            // 块地址
#define  card_key    (card_addr/4)*4+3	    // 块地址对应密钥块地址

u8 RFID_Card_Position = 4;
u8 RFID_Run_Flag = 0;
u8 RFID_Error1 = 0;
u8 RFID_Error2 = 0;
u8 RFID_Error3 = 0;
u8 RFID_Error4 = 0;
u8 RFID_Error5 = 0;

u8 xk_sd = 100;
u8 xk_mp = 100;

void delay_ns(u32 ns)
{
  u32 i;
  for(i=0;i<ns;i++)
  {
    __nop();
    __nop();
    __nop();
  }
}

u8 TXRFID[16] =   {0x41,0x31,0x42,0x32,0x43,0x33,0x44,0x34,0x46,0x31,0x42,0x32,0x43,0x33,0x44,0x34};
u8 RXRFID[16];
/****************************************************************
函数功能：叼炸天的寻卡模式-1
参	  数：无
返回  值：无
*****************************************************************/
void Go_Find_cards1()
{
	char status = 0;
	u8 CT[2];									    //卡类型
	u8 i = 0;
	if(RFID_Write_Flag == 1)
	{
		RFID_Run_Flag = 1;
	}
	else
	{
		RFID_Run_Flag = 0;
		RFID_Error1 = 1;
		RFID_Write_Flag = 0;
		Full_Automatic_GO(60,430);
	}
	while(RFID_Run_Flag == 1)
	{
		Full_Automatic_GO(xk_sd,xk_mp);
		i = i + 1;
		status = PcdRequest(PICC_REQALL,CT);	    //寻卡
		if(status == MI_OK)							//寻卡成功
		{
			RFID_Card_Position = 1;					//RFID 位置坐标
			Read_Card();						//读卡操作
			RFID_Run_Flag = 0;
			RFID_Write_Flag = 0;
		} 
		else
		{
			if(i == 4)
			{
				RFID_Run_Flag = 0;
				RFID_Write_Flag = 0;
			}
		}
	}
}
/****************************************************************
函数功能：叼炸天的寻卡模式-2
参	  数：无
返回  值：无
*****************************************************************/
void Go_Find_cards2()
{
	char status = 0;
	u8 CT[2];									    //卡类型
	u8 i = 0;
	if(RFID_Write_Flag == 1)
	{
		RFID_Run_Flag = 1;
	}
	else
	{
		RFID_Run_Flag = 0;
		RFID_Error2 = 1;
		RFID_Write_Flag = 0;
		Full_Automatic_GO(60,430);
	}
	while(RFID_Run_Flag == 1)
	{
		Full_Automatic_GO(xk_sd,xk_mp);
		i = i + 1;
		status = PcdRequest(PICC_REQALL,CT);	    //寻卡
		if(status == MI_OK)							//寻卡成功
		{
			RFID_Card_Position = 2;					//RFID 位置坐标
			Read_Card();						//读卡操作
			RFID_Run_Flag = 0;
			RFID_Write_Flag = 0;
		} 
		else
		{
			if(i == 4)
			{
				RFID_Run_Flag = 0;
				RFID_Write_Flag = 0;
			}
		}
	}
}
/****************************************************************
函数功能：叼炸天的寻卡模式-3
参	  数：无
返回  值：无
*****************************************************************/
void Go_Find_cards3()
{
	char status = 0;
	u8 CT[2];									    //卡类型
	u8 i = 0;
	if(RFID_Write_Flag == 1)
	{
		RFID_Run_Flag = 1;
	}
	else
	{
		RFID_Run_Flag = 0;
		RFID_Error3 = 1;
		RFID_Write_Flag = 0;
		//Full_Automatic_GO(60,430);
	}
	while(RFID_Run_Flag == 1)
	{
		Full_Automatic_GO(xk_sd,xk_mp);
		i = i + 1;
		status = PcdRequest(PICC_REQALL,CT);	    //寻卡
		if(status == MI_OK)							//寻卡成功
		{
			RFID_Card_Position = 3;					//RFID 位置坐标
			Read_Card();						//读卡操作
			RFID_Run_Flag = 0;
			RFID_Write_Flag = 0;
		} 
		else
		{
			if(i == 4)
			{
				RFID_Run_Flag = 0;
				RFID_Write_Flag = 0;
			}
		}
	}
}
/****************************************************************
函数功能：叼炸天的寻卡模式-4
参	  数：无
返回  值：无
*****************************************************************/
void Go_Find_cards4()
{
	char status = 0;
	u8 CT[2];									    //卡类型
	u8 i = 0;
	if(RFID_Write_Flag == 1)
	{
		RFID_Run_Flag = 1;
	}
	else
	{
		RFID_Run_Flag = 0;
		RFID_Error4 = 1;
		RFID_Write_Flag = 0;
		Full_Automatic_GO(60,430);
	}
	while(RFID_Run_Flag == 1)
	{
		Full_Automatic_GO(xk_sd,xk_mp);
		i = i + 1;
		status = PcdRequest(PICC_REQALL,CT);	    //寻卡
		if(status == MI_OK)							//寻卡成功
		{
			RFID_Card_Position = 4;					//RFID 位置坐标
			Read_Card();						//读卡操作
			RFID_Run_Flag = 0;
			RFID_Write_Flag = 0;
		} 
		else
		{
			if(i == 4)
			{
				RFID_Run_Flag = 0;
				RFID_Write_Flag = 0;
			}
		}
	}
}
/****************************************************************
函数功能：叼炸天的寻卡模式-5
参	  数：无
返回  值：无
*****************************************************************/
void Go_Find_cards5()
{
	char status = 0;
	u8 CT[2];									    //卡类型
	u8 i = 0;
	if(RFID_Write_Flag == 1)
	{
		RFID_Run_Flag = 1;
	}
	else
	{
		RFID_Run_Flag = 0;
		RFID_Error5 = 1;
		RFID_Write_Flag = 0;
		Full_Automatic_GO(60,430);
	}
	while(RFID_Run_Flag == 1)
	{
		Full_Automatic_GO(xk_sd,xk_mp);
		i = i + 1;
		status = PcdRequest(PICC_REQALL,CT);	    //寻卡
		if(status == MI_OK)							//寻卡成功
		{
			RFID_Card_Position = 5;					//RFID 位置坐标
			Read_Card();						//读卡操作
			RFID_Run_Flag = 0;
			RFID_Write_Flag = 0;
		} 
		else
		{
			if(i == 4)
			{
				RFID_Run_Flag = 0;
				RFID_Write_Flag = 0;
			}
		}
	}
}
/****************************************************************
函数功能：之前版本的读卡程序
参	  数：无
返回  值：无
*****************************************************************/
void Full_Read_Card()
{
	char status = 0;
	u8 CT[2];									//卡类型
	u8 SN[4]; 									//卡号
	u8 KEY[6]={0xff,0xff,0xff,0xff,0xff,0xff};  //密钥
	u8 RXRFIDH[8];
	
	u8 i = 0, j = 0;

			status = PcdAnticoll(SN);				//防冲撞
			if(status == MI_OK)
			{
				status=MI_ERR;
				Send_Debug_Info("1\n",3);
				
				status =PcdSelect(SN);				//选定此卡
				if(status == MI_OK)					//选定成功
				{
					status=MI_ERR;
					Send_Debug_Info("2\n",3);
					
					//status =PcdAuthState(0x60,card_key,KEY,SN);		//验证密钥
					status =PcdAuthState(0x60,0x03,KEY,SN);		//验证密钥
					if(status == MI_OK)
					{
						status = MI_ERR;
						Send_Debug_Info("3\n",3);
						
						status=PcdRead(card_addr,RXRFID);				//读卡
						if(status == MI_OK)
						{
							status = MI_ERR;
						MP_SPK = 0;						
							Send_Debug_Info("4\n",3);
							for(i=0;i<8;i++)
							{
								RXRFIDH[i] = RXRFID[i+8];
							}
							Send_Debug_Info(RXRFID,8);
//							Send_Debug_Info(RXRFIDH,8);
//							Send_Debug_Info("\n",2);
							Send_ZigBee_Info( RXRFID ,8);
						}
						delay_ms(200);
						MP_SPK = 0;						
					}
				}
			}
//	}
}
/****************************************************************
函数功能：RFID读写卡
参	  数：
返回  值：无
*****************************************************************/
void Read_Card(void)
{
	char status = 0;
	u8 CT[2];									//卡类型
	u8 SN[4]; 									//卡号
	u8 KEY[6]={0xff,0xff,0xff,0xff,0xff,0xff};  //密钥
	u8 TEST[12] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	u8 s = 0x00;       							// #define   Status2Reg   0x08	
	u8 RXRFIDH[8];
	#define  DATA_LEN    16                     // 定义数据字节长度	

//		status = PcdRequest(PICC_REQALL,CT);		//寻卡
//		if(status == MI_OK)							//寻卡成功
//		{
//			Send_Debug_Info("xkYes\n",7);
			status=MI_ERR;
			LED1 = 1;
			status = PcdAnticoll(SN);				//防冲撞
			if(status == MI_OK)
			{
				//Send_Debug_Info("fcYes\n",7);
				status=MI_ERR;
				LED2 = 1;				
				status =PcdSelect(SN);				//选定此卡
				if(status == MI_OK)					//选定成功
				{
					//Send_Debug_Info("xdYes\n",7);
					status=MI_ERR;	
					LED3 = 1;
					MP_SPK = 1;
					delay_ms(200);
					status =PcdAuthState(0x60,0x03,KEY,SN);		//验证密钥
					if(status == MI_OK)
					{
						//Send_Debug_Info("yzYes\n",7);
						status = MI_ERR;
						LED4 = 1;						
//						status = PcdWrite(s,TXRFID);			//写卡
//						if(status == MI_OK)
//						{
//							Send_Debug_Info("xkYes\n",7);
//							status = MI_ERR;
//						}							
						status=PcdRead(s,RXRFID);				//读卡
						if(status == MI_OK)
						{
							//Send_Debug_Info("dkYes\n",7);
							Send_ZigBee_Info( RXRFID ,8);
						
							status = MI_ERR;
							
						}
						MP_SPK = 0;						
					}
				}
			}
//	}
}
/****************************************************************
函数功能：RFID读写卡
参	  数：
返回  值：无
*****************************************************************/
char Rcc522_MoveRead_Card(void)
{
	char status = 0;
	u8 CT[2];									//卡类型
	u8 SN[4]; 									//卡号
	u8 KEY[6]={0xff,0xff,0xff,0xff,0xff,0xff};  //密钥
	u8 s = 0x01;       							// #define   Status2Reg   0x08	
	u8 RXRFIDH[8];
	#define  DATA_LEN    16                     // 定义数据字节长度	

//		status = PcdRequest(PICC_REQALL,CT);		//寻卡
//		if(status == MI_OK)							//寻卡成功
//		{
			status=MI_ERR;
			LED1 = 1;
			status = PcdAnticoll(SN);				//防冲撞
			if(status == MI_OK)
			{
				status=MI_ERR;
				LED2 = 1;				
				status =PcdSelect(SN);				//选定此卡
				if(status == MI_OK)					//选定成功
				{
					status=MI_ERR;	
					LED3 = 1;
					MP_SPK = 1;
					delay_ms(200);
					status =PcdAuthState(0x60,0x03,KEY,SN);		//验证密钥
					if(status == MI_OK)
					{
						status = MI_ERR;
						LED4 = 1;						
						status = PcdWrite(s,TXRFID);			//写卡
						if(status == MI_OK)
						{
							status = MI_ERR;
						}							
						status=PcdRead(s,RXRFID);				//读卡
						if(status == MI_OK)
						{
							status = MI_ERR;							
						}
						MP_SPK = 0;						
					}
				}
			}
//	}
			return status;
}

uint8_t RC522_Search_Card(void)
{
	char status = 0;
	u8 CT[2];
	status = PcdRequest(PICC_REQALL,CT);		//寻卡
	if(status == MI_OK)							//寻卡成功
	{
		return 1;
	} else return 0;
}

//初始化IO 串口1 
//bound:波特率
void RC522_Uart_init(u32 baudrate)
{
	GPIO_InitTypeDef  GPIO_TypeDefStructure;
	USART_InitTypeDef USART_TypeDefStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
	
	//PA9-Tx
	GPIO_TypeDefStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;
	GPIO_TypeDefStructure.GPIO_Mode = GPIO_Mode_AF;		//复用功能
	GPIO_TypeDefStructure.GPIO_OType = GPIO_OType_PP; //推挽输出
	GPIO_TypeDefStructure.GPIO_PuPd = GPIO_PuPd_UP;   //上拉
	GPIO_TypeDefStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA,&GPIO_TypeDefStructure);
	
	USART_TypeDefStructure.USART_BaudRate = baudrate;					       //波特率
	USART_TypeDefStructure.USART_HardwareFlowControl = 				       //无硬件控制流
												 USART_HardwareFlowControl_None;  
	USART_TypeDefStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx; //接收与发送模式
	USART_TypeDefStructure.USART_Parity = USART_Parity_No; 		       //无校验位
	USART_TypeDefStructure.USART_StopBits = USART_StopBits_1;        //停止位1
	USART_TypeDefStructure.USART_WordLength = USART_WordLength_8b;   //数据位8位
	USART_Init(USART1,&USART_TypeDefStructure);

	USART_Cmd(USART1, ENABLE);                    //使能串口 

}

void InitRc522(void)
{
	  PcdReset();
	  PcdAntennaOff();
	  delay_ms(2);  
	  PcdAntennaOn();
	  M500PcdConfigISOType( 'A' );
}

void Readcard_daivce_Init(void)
{
	RC522_Uart_init(9600);	// 串口初始化为9600
	delay_ms(500);
	InitRc522();			//读卡器初始化
}

void Reset_RC522(void)
{
  PcdReset();
  PcdAntennaOff();
  delay_ms(2);  
  PcdAntennaOn();
}   
/////////////////////////////////////////////////////////////////////
//功    能：寻卡
//参数说明: req_code[IN]:寻卡方式
//                0x52 = 寻感应区内所有符合14443A标准的卡
//                0x26 = 寻未进入休眠状态的卡
//          	  pTagType[OUT]：卡片类型代码
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdRequest(unsigned char req_code,unsigned char *pTagType)
{
   char status;  
   unsigned int  unLen;
   unsigned char ucComMF522Buf[MAXRLEN]; 
//  unsigned char xTest ;
   ClearBitMask(Status2Reg,0x08);
   WriteRawRC(BitFramingReg,0x07);

//  xTest = ReadRawRC(BitFramingReg);
//  if(xTest == 0x07 )
 //   { LED_GREEN  =0 ;}
 // else {LED_GREEN =1 ;while(1){}}
   SetBitMask(TxControlReg,0x03);
 
   ucComMF522Buf[0] = req_code;

   status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);
//     if(status  == MI_OK )
//   { LED_GREEN  =0 ;}
//   else {LED_GREEN =1 ;}
   if ((status == MI_OK) && (unLen == 0x10))
   {    
       *pTagType     = ucComMF522Buf[0];
       *(pTagType+1) = ucComMF522Buf[1];
   }
   else
   {   status = MI_ERR;   }
   return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：防冲撞
//参数说明: pSnr[OUT]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////  
char PcdAnticoll(unsigned char *pSnr)
{
    char status;
    unsigned char i,snr_check=0;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 
    

    ClearBitMask(Status2Reg,0x08);
    WriteRawRC(BitFramingReg,0x00);
    ClearBitMask(CollReg,0x80);
 
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x20;

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);

    if (status == MI_OK)
    {
    	 for (i=0; i<4; i++)
         {   
             *(pSnr+i)  = ucComMF522Buf[i];
             snr_check ^= ucComMF522Buf[i];
         }
         if (snr_check != ucComMF522Buf[i])
         {   status = MI_ERR;    }
    }
    
    SetBitMask(CollReg,0x80);
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：选定卡片
//参数说明: pSnr[IN]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdSelect(unsigned char *pSnr)
{
    char status;
    unsigned char i;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x70;
    ucComMF522Buf[6] = 0;
    for (i=0; i<4; i++)
    {
    	ucComMF522Buf[i+2] = *(pSnr+i);
    	ucComMF522Buf[6]  ^= *(pSnr+i);
    }
    CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);
  
    ClearBitMask(Status2Reg,0x08);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);
    
    if ((status == MI_OK) && (unLen == 0x18))
    {   status = MI_OK;  }
    else
    {   status = MI_ERR;    }

    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：验证卡片密码
//参数说明: auth_mode[IN]: 密码验证模式
//                 0x60 = 验证A密钥
//                 0x61 = 验证B密钥 
//          addr[IN]：块地址
//          pKey[IN]：密码
//          pSnr[IN]：卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////               
char PcdAuthState(unsigned char auth_mode,unsigned char addr,unsigned char *pKey,unsigned char *pSnr)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = auth_mode;
    ucComMF522Buf[1] = addr;
    for (i=0; i<6; i++)
    {    ucComMF522Buf[i+2] = *(pKey+i);   }
    for (i=0; i<6; i++)
    {    ucComMF522Buf[i+8] = *(pSnr+i);   }
 //   memcpy(&ucComMF522Buf[2], pKey, 6); 
 //   memcpy(&ucComMF522Buf[8], pSnr, 4); 
    
    status = PcdComMF522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);
    if ((status != MI_OK) || (!(ReadRawRC(Status2Reg) & 0x08)))
    {   status = MI_ERR;   }
    
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：读取M1卡一块数据
//参数说明: addr[IN]：块地址
//          pData[OUT]：读出的数据，16字节
//返    回: 成功返回MI_OK
///////////////////////////////////////////////////////////////////// 
char PcdRead(unsigned char addr,unsigned char *pData)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_READ;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
   
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    if ((status == MI_OK) && (unLen == 0x90))
 //   {   memcpy(pData, ucComMF522Buf, 16);   }
    {
        for (i=0; i<16; i++)
        {    *(pData+i) = ucComMF522Buf[i];   }
    }
    else
    {   status = MI_ERR;   }
    
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：写数据到M1卡一块
//参数说明: addr[IN]：块地址
//          pData[IN]：写入的数据，16字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////                  
char PcdWrite(unsigned char addr,unsigned char *pData)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_WRITE;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
        
    if (status == MI_OK)
    {
        //memcpy(ucComMF522Buf, pData, 16);
        for (i=0; i<16; i++)
        {    ucComMF522Buf[i] = *(pData+i);   }
        CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);

        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,18,ucComMF522Buf,&unLen);
        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   status = MI_ERR;   }
    }
    
    return status;
}



/////////////////////////////////////////////////////////////////////
//功    能：命令卡片进入休眠状态
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdHalt(void)
{
    char status;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_HALT;
    ucComMF522Buf[1] = 0;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    return MI_OK;
}

/////////////////////////////////////////////////////////////////////
//用MF522计算CRC16函数
/////////////////////////////////////////////////////////////////////
void CalulateCRC(unsigned char *pIndata,unsigned char len,unsigned char *pOutData)
{
    unsigned char i,n;
    ClearBitMask(DivIrqReg,0x04);
    WriteRawRC(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);
    for (i=0; i<len; i++)
    {   WriteRawRC(FIFODataReg, *(pIndata+i));   }
    WriteRawRC(CommandReg, PCD_CALCCRC);
    i = 0xFF;
    do 
    {
        n = ReadRawRC(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04));
    pOutData[0] = ReadRawRC(CRCResultRegL);
    pOutData[1] = ReadRawRC(CRCResultRegM);
}

/////////////////////////////////////////////////////////////////////
//功    能：复位RC522
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdReset(void)
{
	/*
    MF522_RST=1;
    delay_ns(10);
    MF522_RST=0;
    delay_ns(10);
    MF522_RST=1;
    delay_ns(10);
	*/
    WriteRawRC(CommandReg,PCD_RESETPHASE);
	  delay_ns(10);

    WriteRawRC(ModeReg,0x3D);            //和Mifare卡通讯，CRC初始值0x6363
    WriteRawRC(TReloadRegL,30);           
    WriteRawRC(TReloadRegH,0);
    WriteRawRC(TModeReg,0x8D);
    WriteRawRC(TPrescalerReg,0x3E);
    WriteRawRC(TxAutoReg,0x40); 
	  
    return MI_OK;
}
//////////////////////////////////////////////////////////////////////
//设置RC522的工作方式 
//////////////////////////////////////////////////////////////////////
char M500PcdConfigISOType(unsigned char type)
{
   if (type == 'A')                     //ISO14443_A
   { 
		   
       ClearBitMask(Status2Reg,0x08);
       WriteRawRC(ModeReg,0x3D);//3F
       WriteRawRC(RxSelReg,0x86);//84
       WriteRawRC(RFCfgReg,0x7F);   //4F
   	   WriteRawRC(TReloadRegL,30);//tmoLength);// TReloadVal = 'h6a =tmoLength(dec) 
			 WriteRawRC(TReloadRegH,0);
       WriteRawRC(TModeReg,0x8D);
	     WriteRawRC(TPrescalerReg,0x3E);
			 delay_ms(10);
       PcdAntennaOn();
//       WriteRawRC(SerialSpeedReg,0x7A);//
		 
		   //WriteRawRC(CommandReg,0x00);//
		   
   }
   else{ return -1; }
   
   return MI_OK;
}
/*************************************************
Function:       Send_data
Description:
     write a byte to serial port
Parameter:
     ch            the byte to write
Return:
     None
**************************************************/
void Send_data(unsigned char ch)
{	
	  USART1->SR;
	while (!(USART1->SR & USART_FLAG_TXE));
    USART_SendData(USART1,ch); 
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
	
}

/*************************************************
Function:       Rece_data
Description:
     get a byte from serial port
Parameter:
     ch            the byte to save the received byte
     WaitTime      the counter for polling, avoid endless loop.
Return:
     STATUS_SUCCESS         get a byte data successfully
     STATUS_IO_TIMEOUT      time out
**************************************************/
short Rece_data(unsigned char *ch, unsigned int WaitTime)
{
	/*
    unsigned int j;
    j=0;
  while(!(USART1->SR & USART_FLAG_RXNE))
	{
		j++;
		if(j>=WaitTime)          //avoid endless loop
			break;
	}
	if(j<WaitTime)
	{
		*ch = USART_ReceiveData(USART1);
		//RI=0;
		return STATUS_SUCCESS;
	}
	else
	    return STATUS_IO_TIMEOUT;
	*/
	
//	while(!(UART4->SR & USART_FLAG_RXNE));
//	*ch = (uint8_t)USART_ReceiveData(UART4);
//	return STATUS_SUCCESS;
	
//	while(!(USART3->SR & USART_FLAG_RXNE));
//	*ch = (uint8_t)USART_ReceiveData(USART3);
//	return STATUS_SUCCESS;



	while(!(USART1->SR & USART_FLAG_RXNE))
	{
		 if(USART_GetFlagStatus(USART1,USART_FLAG_ORE)==SET)
        {
						Send_Debug_Info("ore\n",13);
						USART_ClearFlag(USART1,USART_FLAG_ORE); //读SR其实就是清除标志
						(uint8_t)USART_ReceiveData(USART1);
								
         }
	};
	*ch = (uint8_t)USART_ReceiveData(USART1);
	return STATUS_SUCCESS;
}
/////////////////////////////////////////////////////////////////////
//功    能：读RC632寄存器
//参数说明：Address[IN]:寄存器地址
//返    回：读出的值
/////////////////////////////////////////////////////////////////////
unsigned char ReadRawRC(unsigned char Address)
{
    unsigned char RegVal;
    short status;
    Address = (Address & 0x3f) | 0x80;   //code the first byte
    Send_data(Address);
    status = Rece_data(&RegVal, 10000);
    if(status != STATUS_SUCCESS)
        return 0xff;
    return RegVal;
}

/////////////////////////////////////////////////////////////////////
//功    能：写RC632寄存器
//参数说明：Address[IN]:寄存器地址
//          value[IN]:写入的值
/////////////////////////////////////////////////////////////////////
void WriteRawRC(unsigned char Address, unsigned char value)
{  
    unsigned char EchoByte;
    short status;
    Address &= 0x3f;   //code the first byte
    Send_data(Address);
    Send_data(value);
		
    status = Rece_data(&EchoByte, 10000);
}

/////////////////////////////////////////////////////////////////////
//功    能：置RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:置位值
/////////////////////////////////////////////////////////////////////
void SetBitMask(unsigned char reg,unsigned char mask)  
{
    char tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg,tmp | mask);  // set bit mask
}

/////////////////////////////////////////////////////////////////////
//功    能：清RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:清位值
/////////////////////////////////////////////////////////////////////
void ClearBitMask(unsigned char reg,unsigned char mask)  
{
    char tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg, tmp & ~mask);  // clear bit mask
} 

/////////////////////////////////////////////////////////////////////
//功    能：通过RC522和ISO14443卡通讯
//参数说明：Command[IN]:RC522命令字
//          pInData[IN]:通过RC522发送到卡片的数据
//          InLenByte[IN]:发送数据的字节长度
//          pOutData[OUT]:接收到的卡片返回数据
//          *pOutLenBit[OUT]:返回数据的位长度
/////////////////////////////////////////////////////////////////////
char PcdComMF522(unsigned char Command, 
                 unsigned char *pInData, 
                 unsigned char InLenByte,
                 unsigned char *pOutData, 
                 unsigned int  *pOutLenBit)
{
    char status = MI_ERR;
    unsigned char irqEn   = 0x00;
    unsigned char waitFor = 0x00;
    unsigned char lastBits;
    unsigned char n;
    unsigned int i;
    switch (Command)
    {
       case PCD_AUTHENT:
          irqEn   = 0x12;
          waitFor = 0x10;
          break;
       case PCD_TRANSCEIVE:
          irqEn   = 0x77;
          waitFor = 0x30;
          break;
       default:
         break;
    }
   
    WriteRawRC(ComIEnReg,irqEn|0x80);
    ClearBitMask(ComIrqReg,0x80);
    WriteRawRC(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);
    
    for (i=0; i<InLenByte; i++)
    {   WriteRawRC(FIFODataReg, pInData[i]);    }
    WriteRawRC(CommandReg, Command);
   
    
    if (Command == PCD_TRANSCEIVE)
    {    SetBitMask(BitFramingReg,0x80);  }
    
//    i = 600;//根据时钟频率调整，操作M1卡最大等待时间25ms
 i = 2000;
    do 
    {
         n = ReadRawRC(ComIrqReg);
         i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitFor));
    ClearBitMask(BitFramingReg,0x80);
	      
    if (i!=0)
    {    
         if(!(ReadRawRC(ErrorReg)&0x1B))
         {
             status = MI_OK;
             if (n & irqEn & 0x01)
             {   status = MI_NOTAGERR;   }
             if (Command == PCD_TRANSCEIVE)
             {
               	n = ReadRawRC(FIFOLevelReg);
              	lastBits = ReadRawRC(ControlReg) & 0x07;
                if (lastBits)
                {   *pOutLenBit = (n-1)*8 + lastBits;   }
                else
                {   *pOutLenBit = n*8;   }
                if (n == 0)
                {   n = 1;    }
                if (n > MAXRLEN)
                {   n = MAXRLEN;   }
                for (i=0; i<n; i++)
                {   pOutData[i] = ReadRawRC(FIFODataReg);    }
            }
         }
         else
         {   status = MI_ERR;   }
        
   }
   

   SetBitMask(ControlReg,0x80);           // stop timer now
   WriteRawRC(CommandReg,PCD_IDLE); 
   return status;
}


/////////////////////////////////////////////////////////////////////
//开启天线  
//每次启动或关闭天险发射之间应至少有1ms的间隔
/////////////////////////////////////////////////////////////////////
void PcdAntennaOn()
{
    unsigned char i;
    i = ReadRawRC(TxControlReg);
    if (!(i & 0x03))
    {
        SetBitMask(TxControlReg, 0x03);
    }
}


/////////////////////////////////////////////////////////////////////
//关闭天线
/////////////////////////////////////////////////////////////////////
void PcdAntennaOff()
{
    ClearBitMask(TxControlReg, 0x03);
}

//等待卡离开
void WaitCardOff(void)
{
	unsigned char status, TagType[2];

	while(1)
	{
		status = PcdRequest(REQ_ALL, TagType);
		if(status)
		{
			status = PcdRequest(REQ_ALL, TagType);
			if(status)
			{
				status = PcdRequest(REQ_ALL, TagType);
				if(status)
				{
					return;
				}
			}
		}
		delay_ms(10);
	}
}

