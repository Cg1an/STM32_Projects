#include "main.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "can.h"
float out_incr[ALL_PID_INCR] = {0};
#define PI_mu 0.01745f
#define YAW_MECHANICAL_CENTRE   0.0020980835*57.3

#define ON  1
#define OFF 0
#define BLUE  1
#define RED 0
#define COLOR (RED)


CHASSIS_MODE_SET Chassis_Mode_Set;

DATA_pack DATA_pack_recevie;
extern  DATA_pack_imu DATA_pack_imu_recive;
extern Data_From_C Data_C;


extern PID M3508_1,M3508_2,M3508_3,M3508_4;
extern PID DM10010L_Follow_Ang,DM10010L_Follow_Speed;
extern motor_t motor[num];
extern dji_motor motor1,motor2,motor3,motor4;
extern PID DM10010L_Ang;

CHASSIS_Task chassis_task;
float test_10010;
float real_yaw;
float chen=0;


extern float usTms; //毫秒计时
extern float ms_2500; //毫秒计时


float test_speed;

uint8_t color=0;

extern float ms_2500;
uint8_t rand_num_0;
uint8_t rand_num_1;
uint8_t rand_num_2;
uint8_t rand_num_3;
uint8_t rand_num_4;

Board_Order_info_t Board_Order_info[5];
Big_Fu_info_t Big_Fu_info[5];

float cnt;
void Data_train(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  for(;;)
  {
//       DATA_pack_recevie.DATA=Data_C.QB;
//		   DATA_Set();
//		   test_10010+=PID_Cal_position(&DM10010L_Ang,DATA_pack_imu_recive.YAW,Data_C.DM_YAW);//跟随云台速度环
//		   real_yaw=test_10010/57.3;
//		if(chen==1){ motor[Motor3].ctrl.pos_set=real_yaw;}
//		   motor[Motor3].ctrl.pos_set=test_10010/57.3;
		  /* 大符速度设置 */	
	
				//颜色设置
				Board_Order_info[0].Set_Color = color;
				Board_Order_info[1].Set_Color = color;
				Board_Order_info[2].Set_Color = color;
				Board_Order_info[3].Set_Color = color;
				Board_Order_info[4].Set_Color = color;

//////////////////////////////////////////////////////////////		
		    Board_Order_info[0].Board_Work  = ON;
				cnt++;
			if(cnt>20000)cnt=500;
			if(cnt<500)
				{
//						if(Big_Fu_info[rand_num_0].Single_Hit_State == OFF)//1号未激活
//						{
//							if()					
//								{
										Board_Order_info[0].Board_Work = OFF;
										Board_Order_info[1].Board_Work = OFF;
										Board_Order_info[2].Board_Work = OFF;
										Board_Order_info[3].Board_Work = OFF;
										Board_Order_info[4].Board_Work = OFF;

										rand_num();
//								}		
				}			
					
//				if(Big_Fu_info[rand_num_0].Single_Hit_State == OFF)
//						{
//								if((ms_2500 >= 2400 && Big_Fu_info[rand_num_0].Single_Hit_State == OFF)
//									|| (Big_Fu_info[rand_num_1].Single_Hit_State
//									||  Big_Fu_info[rand_num_2].Single_Hit_State
//									||	Big_Fu_info[rand_num_3].Single_Hit_State
//									||	Big_Fu_info[rand_num_4].Single_Hit_State))
//								{
//										Board_Order_info[0].Board_Work = OFF;
//										Board_Order_info[1].Board_Work = OFF;
//										Board_Order_info[2].Board_Work = OFF;
//										Board_Order_info[3].Board_Work = OFF;
//										Board_Order_info[4].Board_Work = OFF;

//										rand_num();
//								}		
//						}		
				//////////////////////////////////////
//		if(Big_Fu_info[rand_num_0].Single_Hit_State == ON)
//				{				
//					Board_Order_info[rand_num_1].Board_Work  = ON;
//					cnt=2;
//	   	  }
//				if(Big_Fu_info[rand_num_1].Single_Hit_State == ON)
//				{
//					Board_Order_info[rand_num_2].Board_Work  = ON;	cnt=2;
//				}
//				if(Big_Fu_info[rand_num_2].Single_Hit_State == ON)
//				{
//					Board_Order_info[rand_num_3].Board_Work  = ON;	cnt=2;
//				}
//				if(Big_Fu_info[rand_num_3].Single_Hit_State == ON)
//				{
//					Board_Order_info[rand_num_4].Board_Work  = ON;	cnt=2;
//				}

		    if(Big_Fu_info[0].Single_Hit_State == ON)
				{				
					Board_Order_info[1].Board_Work  = ON;
	   	  }
				if(Big_Fu_info[1].Single_Hit_State == ON)
				{
					Board_Order_info[2].Board_Work  = ON;	
				}
				if(Big_Fu_info[2].Single_Hit_State == ON)
				{
					Board_Order_info[3].Board_Work  = ON;
				}
				if(Big_Fu_info[3].Single_Hit_State == ON)
				{
					Board_Order_info[4].Board_Work  = ON;
				}			
	if(Big_Fu_info[rand_num_0].Single_Hit_State
	   &&Big_Fu_info[rand_num_1].Single_Hit_State
     &&Big_Fu_info[rand_num_2].Single_Hit_State
     &&Big_Fu_info[rand_num_3].Single_Hit_State
     &&Big_Fu_info[rand_num_4].Single_Hit_State)
  {cnt=0;                         
		Big_Fu_info[rand_num_0].Single_Hit_State=
		Big_Fu_info[rand_num_1].Single_Hit_State=
		Big_Fu_info[rand_num_2].Single_Hit_State=
		Big_Fu_info[rand_num_3].Single_Hit_State=
		Big_Fu_info[rand_num_4].Single_Hit_State=0;}

	Board_Info_Tx();
  osDelay(20);
	}
  /* USER CODE END StartDefaultTask */
}
uint8_t Can_Board_Order_data[8];
void Board_Info_Tx(void)
{
	static uint16_t i = 0;
	i++;
	if(i%5 == 0)
	{
		memcpy(Can_Board_Order_data, &Board_Order_info[0], 8);
		Board_Tx(CAN_BOARD_ID_1_F,Can_Board_Order_data,1,0x08);
	}
	if(i%5 == 1)
	{
		memcpy(Can_Board_Order_data, &Board_Order_info[1], 8);
		Board_Tx(CAN_BOARD_ID_2_F,Can_Board_Order_data,1,0x08);
	}
	if(i%5 == 2)
	{
		memcpy(Can_Board_Order_data, &Board_Order_info[2], 8);
		Board_Tx(CAN_BOARD_ID_3_F,Can_Board_Order_data,1,0x08);
	}
	if(i%5 == 3)
	{
		memcpy(Can_Board_Order_data, &Board_Order_info[3], 8);
		Board_Tx(CAN_BOARD_ID_4_F,Can_Board_Order_data,1,0x08);
	}
	if(i%5 == 4)
	{
		memcpy(Can_Board_Order_data, &Board_Order_info[4], 8);
		Board_Tx(CAN_BOARD_ID_5_F,Can_Board_Order_data,1,0x08);
	}
	if(i>=30000)
	{
		i = 0;
	}
}

void rand_num(void)
{
	rand_num_0 = rand()%5;
	rand_num_1 = rand()%5;
	while(rand_num_1 == rand_num_0)
	{
		rand_num_1 = rand()%5;
	}
	rand_num_2 = rand()%5;
	while(rand_num_2 == rand_num_0 || rand_num_2 == rand_num_1)
	{
		rand_num_2 = rand()%5;
	}
	rand_num_3 = rand()%5;
	while(rand_num_3 == rand_num_0 || rand_num_3 == rand_num_1 || rand_num_3 == rand_num_2)
	{
		rand_num_3 = rand()%5;
	}
		rand_num_4 = rand()%5;
	while(rand_num_4 == rand_num_0 || rand_num_4 == rand_num_1 || rand_num_4 == rand_num_2 || rand_num_4 == rand_num_3 )
	{
		rand_num_4 = rand()%5;
	}
}


void DATA_Set(void)
{
 if(DATA_pack_recevie.rc.sr==1)Chassis_Mode_Set=CHASSIS_MODE_FOLLOW;
 if(DATA_pack_recevie.rc.sr==3)Chassis_Mode_Set=CHASSIS_MODE_NO_FOLLOW;
 if(DATA_pack_recevie.rc.sr==2)Chassis_Mode_Set=CHASSIS_MODE_STOP;
}

//大符计算函数=======================
static double calculate_spd(double t)
{
	double a = 0.8;
	double b = 1.29;
	double omega = 1.9;
	return a * sin(omega * t) + b;
}
//====================================

uint8_t Control_flag1 = 0,Switch_flag = 0;
uint32_t last_Key_time1 = 0,Key_time1 = 0,Key_time2 = 0;
double time = 0;
void Chassis_control(void)
{
	if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2) == 1 && Control_flag1 == 0)					  //按下
  {
		Control_flag1 = 1;					
		if(HAL_GetTick() - last_Key_time1 > 5)						
		{						
			Key_time1 = HAL_GetTick();   												  //按下时间
		}          						
	}
	else if(HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_11) == 0 && Control_flag1 == 1)			      //松开
	{
		if(HAL_GetTick() - last_Key_time1 > 5)						
		{						
			Key_time2 = HAL_GetTick();													  //松开时间
			if(Key_time2 - Key_time1 > 5)												  //消抖
			{
				last_Key_time1 = HAL_GetTick();											  //按下后松手的时间/上一次按下的时间
				Switch_flag = 1 - Switch_flag;			  //按下标志
				Control_flag1 = 0;	
			}
		}
	}
	if(Switch_flag == 0)	out_incr[0]=PID_Cal_position(&M3508_1,motor1.re_speed,810);
	else	
	{
		float set_speed = (float)calculate_spd(time);
		out_incr[0]=PID_Cal_position(&M3508_1,motor1.re_speed,set_speed);
	}
	Set_Shoot_Motor_Current((int16_t)out_incr[0], 0,0, 0);
}

void chassis_Mode(void)//确定输入量
{
	switch(Chassis_Mode_Set)
	{
	case CHASSIS_MODE_FOLLOW:	       chassis_follow_gimbal();   break;
  case CHASSIS_MODE_STOP:          chassis_no_move(); break;
	case CHASSIS_MODE_NO_FOLLOW:     chassis_no_follow_gimbal();break;
	}
}

//chassis_task.vx_set=Data_C.VY; 
//chassis_task.vy_set=Data_C.VX;
//chassis_task.wz=Data_C.Turn*20;//角度
//float ecd_angle_difference;

float partern;

void chassis_no_move(void)//底盘失能
{
		chassis_task.vx_set=0;
		chassis_task.vy_set=0;
		chassis_task.wz_set=0;
}

void chassis_no_follow_gimbal(void)//底盘不跟随
{
  	float vx_set,vy_set,wz_set;
	chassis_task.vx_set=Data_C.VX; 
	chassis_task.vy_set=Data_C.VY;
	chassis_task.wz_set=Data_C.Turn*20;
	
//	chassis_task.vx_set=vx_set*cosf(DATA_pack_imu_recive.YAW*PI_mu)+vy_set*sinf(DATA_pack_imu_recive.YAW*PI_mu);//左右
//	chassis_task.vy_set=vy_set*cosf(DATA_pack_imu_recive.YAW*PI_mu)-vx_set*sinf(DATA_pack_imu_recive.YAW*PI_mu);//前进
}
//float ecd_angle_difference=DATA_pack_imu_recive.YAW;	
float watch_yaw;
void chassis_follow_gimbal(void)//底盘跟随
{
	float vx_set,vy_set,wz_set;
	chassis_task.vx_set=Data_C.VX; 
	chassis_task.vy_set=Data_C.VY;
	chassis_task.wz_set=Data_C.Turn*20;
//	ecd_angle_difference=motor[3].para.pos*57.3 - chassis_task.follow_angle_set;	
//	chassis_task.follow_angle_set=0;
	
//	watch_yaw=DATA_pack_imu_recive.YAW*PI_mu;

//	chassis_task.vx_set=vx_set*cosf(DATA_pack_imu_recive.YAW*PI_mu)+vy_set*sinf(DATA_pack_imu_recive.YAW*PI_mu);//左右
//	chassis_task.vy_set=vy_set*cosf(DATA_pack_imu_recive.YAW*PI_mu)-vx_set*sinf(DATA_pack_imu_recive.YAW*PI_mu);//前进

//	chassis_task.wz_current=(motor1.re_speed + motor2.re_speed + motor3.re_speed + motor4.re_speed)/4.0f;
//	
//	chassis_task.wz_set=PID_Cal_position(&DM10010L_Follow_Ang,motor[2].para.pos*57.3,chassis_task.follow_angle_set);//跟随云台速度环
//	chassis_task.wz_set=PID_Cal_position(&DM10010L_Follow_Speed,chassis_task.wz_current,-chassis_task.wz_set);//跟随云台速度环
}

void chassis_QX_wheel(void)
{
	chassis_task.A =  -chassis_task.vx_set + chassis_task.vy_set+chassis_task.wz_set;//
	chassis_task.B =  chassis_task.vx_set + chassis_task.vy_set+chassis_task.wz_set;//
	chassis_task.C =  chassis_task.vx_set - chassis_task.vy_set+chassis_task.wz_set;//   
  chassis_task.D =	-chassis_task.vx_set - chassis_task.vy_set+chassis_task.wz_set;// 
	
	chassis_task.chassis_motor_set[0]=chassis_task.A;
	chassis_task.chassis_motor_set[1]=chassis_task.B;
	chassis_task.chassis_motor_set[2]=chassis_task.C;
	chassis_task.chassis_motor_set[3]=chassis_task.D;
}

void chassis_speed_loop(void)
{
	if(DATA_pack_recevie.rc.rc_flag==1){out_incr[0]=out_incr[1]=out_incr[2]=out_incr[3]=0;}
	else
	{
	out_incr[0]=chassis_task.value_3508_ID[0]=PID_Cal_position(&M3508_1,motor1.re_speed,chassis_task.chassis_motor_set[0]);
	out_incr[1]=chassis_task.value_3508_ID[1]=PID_Cal_position(&M3508_2,motor2.re_speed,chassis_task.chassis_motor_set[1]);
	out_incr[2]=chassis_task.value_3508_ID[2]=PID_Cal_position(&M3508_3,motor3.re_speed,chassis_task.chassis_motor_set[2]);
	out_incr[3]=chassis_task.value_3508_ID[3]=PID_Cal_position(&M3508_4,motor4.re_speed,chassis_task.chassis_motor_set[3]);//re_speed机械转子值
  }
}
void send_to_chassis_3508(void)
{			
  Set_Shoot_Motor_Current((int16_t)out_incr[0], (int16_t)out_incr[1], (int16_t)out_incr[2], (int16_t)out_incr[3]);
}
void Qibeng_Turn(void)
{

 if(DATA_pack_recevie.rc.Qibeng==1)//气泵开关，遥控器拨轮控制，上拨一下开，下拨一下关
	  {
		HAL_GPIO_WritePin(GPIOH,GPIO_PIN_2,GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOH,GPIO_PIN_4,GPIO_PIN_RESET);
	  }
	  else if(DATA_pack_recevie.rc.Qibeng==0)
	  {
		HAL_GPIO_WritePin(GPIOH,GPIO_PIN_4,GPIO_PIN_SET);	 
		HAL_GPIO_WritePin(GPIOH,GPIO_PIN_2,GPIO_PIN_RESET);
	  }
}


float a_x_1000;
float a_x_1;
float w_x_1000;
float w_x_1;
float b;
float sin_value,motor_speed;
float speed_rate = 810;
float  set_mode_bufu(void)
{
	 rand_num();
	a_x_1000 = rand()%265 + 780;
//	a_x_1 = a_x_1000/1000.0f;
  a_x_1 =0.9;
	
	w_x_1000 = rand()%116 + 1884;
	//w_x_1 = w_x_1000/1000.0f;
	w_x_1=1.9;
	b = 2.09f - a_x_1;	

  sin_value = sin(w_x_1*usTms/1000.0f);
	motor_speed = (a_x_1 * sin_value + b)*speed_rate;

return motor_speed;
}
