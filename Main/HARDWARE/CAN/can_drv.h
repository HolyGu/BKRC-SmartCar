
#ifndef __CAN_DRV_H__
#define __CAN_DRV_H__

#ifdef __CAN_DRV_C__
#define GLOBAL
#else
#define GLOBAL extern
#endif



GLOBAL uint8_t CanDrv_TxEmptyCheck(void);
GLOBAL uint8_t CanDrv_TxData(uint8_t *tbuf,uint8_t len,uint32_t sid,uint32_t eid,uint8_t *tmbox);
GLOBAL uint8_t CanDrv_TxStatus(uint8_t TMbox);
GLOBAL uint8_t CanDrv_TxDelay(uint8_t tmbox,uint32_t step,uint32_t m);
GLOBAL void CanDrv_WhaitTxEmpty(void);

GLOBAL uint8_t CanDrv_RxCheck(void);
GLOBAL uint8_t CanDrv_RxData(uint8_t *rbuf);
GLOBAL uint8_t CanDrv_RxGetMeesage(CanRxMsg *p);

typedef struct _Can_Filter_Struct
{
	uint16_t sid_id;
	uint16_t sid_mask;
	//uint32_t eid_id;
	//uint32_t eid_mask;
}Can_Filter_Struct;

#define CAN_FLAG_NULL		0xff
#define CAN_FLAG_MASK_16	0
#define CAN_FLAG_LIST_16	1
#define CAN_FLAG_MASK_32	2	
#define CAN_FLAG_LIST_32	3
#define CAN_FLAG_MODEMASK	0x0f
#define CAN_FLAG_IDE_EN		0x10

GLOBAL uint8_t Can_Filter_Flag;


/*
#define CAN_ID_HL(h,l)	(((h<<3)|(l&0x03))&0x3ff)

#define CAN_ID_MOTOR	CAN_ID_HL(0x01,0x00)
#define CAN_ID_TRACK	CAN_ID_HL(0x10,0x00)
#define CAN_ID_NAVIG	CAN_ID_HL(0x20,0x00)
#define CAN_ID_HOST		CAN_ID_HL(0x40,0x00)
#define CAN_ID_WIFI		CAN_ID_HL(0x50,0x00)
#define CAN_ID_ZIGBEE	CAN_ID_HL(0x60,0x00)
#define CAN_ID_DISP		CAN_ID_HL(0xF0,0x00)
*/

#define ID_MOTOR	0x01
#define ID_TRACK	0x07
#define ID_NAVIG	0x08
#define ID_HOST		0x0f
#define ID_WIFI		0x14
#define ID_ZIGBEE	0x15
#define ID_DISP		0x1e

#define ID_ALL		0x3f
#define ID_ZERO		0x00

#define CAN_SID_HL(s,d)	((((uint16_t)s&0x3f)<<5)|(d&0x1f))




void CanDrv_FiterUpdata(Can_Filter_Struct *p,uint8_t len);



#undef GLOBAL

#endif //__CAN_DRV_H__

