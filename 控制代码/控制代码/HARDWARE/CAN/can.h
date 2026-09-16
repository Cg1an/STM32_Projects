#ifndef __CAN_H
#define __CAN_H	 
#include "sys.h"	  
//#include "PWM.h"
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
typedef union {
	u8  buf[8];
	u32 DR[2];
}databuf;

typedef struct {
	u32 IR;
	u32 DTR;
	databuf buf;
	u8 mbox;
}CAN_DATA;
										 							 				    
u8 CAN1_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);//CAN初始化
u8 CAN2_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);//CAN初始化
void InitControl(void);
void CAN_SetHead(CAN_DATA * data, u32 id,u8 ide,u8 rtr,u8 len);//
void CAN_SetDATA(CAN_DATA * CAN, u8 * data);
u8 CAN_Trans(CAN_TypeDef * CAN, CAN_DATA * data);//
u8 CAN_Tx_Staus(CAN_TypeDef * CAN, u8 mbox);//查询发送状态
void CAN_Res(u8 fifox, CAN_TypeDef * CAN, CAN_DATA * data);
u8 CANx_SendStdData(CAN_TypeDef * CANx, uint32_t std_id, uint8_t *pData);
//extern CAN_DATA CAN_DATA2, CAN_DATA207, CAN_DATA207_0, CAN_DATA208, CAN_DATA308, CAN_DATA607, CAN_DATA608;
extern CAN_DATA CAN_DATA1;
extern CAN_DATA CAN_AngleFL, CAN_AngleFR, CAN_AngleBL, CAN_AngleBR;
extern CAN_DATA CAN_VelFL, CAN_VelFR, CAN_VelBL, CAN_VelBR;
extern CAN_DATA CAN_Motor1, CAN_Motor2;

extern u8 CAN_Data[8];
extern float angle_temp, vel_temp;

extern u16 Motor_Enabled;

#endif

















