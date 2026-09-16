/**
  ****************************(C) COPYRIGHT 2019 DJI****************************
  * @file       can_receive.c/h
  * @brief      there is CAN interrupt function  to receive motor data,
  *             and CAN send function to send motor current to control motor.
  *             这里是CAN中断接收函数，接收电机数据,CAN发送函数发送电机电流控制电机.
  * @note       
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0                       RM              1. done
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2019 DJI****************************
  */

#ifndef CAN_RECEIVE_H
#define CAN_RECEIVE_H
#include "struct_typedef.h"
#include "main.h"

#define OFFLINE_TIME_MAX   25

/* CAN send and receive ID */
typedef enum
{
    CAN_MOTOR_ALL_ID = 0x200,
    CAN_3508_ID      = 0x201,

    CAN_BOARD_ID_1 = 0x101,
    CAN_BOARD_ID_2 = 0x102,
    CAN_BOARD_ID_3 = 0x103,
    CAN_BOARD_ID_4 = 0x104,
	  CAN_BOARD_ID_5 = 0x105,
	
	
	  CAN_BOARD_ID_1_F = 0x301,
    CAN_BOARD_ID_2_F = 0x302,
    CAN_BOARD_ID_3_F = 0x303,
    CAN_BOARD_ID_4_F = 0x304,
	  CAN_BOARD_ID_5_F = 0x305,
} can_msg_id_e;

typedef struct 
{
	uint16_t     B1_offline_cnt;
	uint16_t     B2_offline_cnt;
	uint16_t     B3_offline_cnt;
	uint16_t     B4_offline_cnt;
	uint16_t     B5_offline_cnt;
	uint16_t     offline_cnt_max;
	uint16_t     status;
}Board_Can_info_t;

/*发送给F1的数据*/
typedef struct 
{
	uint8_t    Twinkle_state;     //闪烁状态
	uint8_t    All_Led_Close;     //所有灯灭
	uint8_t    All_Led_Open;      //所有灯开
	uint8_t    Board_Work;        //板子工作
	uint8_t    Set_Color;         //设置颜色
	uint8_t    start_permission;  //开始权限
	uint8_t    RainBow_Flag;      //彩虹标识
	uint8_t    count_t;
}Board_Order_info_t;






void CAN1_Init(void);
void get_Centrol_measure(Board_Order_info_t *ptr);       
/* Extern  ------------------------------------------------------------------*/
extern Board_Can_info_t Board_Can_info;
extern uint8_t can1_rx_data[8];
extern Board_Order_info_t Board_Order_info[5];
#endif

