#ifndef __BH1750_H
#define __BH1750_H	 
#include "sys.h"

//IO��������	 
#define IIC_SCL    PBout(6) //SCL 
#define IIC_SDA    PBout(7) //SDA���	 
#define READ_SDA   PBin(7)  //SDA���� 
#define ADDR 	   PGout(15) 

void BH1750_Configure(void);
uint16_t Get_Bh_Value(void);

#endif
