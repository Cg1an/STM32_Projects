#ifndef  _CAN_SEND_H
#define  _CAN_SEND_H

#include "main.h"
#include "CAN_receive.h"

typedef struct {
	CAN_TxHeaderTypeDef header;
	uint8_t				data[8];
} CAN_TxFrameTypeDef;

/*发送给F4的数据*/
typedef struct
{   
	  uint8_t  	Color;                 //大符颜色
	  uint8_t  	Last_Color;            //上一次大符颜色
	  uint8_t   Hit_LED_state;         //(击中/未击中)LED状态   击中1，未击中0
	  uint8_t   Single_Hit_State;      //单个扇叶（击中/未击中）状态 //击中1，未击中0
	  uint8_t   Single_Working_State;  //单个扇叶待击打状态/
	  uint8_t   All_Hit_State;         //全部扇叶（击中/未击中）状态
	  uint8_t   Spin_State;            //大符（旋转/未旋转）状态
	  uint8_t   Twinkle_Already;       //已经闪烁了
}Big_Fu_info_t;

extern Big_Fu_info_t Big_Fu_info[5];

uint8_t Board_Tx(uint32_t std, uint8_t *data,char can,uint32_t DL);




#endif



