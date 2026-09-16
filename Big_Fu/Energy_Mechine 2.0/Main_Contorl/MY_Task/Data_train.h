#ifndef __DATA_TRAIN_H__
#define __DATA_TRAIN_H__
#include "stm32f4xx.h"                  // Device header
#include "main.h"
typedef enum{
	CHASSIS_MODE_FOLLOW=0,
	CHASSIS_MODE_STOP,//底盘轮子全给零
	CHASSIS_MODE_NO_FOLLOW,
}CHASSIS_MODE_SET;

typedef struct{

	float chassis_motor_real[4];	
  float chassis_motor_set[4];
	float A,B,C,D;
	
	float value_3508_ID[4];//PID计算后的值

	float vx;//前后
	float vy;//左右
	float wz;//旋转(角速度rad/s)
	
	float vx_set;   //车体三轴速度的设定
	float vy_set;
  float wz_set;	
	float dr_set; 
	
	float yaw_target;
  float follow_angle_set;
	float follow_angle_set_last;
	float wz_current;
	float angle_L;

  int spin_flag;
	int circle_count;
	int remainder;
	float spin_speed_set;
	
	double power_k1;
	double power_k2;
	int G_K;
	int accelerate_flag;
  }
  CHASSIS_Task;

	
typedef union{
uint8_t   DATA;
struct{
uint8_t    sr     :3;
uint8_t    sl     :3;
uint8_t	  Qibeng  :1;
uint8_t	  rc_flag :1;
}rc;
}DATA_pack;






#define BOARD_CAN hcan1
#define MOTOR_CAN hcan2

#define OFFLINE_TIME_MAX   25



/* CAN send and receive ID */
typedef enum
{
    CAN_MOTOR_ALL_ID = 0x200,
    CAN_3508_ID      = 0x203,
	  CAN_SHOOT        = 0x207,


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
	uint8_t     offline_cnt_max;
	uint8_t     offline_cnt;
	uint8_t     status;
}motor_info_t;

typedef struct 
{
	uint8_t     B1_offline_cnt;
	uint8_t     B2_offline_cnt;
	uint8_t     B3_offline_cnt;
	uint8_t     B4_offline_cnt;
	uint8_t     B5_offline_cnt;
	uint8_t     offline_cnt_max;
	uint8_t     status;
}Board_info_t;

/*发送给F1的数据*/
typedef struct 
{
	uint8_t    Twinkle_state;
	uint8_t    All_Led_Close;
	uint8_t    All_Led_Open;
	uint8_t    Board_Work;
	uint8_t    Set_Color;
	uint8_t    start_permission;
	uint8_t    RainBow_Flag;
	
}Board_Order_info_t;         //发送给F1

/*发送给F4的数据*/
typedef struct
{   
	  uint8_t  	Color;                 //大符颜色
	  uint8_t  	Last_Color;            //大符颜色
	  uint8_t   Hit_LED_state;         //(击中/未击中)LED状态
	  uint8_t   Single_Hit_State;      //单个扇叶（击中/未击中）状态
	  uint8_t   Single_Working_State;  //单个扇叶待击打状态
	  uint8_t   All_Hit_State;         //全部扇叶（击中/未击中）状态
	  uint8_t   Spin_State;           //大符（旋转/未旋转）状态
	  uint8_t   Twinkle_Already;
}Big_Fu_info_t;


typedef struct {
	CAN_TxHeaderTypeDef header;
	uint8_t				data[8];
} CAN_TxFrameTypeDef;











	void DATA_Set(void);
void chassis_no_follow_gimbal(void);
void chassis_follow_gimbal(void);
void chassis_no_move(void);
void chassis_Mode(void);
void Chassis_control(void);
	
	void	chassis_QX_wheel(void);
	void chassis_speed_loop(void);
	void send_to_chassis_3508(void);
  void Qibeng_Turn(void);
	
float  set_mode_bufu(void);
void rand_num(void);

void Board_Info_Tx(void);
#define FR 3
#define FL 2
#define BL 1
#define BR 0





#endif

