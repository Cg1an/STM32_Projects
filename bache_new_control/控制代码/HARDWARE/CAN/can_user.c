#include "can.h"
#include "canuser.h"
#include "delay.h"
#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//CAN驱动 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/7
//版本：V1.0 
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 

u8 F608 = 0, F607 = 0, Dcount = 0;

u16 ID607_1[256];
u16 ID607_2[256];
u8 Fchange = 0, te = 0;


void CAN1_DataHandler()
{
	/*u32 ID;
	ID = (CAN_DATA1.IR>>21)&0x7ff;
	te = 0;
	switch(ID)
	{
		case 0x607:
			if((CAN_DATA1.buf.buf[0] == 0x40)&&
					//(CAN_DATA1.buf.buf[1] == 0x90)&&
					(CAN_DATA1.buf.buf[2] == 0x30)&&
					(CAN_DATA1.buf.buf[3] == 0x01))
			{
				CAN_SetHead(&CAN_DATA1, 0x587, 0, 0, 8);
				CAN_DATA1.buf.buf[0] = 0x43; CAN_DATA1.buf.buf[4] = 0x49; CAN_DATA1.buf.buf[5] = 0x5d; CAN_DATA1.buf.buf[6] = 0x00; CAN_DATA1.buf.buf[7] = 0x00;
				CAN_Trans(CAN1, &CAN_DATA1);
				return;
			}
			break;
		case 0x608:
//			F608 = 1;
//			CAN_Trans(CAN2, &CAN_DATA1);
			break;
		case 0x207:
			//CAN_SetHead(&CAN_DATA1, 0x187, 0, 0, 8);
		break;
		case 0x208:
//			CAN_DATA1.buf.buf[0] = 0x0d;
//			CAN_DATA1.buf.buf[1] = 0x0a;
//			CAN_DATA1.buf.buf[2] = 0x01;
//			CAN_DATA1.buf.buf[3] = 0x12;
		break;
		default:
		break;
	}
//	if((F607 == 1)||(F608 == 1))
//	{
//		CAN_DATAfifo[Dcount++] = CAN_DATA1;
//	}
//	else
//	{
//		CAN_Trans(CAN2, &CAN_DATA1);
//	}
//	if(Fchange == 0)
//	{
//		pre++;
//	}
//	else 
//	{
//		after++;
//	}*/
	CAN_Trans(CAN2, &CAN_DATA1);
//	if(te == 1) CAN2_te ++;
}


void CAN2_DataHandler()
{
//	u32 ID;
//	ID = (CAN_DATA2.IR>>21)&0x7ff;
//	switch(ID)
//	{
//		case 0x587:
//			F607 = 0;
//		break;
//		case 0x588:
//			F608 = 0;
//		break;
//		default:
//		break;
//	}
//	CAN_Trans(CAN1, &CAN_DATA2);
}




