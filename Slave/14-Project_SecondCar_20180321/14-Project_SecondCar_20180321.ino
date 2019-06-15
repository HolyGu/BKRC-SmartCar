#include <DCMotor.h>
#include <CoreLED.h>
#include <CoreKEY.h>
#include <CoreBeep.h>
#include <ExtSRAMInterface.h>
#include <LED.h>
#include <BH1750.h>
#include <Command.h>
#include <BEEP.h>
#include <infrare.h>
#include <Ultrasonic.h>


#define TSendCycle	200

uint8_t ZigBee_back[16] = { 0x55, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t DZ_K[8] = {0x55,0x11,0x00,0x00,0x00,0x00,0x00,0xBB};
uint8_t CSB[8] = {0x55,0x22,0x00,0x00,0x00,0x00,0x00,0xBB};
uint8_t Stop[8] = {0x55,0x33,0x00,0x00,0x00,0x00,0x00,0xBB};
uint8_t Light_Down[8] = {0x55,0x44,0x00,0x00,0x00,0x00,0x00,0xBB};

uint8_t ZigBee_command[8];
uint8_t ZigBee_judge;
uint8_t infrare_com[6];
uint8_t sendflag;
unsigned long frisrtime;
unsigned long Tcount;

uint8_t Speed = 60;
uint8_t Whell = 90;

uint8_t Data_Type;
uint8_t Data_Flag;
uint8_t Data_Length;
uint8_t Data_OTABuf[40];
uint8_t OpenMv_AGVData_Flag = 1;
unsigned long openmv_Max_num_time = 12000;
unsigned long openmv_Max_En_timer = 0;
void Analyze_Handle(uint8_t com);
void KEY_Handler(uint8_t k_value);
String DecIntToHexStr(long long num);

void setup()
{
  CoreLED.Initialization();
  CoreKEY.Initialization();
  CoreBeep.Initialization();
  ExtSRAMInterface.Initialization();
  LED.Initialization();
  BH1750.Initialization();
  BEEP.Initialization();
  Infrare.Initialization();
  Ultrasonic.Initialization();
  DCMotor.Initialization();

  Serial.begin(115200);
  while (!Serial);

  sendflag = 0;
  frisrtime = 0;
  Tcount = 0;
}

unsigned long gt_getsuf(unsigned long a, unsigned long b)
{ unsigned long c = 0;
  if (a > b)
  {
    c = a - b;
  } else if (a < b) {
    c = b - a;
  } else {
    c = 0;
  }
}

/**
  函数功能：判断光档位
  参    数：无
  返 回 值：无
*/
void Full_Light_Down()
{
  u16 Light_num[4];     //光强度数值存放数组
  u16 Start_Light = 0;    //起始光强度值
  u8  Light_Text[] = "0000";  //显示光强度数组
  int i=0, j=0;
  u16 temp = 0;
  u8 Light_down = 1;
  
  Infrare.Transmition(Command.HW_Dimming1, 6);
  delay(2000);
  Light_num[0] = BH1750.ReadLightLevel(); //得到四个档位的光强度值
  
  Infrare.Transmition(Command.HW_Dimming1, 6);
  delay(2000);
  Light_num[1] = BH1750.ReadLightLevel();
  
  Infrare.Transmition(Command.HW_Dimming1, 6);
  delay(2000);
  Light_num[2] = BH1750.ReadLightLevel();
  
  Infrare.Transmition(Command.HW_Dimming1, 6);
  delay(2000);
  Light_num[3] = Start_Light = BH1750.ReadLightLevel();
  
  for(i=0;i<4;i++)
  {
    Light_Text[0] = Light_num[i]/1000+0x30;
    Light_Text[1] = Light_num[i]%1000/100+0x30;
    Light_Text[2] = Light_num[i]%100/10+0x30;
    Light_Text[3] = Light_num[i]%10+0x30;
  }
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
    Light_Down[2]=0x01;
    Command.Judgment(Light_Down);
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008,Light_Down,8);
  }
  
  else if (Start_Light == Light_num[1])
  {
    Light_down =2;
    Light_Down[2]=0x02;
    Command.Judgment(Light_Down);
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008,Light_Down,8);
  }
  
  else if (Start_Light == Light_num[2])
  {
    Light_down =3;
    Light_Down[2]=0x03;
    Command.Judgment(Light_Down);
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008,Light_Down,8);
  }
  
  else if (Start_Light == Light_num[3])
  {
    Light_down =4;
    Light_Down[2]=0x04;
    Command.Judgment(Light_Down);
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008,Light_Down,8);
  }
}


/**
  函数功能：调光任务
  参    数：光档位
  返 回 值：无
*/
void Light(uint8_t End_Goal)
{
  uint16_t Light_num[4];     /*光强度数值存放数组*/
  uint16_t Start_Light = 0;    /*起始光强度值*/
  int i = 0, j = 0;
  uint16_t temp = 0;

  if ((End_Goal <= 8) && (End_Goal > 4)) /*对目标档位数据处理*/
  {
    End_Goal = End_Goal - 4;
  }
  if ((End_Goal <= 16) && (End_Goal > 8))
  {
    End_Goal = End_Goal - 8;
  }

  Infrare.Transmition(Command.HW_Dimming1, 6);            /*第一次获取光强度*/
  delay(2000);
  Light_num[0] = BH1750.ReadLightLevel();

  Infrare.Transmition(Command.HW_Dimming1, 6);            /*第二次获取光强度*/
  delay(2000);
  Light_num[1] = BH1750.ReadLightLevel();

  Infrare.Transmition(Command.HW_Dimming1, 6);            /*第三次获取光强度*/
  delay(2000);
  Light_num[2] = BH1750.ReadLightLevel();

  Infrare.Transmition(Command.HW_Dimming1, 6);            /*第四次获取光强度*/
  delay(2000);
  Light_num[3] = Start_Light = BH1750.ReadLightLevel();

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

  for (i = 0; i < 4; i++)
  {
    if (Start_Light == Light_num[i])
    {
      if ((End_Goal + 3 - i) % 4 != 0)
      {
        if (((End_Goal + 3 - i) % 4) == 1)
        {
          Infrare.Transmition(Command.HW_Dimming1, 4);
          break;
        }
        if (((End_Goal + 3 - i) % 4) == 2)
        {
          Infrare.Transmition(Command.HW_Dimming2, 4);
          break;
        }
        if (((End_Goal + 3 - i) % 4) == 3)
        {
          Infrare.Transmition(Command.HW_Dimming3, 4);
          break;
        }
      }
      else
      {
        break;
      }
    }
  }
}

void loop()
{
  frisrtime = millis();

  if (ExtSRAMInterface.ExMem_Read(0x6100) == 0x55)
  {
    ExtSRAMInterface.ExMem_Read_Bytes(ZigBee_command, 8);
    ZigBee_judge = ZigBee_command[6];
    Command.Judgment(ZigBee_command);
    if ((ZigBee_judge == ZigBee_command[6]) && (ZigBee_command[0] == 0x55) &&
        (ZigBee_command[1] == 0x02) && (ZigBee_command[7] == 0xbb))
    {
      Analyze_Handle(ZigBee_command[2]);
    }
  }
  if (OpenMv_AGVData_Flag == 1)
  {
    if (((millis() - frisrtime >= TSendCycle) || (Tcount >= TSendCycle)) && (sendflag == 1))
    {
      uint16_t tp = (uint16_t)(Ultrasonic.Ranging(CM) * 10.0);
      ZigBee_back[5] = (tp >> 8) & 0xff;
      ZigBee_back[4] = tp & 0xff;

      tp = BH1750.ReadLightLevel();
      ZigBee_back[7] = (tp >> 8) & 0xff;
      ZigBee_back[6] = tp & 0xff;

      ZigBee_back[9] = (uint8_t)ExtSRAMInterface.ExMem_Read(0x6003);
      if (ZigBee_back[9] >= 0x80)	ZigBee_back[9] = 0xff - ZigBee_back[9];
      ZigBee_back[8] = (uint8_t)ExtSRAMInterface.ExMem_Read(0x6002);

      ExtSRAMInterface.ExMem_Write_Bytes(0x6080, ZigBee_back, 16);
      Tcount = 0x00;
    }
    else if (sendflag == 1)
    {
      Tcount += (millis() - frisrtime);
    }
  }

  if (OpenMv_AGVData_Flag == 0)
  {
    if (ExtSRAMInterface.ExMem_Read(0x6038) == 0x55)          //检测OpenMV识别结果
    {
      Data_Type = ExtSRAMInterface.ExMem_Read(0x603A);
      Data_Flag = ExtSRAMInterface.ExMem_Read(0x603B);
      Data_Length = ExtSRAMInterface.ExMem_Read(0x603C);
      if (Data_Type == 0x92)
      {
        Data_Length = Data_Length + 6;
        ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OTABuf, Data_Length);
        ExtSRAMInterface.ExMem_Write_Bytes(0x6180, Data_OTABuf, Data_Length);
        OpenMv_AGVData_Flag = 1;
        delay(800);
        for (int i = 0; i < Data_Length; i++)
        {
          Serial.print(Data_OTABuf[i]);
          Serial.print(" ");
        }
        Serial.println("");
      }
    }
  }
  CoreKEY.Kwhile(KEY_Handler);
}


uint8_t ATO_Data[] = {0x55, 0x02, 0x92, 0x00, 0x00, 0x00, 0x00, 0xBB};
#define ATM_Data_Length 48
uint8_t ATM_Data[ATM_Data_Length];
uint8_t Run_Ultrasonic[8] = {0x55, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

uint8_t Light_Run;

/**
  功   能:按键处理函数
  参   数: K_value 按键值
  返回值:无
*/
void KEY_Handler(uint8_t k_value)
{
  switch (k_value)
  {
    case 1:
      ATO_Data[3] = 0x01;           //开始识别
      CoreLED.TurnOnOff(8);
      OpenMv_AGVData_Flag = 0;
      Command.Judgment(ATO_Data);    //计算校验和
      ExtSRAMInterface.ExMem_Write_Bytes(0x6008, ATO_Data, 8);
      break;
    case 2:
      ATO_Data[3] = 0x02;         //停止识别
      CoreLED.TurnOnOff(4);
      Command.Judgment(ATO_Data);    //计算校验和
      ExtSRAMInterface.ExMem_Write_Bytes(0x6008, ATO_Data, 8);
      break;
    case 3:
      CoreLED.TurnOnOff(2);
      DCMotor.SpeedCtr(80,-80);
      delay(450);
      DCMotor.Stop();
      break;
    case 4:
      Full_Light_Down();
      break;
  }
}

/**
  功   能:从车任务处理函数
  参   数:com 主指令
  返回值:无
*/
void Analyze_Handle(uint8_t com)
{
  switch (com)
  {
    case 0x01:                                       //停止
      DCMotor.Stop();
      break;
    case 0x02:                                        //前进
      DCMotor.Go(ZigBee_command[3], (ZigBee_command[4] + (ZigBee_command[5] << 8)));
      ZigBee_back[2] = 0x03;
      break;
    case 0x03:                                        //后退
      DCMotor.Back(ZigBee_command[3], (ZigBee_command[4] + (ZigBee_command[5] << 8)));
      ZigBee_back[2] = 0x03;
      break;
    case 0x04:                                        //左转
      DCMotor.TurnLeft(ZigBee_command[3]);
      ZigBee_back[2] = 0x02;
      break;
    case 0x05:                                        //右转
      DCMotor.TurnRight(ZigBee_command[3]);
      ZigBee_back[2] = 0x02;
      break;
    case 0x06:                                        //循迹
      DCMotor.CarTrack(ZigBee_command[3]);
      ZigBee_back[2] = 0x00;
      break;
    case 0x07:                                        //码盘清零
      Command.Judgment(Command.command01);                        //计算校验和
      ExtSRAMInterface.ExMem_Write_Bytes(Command.command01, 8);   //码盘清零
      break;
    case 0x08:                                        //左转45度
      DCMotor.SpeedCtr(ZigBee_command[3]*(-1),ZigBee_command[3]);
      delay(ZigBee_command[4]+(ZigBee_command[5]*256));
      DCMotor.Stop();
      break;
    case 0x09:                                        //右转45度
      DCMotor.SpeedCtr(ZigBee_command[3],ZigBee_command[3]*(-1));
      delay(ZigBee_command[4]+(ZigBee_command[5]*256));
      DCMotor.Stop();
      break;
    case 0x10:                                       //保存红外数据
      infrare_com[0] = ZigBee_command[3];
      infrare_com[1] = ZigBee_command[4];
      infrare_com[2] = ZigBee_command[5];
      break;
    case 0x11:                                       //保存红外数据
      infrare_com[3] = ZigBee_command[3];
      infrare_com[4] = ZigBee_command[4];
      infrare_com[5] = ZigBee_command[5];
      break;
    case 0x12:                                        //发送红外数据
      Infrare.Transmition(infrare_com, 6);
      break;
    case 0x20:                                        //左右转向灯
      if (ZigBee_command[3] == 0x01)	LED.LeftTurnOn();
      else							LED.LeftTurnOff();
      if (ZigBee_command[4] == 0x01)	LED.RightTurnOn();
      else							LED.RightTurnOff();
      break;
    case 0x30:                                        //蜂鸣器
      if (ZigBee_command[3] == 0x01)	BEEP.TurnOn();
      else							BEEP.TurnOff();
      break;
    case 0x40:                                        //保留
      break;
    case 0x50:                                        //LCD图片上翻页
      Command.Judgment(Command.command13);
      ExtSRAMInterface.ExMem_Write_Bytes(Command.command13, 8);
      break;
    case 0x51:                                        //LCD图片下翻页
      Command.Judgment(Command.command14);
      ExtSRAMInterface.ExMem_Write_Bytes(Command.command14, 8);
      break;
    case 0x61:                                        //光源加一档
      Infrare.Transmition(Command.HW_Dimming1, 4);
      break;
    case 0x62:                                        //光源加二挡
      Infrare.Transmition(Command.HW_Dimming2, 4);
      break;
    case 0x63:                                        //光源加三挡
      Infrare.Transmition(Command.HW_Dimming3, 4);
      break;
    case 0x64:                                        //烽火台开启
      Infrare.Transmition(Command.HW_K, 6);
      break;
    case 0x65:                                        //烽火台关闭
      Infrare.Transmition(Command.HW_G, 6);
      break;
    case 0x66:                                        //开始调光
      Light(ZigBee_command[3]);
      break;
    case 0x67:                                        //开始特殊地形

      break;
    case 0x80:                                        //从车返回数据切换
      if (ZigBee_command[3] == 0x01)
      {
        sendflag = 0x01;                             //上传从车数据
        OpenMv_AGVData_Flag = 1;
      }
      else {
        sendflag = 0x00;                              //关闭上从车数据
      }
      break;
    case 0x92:                                        //OpenMv二维码识别
      if (ZigBee_command[3] == 0x01)
      {
        ATO_Data[3] = 0x01;                         //开始识别
        OpenMv_AGVData_Flag = 0;                    //关闭上传从车数据
        Command.Judgment(ATO_Data);                 //计算校验和
        ExtSRAMInterface.ExMem_Write_Bytes(0x6008, ATO_Data, 8);
        delay(200);
        openmv_Max_En_timer = millis() + openmv_Max_num_time;
      } else if (ZigBee_command[3] == 0x02) {
        ATO_Data[3] = 0x02;                      //停止识别
        CoreLED.TurnOnOff(4);
        Command.Judgment(ATO_Data);              //计算校验和
        ExtSRAMInterface.ExMem_Write_Bytes(0x6008, ATO_Data, 8);
      }
      break;

    case 0x93:
        DCMotor.CarTrack(Speed);
        delay(1000);
        DCMotor.Go(Speed,380);
        delay(200);
        DCMotor.TurnLeft(Whell);
        delay(800);
        DCMotor.CarTrack(Speed);
        delay(1000);
        DCMotor.Go(Speed,380);
        delay(200);
        DCMotor.TurnRight(Whell);
        delay(800);
        Command.Judgment(DZ_K);
        ExtSRAMInterface.ExMem_Write_Bytes(0x6008, DZ_K, 8);
    delay(1000);
        DCMotor.CarTrack(Speed);
        delay(1000);
        DCMotor.Go(Speed,380);
        delay(200);
        Command.Judgment(Stop);
        ExtSRAMInterface.ExMem_Write_Bytes(0x6008, Stop, 8);
      break;

    case 0x94:
        DCMotor.TurnRight(Whell);
        delay(800);
        DCMotor.TurnRight(Whell);
        delay(800);
        DCMotor.CarTrack(Speed);
        delay(1000);
        DCMotor.Go(Speed,380);
        delay(200);
        DCMotor.TurnLeft(Whell);
        delay(800);
        DCMotor.CarTrack(Speed);
        delay(1000);
        DCMotor.Go(Speed,380);
        delay(200);
        DCMotor.TurnRight(Whell);
        delay(800);
        Command.Judgment(DZ_K);
        ExtSRAMInterface.ExMem_Write_Bytes(0x6008, DZ_K, 8);
    delay(1000);
        DCMotor.CarTrack(Speed);
        delay(1000);
        DCMotor.Go(Speed,380);
        delay(200);
        Command.Judgment(Stop);
        ExtSRAMInterface.ExMem_Write_Bytes(0x6008, Stop, 8);
      break;

    case 0x95:
        DCMotor.TurnRight(Whell);
        delay(800);
        DCMotor.CarTrack(Speed);
        delay(1000);
        DCMotor.Go(Speed,380);
        delay(200);
        DCMotor.TurnLeft(Whell);
        delay(800);
        DCMotor.CarTrack(Speed);
        delay(1000);
        DCMotor.Go(Speed,380);
        delay(200);
        DCMotor.TurnRight(Whell);
        delay(800);
        Command.Judgment(DZ_K);
        ExtSRAMInterface.ExMem_Write_Bytes(0x6008, DZ_K, 8);
        delay(1000);
        DCMotor.CarTrack(Speed);
        delay(1000);
        DCMotor.Go(Speed,380);
        delay(200);
        Command.Judgment(Stop);
        ExtSRAMInterface.ExMem_Write_Bytes(0x6008, Stop, 8);
      break;

    case 0x96:
        DCMotor.TurnLeft(Whell);
        delay(800);
        DCMotor.CarTrack(Speed);
        delay(1000);
        DCMotor.Go(Speed,380);
        delay(200);
        DCMotor.TurnLeft(Whell);
        delay(800);
        DCMotor.CarTrack(Speed);
        delay(1000);
        DCMotor.Go(Speed,380);
        delay(200);
        DCMotor.TurnRight(Whell);
        delay(800);
        Command.Judgment(DZ_K);
        ExtSRAMInterface.ExMem_Write_Bytes(0x6008, DZ_K, 8);
        delay(1000);
        DCMotor.CarTrack(Speed);
        delay(1000);
        DCMotor.Go(Speed,380);
        delay(200);
        Command.Judgment(Stop);
        ExtSRAMInterface.ExMem_Write_Bytes(0x6008, Stop, 8);
      break;
      
    case 0x98:
        DCMotor.CarTrack(Speed);
        delay(1000);
        DCMotor.Go(Speed,380);
        delay(200);
        DCMotor.CarTrack(Speed);
        delay(1000);
        DCMotor.Go(Speed,380);
        delay(200);
        DCMotor.CarTrack(Speed);
        delay(1000);
        DCMotor.Go(Speed,380);
        delay(200);
        DCMotor.TurnRight(Whell);
        delay(1000);
        DCMotor.CarTrack(Speed);
        delay(1000);
        Light(2);
      break;
      
    case 0x99:
        DCMotor.CarTrack(Speed);
        delay(1000);
        DCMotor.Go(Speed,380);
        delay(200);
        DCMotor.TurnRight(Whell);
        delay(800);
        DCMotor.CarTrack(Speed);
        delay(1000);
        DCMotor.Go(Speed,380);
        delay(200);
        DCMotor.TurnLeft(Whell);
        delay(800);
        Light(3);
        delay(1200);
        DCMotor.TurnRight(Whell);
        delay(800);
        DCMotor.CarTrack(Speed);
        delay(1000);
        DCMotor.Go(Speed,380);
        delay(200);
        DCMotor.TurnLeft(Whell);
        delay(800);
        DCMotor.TurnLeft(Whell);
        delay(800);
        DCMotor.Back(Speed,1100);
        Command.Judgment(Stop);
        ExtSRAMInterface.ExMem_Write_Bytes(0x6008, Stop, 8);
      break;
      
    default:
      break;
  }
}
