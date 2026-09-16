#ifndef __CAN_H
#define __CAN_H	 
#include "sys.h"	    

	
//CAN1接收RX0中断使能						 							 				    
void CAN1_Mode_Init();//CAN初始化
 
void Set_motor_urrent(signed short int i1, signed short int i2, signed short int i3, signed short int i4);//发送数据

float  read_spend(void);  //速度
signed short int  read_angle(void);   //角度



//u8 CAN1_Send_Msg(u8* msg,u8 len);						//发送数据

//u8 CAN1_Receive_Msg(u8 *buf);							//接收数据

void Motor_ReadData(void);  //读全部数据
#endif





