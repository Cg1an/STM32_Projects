#include "main.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "can.h"
#include "arm_math.h"

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


extern PID M3508_1, M3508_2, M3508_3, M3508_4;
extern PID DM10010L_Follow_Ang, DM10010L_Follow_Speed;
extern motor_t motor[num];
extern dji_motor motor1, motor2, motor3, motor4;
extern PID DM10010L_Ang;

CHASSIS_Task chassis_task;
float test_10010;
float real_yaw;
float chen = 0;


extern float usTms; //毫秒计时
extern float ms_2500; //毫秒计时


float test_speed;

uint8_t color = 0;

extern float ms_2500;
uint8_t rand_num_0;
uint8_t rand_num_1;
uint8_t rand_num_2;
uint8_t rand_num_3;
uint8_t rand_num_4;

Board_Order_info_t Board_Order_info[5];
Big_Fu_info_t Big_Fu_info[5];

float cnt;
uint8_t Control_flag1 = 0, Switch_flag = 0;
uint32_t last_Key_time1 = 0, Key_time1 = 0, Key_time2 = 0;
uint64_t time1 = 0;
void Data_train(void const *argument)
{
    /* USER CODE BEGIN StartDefaultTask */
    uint8_t wjz = 0, wcis = 0;

    for (;;)
    {
        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == 1 && Control_flag1 == 0)					 //按下
        {
            Control_flag1 = 1;

            if (HAL_GetTick() - last_Key_time1 > 5)
            {
                Key_time1 = HAL_GetTick();   												  //按下时间
            }
        }
        else if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == 0 && Control_flag1 == 1)			    //松开
        {
            if (HAL_GetTick() - last_Key_time1 > 5)
            {
                Key_time2 = HAL_GetTick();													  //松开时间

                if (Key_time2 - Key_time1 > 5)												 //消抖
                {
                    last_Key_time1 = HAL_GetTick();											  //按下后松手的时间/上一次按下的时间
                    Switch_flag = 1 - Switch_flag;			  //按下标志
                    Control_flag1 = 0;
                }
            }
        }

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
        //				Board_Order_info[0].count_t  = 0;
        //				Board_Order_info[1].count_t  = 0;
        //				Board_Order_info[2].count_t  = 0;
        //				Board_Order_info[3].count_t  = 0;
        //				Board_Order_info[4].count_t  = 0;
        //				Board_Info_Tx();
        //
        //				osDelay(30);
        //				if(Board_Order_info[0].Board_Work == OFF)
        //				{
        //				Board_Order_info[0].count_t  = 0;
        //				Board_Order_info[1].count_t  = 0;
        //				Board_Order_info[2].count_t  = 0;
        //				Board_Order_info[3].count_t  = 0;
        //				Board_Order_info[4].count_t  = 0;
        //				Board_Info_Tx();
        //
        //				osDelay(30);
        //				}

        //////////////////////////////////////////////////////////////
        if (Switch_flag == 0)
        {
            wjz = 0;
            Board_Order_info[0].count_t  = 0;
            Board_Order_info[1].count_t  = 0;
            Board_Order_info[2].count_t  = 0;
            Board_Order_info[3].count_t  = 0;
            Board_Order_info[4].count_t  = 0;
            Board_Info_Tx();
            osDelay(30);
            Board_Order_info[0].Board_Work  = ON;
            cnt++;

            if (cnt > 20000)
            {
                cnt = 500;
            }

            if (cnt < 500)
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

            if (Big_Fu_info[0].Single_Hit_State == ON)
            {
                Board_Order_info[1].Board_Work  = ON;
            }

            if (Big_Fu_info[1].Single_Hit_State == ON)
            {
                Board_Order_info[2].Board_Work  = ON;
            }

            if (Big_Fu_info[2].Single_Hit_State == ON)
            {
                Board_Order_info[3].Board_Work  = ON;
            }

            if (Big_Fu_info[3].Single_Hit_State == ON)
            {
                Board_Order_info[4].Board_Work  = ON;
            }

            if (Big_Fu_info[rand_num_0].Single_Hit_State
                    && Big_Fu_info[rand_num_1].Single_Hit_State
                    && Big_Fu_info[rand_num_2].Single_Hit_State
                    && Big_Fu_info[rand_num_3].Single_Hit_State
                    && Big_Fu_info[rand_num_4].Single_Hit_State)
            {
                cnt = 0;
                Big_Fu_info[rand_num_0].Single_Hit_State =
                    Big_Fu_info[rand_num_1].Single_Hit_State =
                        Big_Fu_info[rand_num_2].Single_Hit_State =
                            Big_Fu_info[rand_num_3].Single_Hit_State =
                                Big_Fu_info[rand_num_4].Single_Hit_State = 0;
                Board_Order_info[0].Board_Work  = OFF;
                Board_Order_info[1].Board_Work  = OFF;
                Board_Order_info[2].Board_Work  = OFF;
                Board_Order_info[3].Board_Work  = OFF;
                Board_Order_info[4].Board_Work  = OFF;
            }
        }
        //			else
        //			{
        //				cnt++;
        //				if(cnt>20000)cnt=500;
        //				if(cnt<500)
        //				{
        //					Board_Order_info[0].Board_Work = OFF;
        //					Board_Order_info[1].Board_Work = OFF;
        //					Board_Order_info[2].Board_Work = OFF;
        //					Board_Order_info[3].Board_Work = OFF;
        //					Board_Order_info[4].Board_Work = OFF;
        //					rand_num();
        //				}
        //				if(wjz == 0)
        //				{
        //					Big_Fu_info[0].Single_Hit_State=
        //					Big_Fu_info[1].Single_Hit_State=
        //					Big_Fu_info[2].Single_Hit_State=
        //					Big_Fu_info[3].Single_Hit_State=
        //					Big_Fu_info[4].Single_Hit_State=0;
        //
        //					Board_Order_info[0].count_t=
        //					Board_Order_info[1].count_t=
        //					Board_Order_info[2].count_t=
        //					Board_Order_info[3].count_t=
        //					Board_Order_info[4].count_t=0;
        //
        //					Board_Order_info[0].Board_Work = ON;
        //					Board_Order_info[1].Board_Work = ON;
        //					Board_Order_info[2].Board_Work = OFF;
        //					Board_Order_info[3].Board_Work = OFF;
        //					Board_Order_info[4].Board_Work = OFF;
        //
        //					wjz = 1;
        //				}
        //				if(Big_Fu_info[0].Single_Hit_State == ON && Big_Fu_info[1].Single_Hit_State == ON)
        //				{
        //					Big_Fu_info[0].Single_Hit_State=0;
        //					Big_Fu_info[1].Single_Hit_State=0;
        //
        //					Board_Order_info[0].Board_Work = OFF;
        //					Board_Order_info[1].Board_Work = OFF;
        //					Board_Order_info[2].Board_Work = OFF;
        //					Board_Order_info[3].Board_Work = OFF;
        //					Board_Order_info[4].Board_Work = OFF;
        //
        //					Board_Info_Tx();
        //					osDelay(30);
        //
        //					Board_Order_info[2].Board_Work = ON;
        //					Board_Order_info[0].count_t = 1;
        //					Board_Order_info[3].Board_Work = ON;
        //					Board_Order_info[1].count_t = 1;
        //				}
        //				if(Big_Fu_info[2].Single_Hit_State == ON && Big_Fu_info[3].Single_Hit_State == ON)
        //				{
        //					Big_Fu_info[2].Single_Hit_State=0;
        //					Big_Fu_info[3].Single_Hit_State=0;
        //
        //					Board_Order_info[0].Board_Work = OFF;
        //					Board_Order_info[1].Board_Work = OFF;
        //					Board_Order_info[2].Board_Work = OFF;
        //					Board_Order_info[3].Board_Work = OFF;
        //					Board_Order_info[4].Board_Work = OFF;
        //
        //					Board_Info_Tx();
        //					osDelay(30);
        //
        //					Board_Order_info[4].Board_Work = ON;
        //					Board_Order_info[2].count_t = 1;
        //					Board_Order_info[0].Board_Work = ON;
        //					Board_Order_info[3].count_t = 1;
        //				}
        //				if(Big_Fu_info[4].Single_Hit_State == ON && Big_Fu_info[0].Single_Hit_State == ON)
        //				{
        //					Big_Fu_info[4].Single_Hit_State=0;
        //
        //					Board_Order_info[0].Board_Work = OFF;
        //					Board_Order_info[1].Board_Work = OFF;
        //					Board_Order_info[2].Board_Work = OFF;
        //					Board_Order_info[3].Board_Work = OFF;
        //					Board_Order_info[4].Board_Work = OFF;
        //
        //					Board_Info_Tx();
        //					osDelay(30);
        //
        //					Board_Order_info[2].Board_Work = ON;
        //					Board_Order_info[4].count_t = 1;
        //					Board_Order_info[1].Board_Work = ON;
        //					Board_Order_info[0].count_t = 2;
        //				}
        //				if(Big_Fu_info[2].Single_Hit_State == ON && Big_Fu_info[1].Single_Hit_State == ON)
        //				{
        //					Board_Order_info[0].Board_Work = OFF;
        //					Board_Order_info[1].Board_Work = OFF;
        //					Board_Order_info[2].Board_Work = OFF;
        //					Board_Order_info[3].Board_Work = OFF;
        //					Board_Order_info[4].Board_Work = OFF;
        //
        //					Board_Info_Tx();
        //					osDelay(30);
        //
        //					Board_Order_info[4].Board_Work = ON;
        //					Board_Order_info[2].count_t = 2;
        //					Board_Order_info[3].Board_Work = ON;
        //					Board_Order_info[1].count_t = 2;
        //				}
        //				if(Big_Fu_info[3].Single_Hit_State == ON && Big_Fu_info[4].Single_Hit_State == ON)
        //				{
        //					Board_Order_info[0].Board_Work = OFF;
        //					Board_Order_info[1].Board_Work = OFF;
        //					Board_Order_info[2].Board_Work = OFF;
        //					Board_Order_info[3].Board_Work = OFF;
        //					Board_Order_info[4].Board_Work = OFF;
        //
        //					Board_Info_Tx();
        //					osDelay(30);
        //
        //					Board_Order_info[3].count_t = 2;
        //					Board_Order_info[4].count_t = 2;
        //				}
        //
        //	if(Board_Order_info[0].count_t == 2
        //	   &&Board_Order_info[1].count_t == 2
        //     &&Board_Order_info[2].count_t == 2
        //     &&Board_Order_info[3].count_t == 2
        //     &&Board_Order_info[4].count_t == 2)
        //  {
        //		switch(wcis)
        //		{
        //			case 0:
        //			{
        //				Board_Order_info[0].Board_Work = OFF;
        //				Board_Order_info[1].Board_Work = OFF;
        //				Board_Order_info[2].Board_Work = OFF;
        //				Board_Order_info[3].Board_Work = OFF;
        //				Board_Order_info[4].Board_Work = OFF;
        //
        //				wcis ++;
        //			}break;
        //			case 1:
        //			{
        //				osDelay(300);
        //
        //				Board_Order_info[0].Board_Work = ON;
        //				Board_Order_info[1].Board_Work = ON;
        //				Board_Order_info[2].Board_Work = ON;
        //				Board_Order_info[3].Board_Work = ON;
        //				Board_Order_info[4].Board_Work = ON;
        //
        //				wcis ++;
        //			}break;
        //			case 2:
        //			{
        //				osDelay(300);
        //
        //				Board_Order_info[0].Board_Work = OFF;
        //				Board_Order_info[1].Board_Work = OFF;
        //				Board_Order_info[2].Board_Work = OFF;
        //				Board_Order_info[3].Board_Work = OFF;
        //				Board_Order_info[4].Board_Work = OFF;
        //
        //				wcis ++;
        //			}break;
        //			case 3:
        //			{
        //				osDelay(300);
        //
        //				Board_Order_info[0].Board_Work = ON;
        //				Board_Order_info[1].Board_Work = ON;
        //				Board_Order_info[2].Board_Work = ON;
        //				Board_Order_info[3].Board_Work = ON;
        //				Board_Order_info[4].Board_Work = ON;
        //
        //				wcis ++;
        //			}break;
        //			case 4:
        //			{
        //				osDelay(300);
        //
        //				Board_Order_info[0].Board_Work = OFF;
        //				Board_Order_info[1].Board_Work = OFF;
        //				Board_Order_info[2].Board_Work = OFF;
        //				Board_Order_info[3].Board_Work = OFF;
        //				Board_Order_info[4].Board_Work = OFF;
        //
        //				wcis ++;
        //			}break;
        //			case 5:
        //			{
        //				osDelay(300);
        //
        //				Board_Order_info[0].Board_Work = ON;
        //				Board_Order_info[1].Board_Work = ON;
        //				Board_Order_info[2].Board_Work = ON;
        //				Board_Order_info[3].Board_Work = ON;
        //				Board_Order_info[4].Board_Work = ON;
        //
        //				wcis ++;
        //			}break;
        //			case 6:
        //			{
        //				osDelay(300);
        //
        //				Big_Fu_info[rand_num_0].Single_Hit_State=
        //				Big_Fu_info[rand_num_1].Single_Hit_State=
        //				Big_Fu_info[rand_num_2].Single_Hit_State=
        //				Big_Fu_info[rand_num_3].Single_Hit_State=
        //				Big_Fu_info[rand_num_4].Single_Hit_State=0;
        //
        //				Board_Order_info[0].count_t = 0;
        //				Board_Order_info[1].count_t = 0;
        //				Board_Order_info[2].count_t = 0;
        //				Board_Order_info[3].count_t = 0;
        //				Board_Order_info[4].count_t = 0;
        //
        //				Board_Order_info[0].Board_Work = OFF;
        //				Board_Order_info[1].Board_Work = OFF;
        //				Board_Order_info[2].Board_Work = OFF;
        //				Board_Order_info[3].Board_Work = OFF;
        //				Board_Order_info[4].Board_Work = OFF;
        //
        //				wcis = 0;
        //				wjz = 0;
        //			}break;
        //
        //			}
        //		}
        //	}
        /********************************************************************/
        else
        {
            cnt++;

            if (cnt > 20000)
            {
                cnt = 500;
            }

            if (cnt < 500)
            {
                Board_Order_info[0].Board_Work = OFF;
                Board_Order_info[1].Board_Work = OFF;
                Board_Order_info[2].Board_Work = OFF;
                Board_Order_info[3].Board_Work = OFF;
                Board_Order_info[4].Board_Work = OFF;
                rand_num();
            }

            // --- 0. 开局初始化 ---
            if (wjz == 0)
            {
                Big_Fu_info[0].Single_Hit_State =
                    Big_Fu_info[1].Single_Hit_State =
                        Big_Fu_info[2].Single_Hit_State =
                            Big_Fu_info[3].Single_Hit_State =
                                Big_Fu_info[4].Single_Hit_State = 0;
                Board_Order_info[0].count_t =
                    Board_Order_info[1].count_t =
                        Board_Order_info[2].count_t =
                            Board_Order_info[3].count_t =
                                Board_Order_info[4].count_t = 0;
                Board_Order_info[0].Board_Work = ON;
                Board_Order_info[1].Board_Work = ON;
                Board_Order_info[2].Board_Work = OFF;
                Board_Order_info[3].Board_Work = OFF;
                Board_Order_info[4].Board_Work = OFF;
                wjz = 1;
            }

            // --- 1. 第一组击破 (0,1) -> 进度 1/5 ---
            if (Big_Fu_info[0].Single_Hit_State == ON && Big_Fu_info[1].Single_Hit_State == ON)
            {
                Big_Fu_info[0].Single_Hit_State = 0;
                Big_Fu_info[1].Single_Hit_State = 0;
                Board_Order_info[0].Board_Work = OFF;
                Board_Order_info[1].Board_Work = OFF;
                Board_Order_info[2].Board_Work = OFF;
                Board_Order_info[3].Board_Work = OFF;
                Board_Order_info[4].Board_Work = OFF;
                Board_Info_Tx();
                osDelay(30);
                Board_Order_info[2].Board_Work = ON;
                Board_Order_info[3].Board_Work = ON;
                // 全局进度更新为 1
                Board_Order_info[0].count_t = 1;
                Board_Order_info[1].count_t = 1;
                Board_Order_info[2].count_t = 1;
                Board_Order_info[3].count_t = 1;
                Board_Order_info[4].count_t = 1;
                Board_Info_Tx();
                osDelay(30);
            }

            // --- 2. 第二组击破 (2,3) -> 进度 2/5 ---
            if (Big_Fu_info[2].Single_Hit_State == ON && Big_Fu_info[3].Single_Hit_State == ON)
            {
                Big_Fu_info[2].Single_Hit_State = 0;
                Big_Fu_info[3].Single_Hit_State = 0;
                Board_Order_info[0].count_t = 2;
                Board_Order_info[1].count_t = 2;
                Board_Order_info[2].count_t = 2;
                Board_Order_info[3].count_t = 2;
                Board_Order_info[4].count_t = 2;
                Board_Order_info[0].Board_Work = OFF;
                Board_Order_info[1].Board_Work = OFF;
                Board_Order_info[2].Board_Work = OFF;
                Board_Order_info[3].Board_Work = OFF;
                Board_Order_info[4].Board_Work = OFF;
                Board_Info_Tx();
                osDelay(30);
                Board_Order_info[4].Board_Work = ON;
                Board_Order_info[0].Board_Work = ON;
                // 全局进度更新为 2
                //					Board_Order_info[0].count_t = 2;
                //					Board_Order_info[1].count_t = 2;
                //					Board_Order_info[2].count_t = 2;
                //					Board_Order_info[3].count_t = 2;
                //					Board_Order_info[4].count_t = 2;
                //Board_Info_Tx();
                osDelay(30);
            }

            // --- 3. 第三组击破 (4,0) -> 进度 3/5 ---
            if (Big_Fu_info[4].Single_Hit_State == ON && Big_Fu_info[0].Single_Hit_State == ON)
            {
                Big_Fu_info[4].Single_Hit_State = 0;
                // 注意：原来代码这里只清了4号，稳妥起见我们把0号也清一下，避免残留
                Big_Fu_info[0].Single_Hit_State = 0;
                Board_Order_info[0].Board_Work = OFF;
                Board_Order_info[1].Board_Work = OFF;
                Board_Order_info[2].Board_Work = OFF;
                Board_Order_info[3].Board_Work = OFF;
                Board_Order_info[4].Board_Work = OFF;
                Board_Info_Tx();
                osDelay(30);
                Board_Order_info[2].Board_Work = ON;
                Board_Order_info[1].Board_Work = ON;
                // 全局进度更新为 3
                Board_Order_info[0].count_t = 3;
                Board_Order_info[1].count_t = 3;
                Board_Order_info[2].count_t = 3;
                Board_Order_info[3].count_t = 3;
                Board_Order_info[4].count_t = 3;
                Board_Info_Tx();
                osDelay(30);
            }

            // --- 4. 第四组击破 (2,1) -> 进度 4/5 ---
            if (Big_Fu_info[2].Single_Hit_State == ON && Big_Fu_info[1].Single_Hit_State == ON)
            {
                Big_Fu_info[2].Single_Hit_State = 0;
                Big_Fu_info[1].Single_Hit_State = 0;
                Board_Order_info[0].Board_Work = OFF;
                Board_Order_info[1].Board_Work = OFF;
                Board_Order_info[2].Board_Work = OFF;
                Board_Order_info[3].Board_Work = OFF;
                Board_Order_info[4].Board_Work = OFF;
                Board_Info_Tx();
                osDelay(30);
                Board_Order_info[4].Board_Work = ON;
                Board_Order_info[3].Board_Work = ON;
                // 全局进度更新为 4
                Board_Order_info[0].count_t = 4;
                Board_Order_info[1].count_t = 4;
                Board_Order_info[2].count_t = 4;
                Board_Order_info[3].count_t = 4;
                Board_Order_info[4].count_t = 4;
                Board_Info_Tx();
                osDelay(30);
            }

            // --- 5. 第五组击破 (3,4) -> 进度 5/5 (通关) ---
            if (Big_Fu_info[3].Single_Hit_State == ON && Big_Fu_info[4].Single_Hit_State == ON)
            {
                Big_Fu_info[3].Single_Hit_State = 0;
                Big_Fu_info[4].Single_Hit_State = 0;
                Board_Order_info[0].Board_Work = OFF;
                Board_Order_info[1].Board_Work = OFF;
                Board_Order_info[2].Board_Work = OFF;
                Board_Order_info[3].Board_Work = OFF;
                Board_Order_info[4].Board_Work = OFF;
                Board_Info_Tx();
                osDelay(30);
                // 全局进度更新为 5
                Board_Order_info[0].count_t = 5;
                Board_Order_info[1].count_t = 5;
                Board_Order_info[2].count_t = 5;
                Board_Order_info[3].count_t = 5;
                Board_Order_info[4].count_t = 5;
                Board_Info_Tx();
                osDelay(30);
            }

            // --- 6. 检测大满贯 (检测 count_t 是否等于 5) ---
            if (Board_Order_info[0].count_t == 5
                    && Board_Order_info[1].count_t == 5
                    && Board_Order_info[2].count_t == 5
                    && Board_Order_info[3].count_t == 5
                    && Board_Order_info[4].count_t == 5)
            {
                Board_Order_info[0].Board_Work = OFF;
                Board_Order_info[1].Board_Work = OFF;
                Board_Order_info[2].Board_Work = OFF;
                Board_Order_info[3].Board_Work = OFF;
                Board_Order_info[4].Board_Work = OFF;
								osDelay(500);
								Board_Order_info[0].count_t = 6;
                Board_Order_info[1].count_t = 6;
                Board_Order_info[2].count_t = 6;
                Board_Order_info[3].count_t = 6;
                Board_Order_info[4].count_t = 6;
                Board_Info_Tx();
								
								
                //					switch(wcis)
                //					{
                //						case 0:
                //						{
                //							Board_Order_info[0].Board_Work = OFF;
                //							Board_Order_info[1].Board_Work = OFF;
                //							Board_Order_info[2].Board_Work = OFF;
                //							Board_Order_info[3].Board_Work = OFF;
                //							Board_Order_info[4].Board_Work = OFF;
                //							wcis ++;
                //						}break;
                //						case 1:
                //						{
                //							osDelay(300);
                //							Board_Order_info[0].Board_Work = ON;
                //							Board_Order_info[1].Board_Work = ON;
                //							Board_Order_info[2].Board_Work = ON;
                //							Board_Order_info[3].Board_Work = ON;
                //							Board_Order_info[4].Board_Work = ON;
                //							wcis ++;
                //						}break;
                //						case 2:
                //						{
                //							osDelay(300);
                //							Board_Order_info[0].Board_Work = OFF;
                //							Board_Order_info[1].Board_Work = OFF;
                //							Board_Order_info[2].Board_Work = OFF;
                //							Board_Order_info[3].Board_Work = OFF;
                //							Board_Order_info[4].Board_Work = OFF;
                //							wcis ++;
                //						}break;
                //						case 3:
                //						{
                //							osDelay(300);
                //							Board_Order_info[0].Board_Work = ON;
                //							Board_Order_info[1].Board_Work = ON;
                //							Board_Order_info[2].Board_Work = ON;
                //							Board_Order_info[3].Board_Work = ON;
                //							Board_Order_info[4].Board_Work = ON;
                //							wcis ++;
                //						}break;
                //						case 4:
                //						{
                //							osDelay(300);
                //							Board_Order_info[0].Board_Work = OFF;
                //							Board_Order_info[1].Board_Work = OFF;
                //							Board_Order_info[2].Board_Work = OFF;
                //							Board_Order_info[3].Board_Work = OFF;
                //							Board_Order_info[4].Board_Work = OFF;
                //							wcis ++;
                //						}break;
                //						case 5:
                //						{
                //							osDelay(300);
                //							Board_Order_info[0].Board_Work = ON;
                //							Board_Order_info[1].Board_Work = ON;
                //							Board_Order_info[2].Board_Work = ON;
                //							Board_Order_info[3].Board_Work = ON;
                //							Board_Order_info[4].Board_Work = ON;
                //							wcis ++;
                //						}break;
                //						case 6:
                //						{
                //							osDelay(300);
                //							Big_Fu_info[rand_num_0].Single_Hit_State=
                //							Big_Fu_info[rand_num_1].Single_Hit_State=
                //							Big_Fu_info[rand_num_2].Single_Hit_State=
                //							Big_Fu_info[rand_num_3].Single_Hit_State=
                //							Big_Fu_info[rand_num_4].Single_Hit_State=0;
                //							Board_Order_info[0].count_t = 0;
                //							Board_Order_info[1].count_t = 0;
                //							Board_Order_info[2].count_t = 0;
                //							Board_Order_info[3].count_t = 0;
                //							Board_Order_info[4].count_t = 0;
                //							Board_Order_info[0].Board_Work = OFF;
                //							Board_Order_info[1].Board_Work = OFF;
                //							Board_Order_info[2].Board_Work = OFF;
                //							Board_Order_info[3].Board_Work = OFF;
                //							Board_Order_info[4].Board_Work = OFF;
                //							wcis = 0;
                //							wjz = 0;
                //						}break;
                //					}
            }
        }

        /********************************************************************/
        //        else // Switch_flag == 1：大能量机关模式 (RM2026 官方规则)
        //        {
        //            static uint8_t stage = 0;          // 记录当前成功激活的组数 (0~4，代表第1组到第5组)
        //            static uint8_t hit_state = 0;      // 状态机：0=等首发命中(限时2.5s)，1=等第二发命中(限时1s)
        //            static uint32_t timer_tick = 0;    // 计时器 (依托于外层循环的 osDelay(30)，1 tick ≈ 30ms)
        //            static uint8_t target_A = 0;       // 当前亮起的靶子 A
        //            static uint8_t target_B = 0;       // 当前亮起的靶子 B
        //            static uint8_t hit_which = 0;      // 记录哪个靶子先被击中 (1代表A，2代表B)
        //            cnt++;
        //            if (cnt > 20000)
        //            {
        //                cnt = 500;
        //            }
        //            if (cnt < 500)
        //            {
        //                Board_Order_info[0].Board_Work = OFF;
        //                Board_Order_info[1].Board_Work = OFF;
        //                Board_Order_info[2].Board_Work = OFF;
        //                Board_Order_info[3].Board_Work = OFF;
        //                Board_Order_info[4].Board_Work = OFF;
        //                rand_num();
        //            }
        //            // --- 1. 开局 / 失败重置 的初始化逻辑 ---
        //            if (wjz == 0)
        //            {
        //                // 清空所有状态
        //                for (int i = 0; i < 5; i++)
        //                {
        //                    Board_Order_info[i].Board_Work = OFF;
        //                    Board_Order_info[i].count_t = 0;      // 进度指示清零
        //                    Big_Fu_info[i].Single_Hit_State = 0;  // 清除物理击中反馈
        //                }
        //                // 重新生成 0~4 互不重复的随机数
        //                rand_num();
        //                // 抽取前两个作为本组的点亮目标
        //                target_A = rand_num_0;
        //                target_B = rand_num_1;
        //                Board_Order_info[target_A].Board_Work = ON;
        //                Board_Order_info[target_B].Board_Work = ON;
        //                Board_Info_Tx();
        //                stage = 0;
        //                hit_state = 0;
        //                timer_tick = 0;
        //                wcis = 0;
        //                wjz = 1; // 标记初始化完成
        //            }
        //            // --- 2. 核心状态机 (正在激活阶段) ---
        //            if (wjz == 1 && stage < 5)
        //            {
        //                timer_tick++;
        //                // 【状态 0】：等待 2.5 秒内击中任意一个
        //                if (hit_state == 0)
        //                {
        //                    // 2500ms ÷ 30ms ≈ 83.3 ticks
        //                    if (timer_tick > 83)
        //                    {
        //                        wjz = 0; // 超过 2.5 秒没打中任何一个！大符激活失败，触发重置
        //                    }
        //                    else
        //                    {
        //                        // 检测 A 或 B 是否被击中
        //                        if (Big_Fu_info[target_A].Single_Hit_State == ON || Big_Fu_info[target_B].Single_Hit_State == ON)
        //                        {
        //                            if (Big_Fu_info[target_A].Single_Hit_State == ON)
        //                            {
        //                                hit_which = 1; // 记录 A 先被击中
        //                                Big_Fu_info[target_A].Single_Hit_State = 0;
        //                                //Board_Order_info[target_A].Board_Work = OFF; // 改变灯效（这里以熄灭代替，也可按需发送其它颜色指令）
        //                            }
        //                            else
        //                            {
        //                                hit_which = 2; // 记录 B 先被击中
        //                                Big_Fu_info[target_B].Single_Hit_State = 0;
        //                                //Board_Order_info[target_B].Board_Work = OFF; // 改变灯效
        //                            }
        //                            // 只要击中任意一个，立刻进入 1 秒额外窗口期
        //                            hit_state = 1;
        //                            timer_tick = 0; // 计时器清零，开始计时这 1 秒
        //                        }
        //                    }
        //                }
        //                // 【状态 1】：1 秒额外窗口期，不论是否击中第二个，时间一到就换组
        //                else if (hit_state == 1)
        //                {
        //                    // 在这 1 秒内，依然允许检测剩下的那个目标是否被击中
        //                    if (hit_which == 1 && Big_Fu_info[target_B].Single_Hit_State == ON)
        //                    {
        //                        Big_Fu_info[target_B].Single_Hit_State = 0;
        //                        Board_Order_info[target_B].Board_Work = OFF; // 第二个也被击中，改变灯效
        //                    }
        //                    else if (hit_which == 2 && Big_Fu_info[target_A].Single_Hit_State == ON)
        //                    {
        //                        Big_Fu_info[target_A].Single_Hit_State = 0;
        //                        Board_Order_info[target_A].Board_Work = OFF; // 第二个也被击中，改变灯效
        //                    }
        //                    // 1000ms ÷ 30ms ≈ 33.3 ticks
        //                    // 1 秒时间一到，不论第二发有没有打中，都算成功激活本组，并开始激活下一组
        //                    if (timer_tick > 33)
        //                    {
        //                        stage++; // 当前已激活组数 +1
        //                        // 【规则映射】：更新中部灯臂的进度指示 (1/5, 2/5...)
        //                        // 利用已有的 count_t 变量将进度发给所有灯板的单片机，由灯板单片机负责控制流水灯长度
        //                        for (int i = 0; i < 5; i++)
        //                        {
        //                            Board_Order_info[i].count_t = stage;
        //                            Board_Order_info[i].Board_Work = OFF; // 先把上一组没打完的残余灯全关掉
        //                        }
        //                        if (stage < 5)
        //                        {
        //                            // 还没全部通关，重新随机抽取 2 个新目标
        //                            rand_num();
        //                            target_A = rand_num_0;
        //                            target_B = rand_num_1;
        //                            Board_Order_info[target_A].Board_Work = ON;
        //                            Board_Order_info[target_B].Board_Work = ON;
        //                            Board_Info_Tx();
        //                            hit_state = 0;  // 状态回到等首发命中
        //                            timer_tick = 0; // 计时器重新清零
        //                        }
        //                    }
        //                }
        //            }
        //            // --- 3. 成功激活 5 组，触发大满贯胜利爆闪动画 ---
        //            if (Board_Order_info[0].count_t == 2
        //                    && Board_Order_info[1].count_t == 2
        //                    && Board_Order_info[2].count_t == 2
        //                    && Board_Order_info[3].count_t == 2
        //                    && Board_Order_info[4].count_t == 2)
        //            {
        //                if (stage == 5)
        //                {
        //                    switch (wcis)
        //                    {
        //                    case 0:
        //                        for (int i = 0; i < 5; i++)
        //                        {
        //                            Board_Order_info[i].Board_Work = OFF;
        //                        }
        //                        wcis++;
        //                        break;
        //                    case 1:
        //                    case 3:
        //                    case 5:
        //                        osDelay(300);
        //                        for (int i = 0; i < 5; i++)
        //                        {
        //                            Board_Order_info[i].Board_Work = ON;
        //                        }
        //                        wcis++;
        //                        break;
        //                    case 2:
        //                    case 4:
        //                    case 6: // 增加一次爆闪保持原节奏
        //                        osDelay(300);
        //                        for (int i = 0; i < 5; i++)
        //                        {
        //                            Board_Order_info[i].Board_Work = OFF;
        //                        }
        //                        wcis++;
        //                        break;
        //                    case 7:
        //                        osDelay(300);
        //                        wjz = 0; // 动画播放完毕，重置 wjz，使得下一轮游戏可以重新开始
        //                        break;
        //                    }
        //                }
        //            }
        //        }
        Board_Info_Tx();
        osDelay(30);
    }

    /* USER CODE END StartDefaultTask */
}
uint8_t Can_Board_Order_data[8];
void Board_Info_Tx(void)
{
    static uint16_t i = 0;
    i++;

    if (i % 5 == 0)
    {
        memcpy(Can_Board_Order_data, &Board_Order_info[0], 8);
        Board_Tx(CAN_BOARD_ID_1_F, Can_Board_Order_data, 1, 0x08);
    }

    if (i % 5 == 1)
    {
        memcpy(Can_Board_Order_data, &Board_Order_info[1], 8);
        Board_Tx(CAN_BOARD_ID_2_F, Can_Board_Order_data, 1, 0x08);
    }

    if (i % 5 == 2)
    {
        memcpy(Can_Board_Order_data, &Board_Order_info[2], 8);
        Board_Tx(CAN_BOARD_ID_3_F, Can_Board_Order_data, 1, 0x08);
    }

    if (i % 5 == 3)
    {
        memcpy(Can_Board_Order_data, &Board_Order_info[3], 8);
        Board_Tx(CAN_BOARD_ID_4_F, Can_Board_Order_data, 1, 0x08);
    }

    if (i % 5 == 4)
    {
        memcpy(Can_Board_Order_data, &Board_Order_info[4], 8);
        Board_Tx(CAN_BOARD_ID_5_F, Can_Board_Order_data, 1, 0x08);
    }

    if (i >= 30000)
    {
        i = 0;
    }
}

void rand_num(void)
{
    rand_num_0 = rand() % 5;
    rand_num_1 = rand() % 5;

    while (rand_num_1 == rand_num_0)
    {
        rand_num_1 = rand() % 5;
    }

    rand_num_2 = rand() % 5;

    while (rand_num_2 == rand_num_0 || rand_num_2 == rand_num_1)
    {
        rand_num_2 = rand() % 5;
    }

    rand_num_3 = rand() % 5;

    while (rand_num_3 == rand_num_0 || rand_num_3 == rand_num_1 || rand_num_3 == rand_num_2)
    {
        rand_num_3 = rand() % 5;
    }

    rand_num_4 = rand() % 5;

    while (rand_num_4 == rand_num_0 || rand_num_4 == rand_num_1 || rand_num_4 == rand_num_2 || rand_num_4 == rand_num_3)
    {
        rand_num_4 = rand() % 5;
    }
}


void DATA_Set(void)
{
    if (DATA_pack_recevie.rc.sr == 1)
    {
        Chassis_Mode_Set = CHASSIS_MODE_FOLLOW;
    }

    if (DATA_pack_recevie.rc.sr == 3)
    {
        Chassis_Mode_Set = CHASSIS_MODE_NO_FOLLOW;
    }

    if (DATA_pack_recevie.rc.sr == 2)
    {
        Chassis_Mode_Set = CHASSIS_MODE_STOP;
    }
}


//大符计算函数=======================
static float calculate_spd(uint64_t t)
{
    float a = 1.0f;
    float b = 1.09f;
    float omega = 1.95f;
    return (a * arm_sin_f32(omega * t) + b);
}
//====================================

float time = 0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM1)
    {
        time ++;
    }
}
//====================================




void Chassis_control(void)
{
    time = time + 0.001f;
    time1 = (uint64_t)time;

    if (Switch_flag == 0)
    {
        out_incr[0] = PID_Cal_position(&M3508_1, motor1.re_speed, 810, 100.0f);
    }
    else
    {
        float set_speed = (calculate_spd(time1) * 773.55f);
        out_incr[0] = PID_Cal_position(&M3508_1, motor1.re_speed, set_speed, 100.0f);
    }

    Set_Shoot_Motor_Current((int16_t)out_incr[0], 0, 0, 0);
}

void chassis_Mode(void)//确定输入量
{
    switch (Chassis_Mode_Set)
    {
    case CHASSIS_MODE_FOLLOW:
        chassis_follow_gimbal();
        break;

    case CHASSIS_MODE_STOP:
        chassis_no_move();
        break;

    case CHASSIS_MODE_NO_FOLLOW:
        chassis_no_follow_gimbal();
        break;
    }
}

//chassis_task.vx_set=Data_C.VY;
//chassis_task.vy_set=Data_C.VX;
//chassis_task.wz=Data_C.Turn*20;//角度
//float ecd_angle_difference;

float partern;

void chassis_no_move(void)//底盘失能
{
    chassis_task.vx_set = 0;
    chassis_task.vy_set = 0;
    chassis_task.wz_set = 0;
}

void chassis_no_follow_gimbal(void)//底盘不跟随
{
    float vx_set, vy_set, wz_set;
    chassis_task.vx_set = Data_C.VX;
    chassis_task.vy_set = Data_C.VY;
    chassis_task.wz_set = Data_C.Turn * 20;
    //	chassis_task.vx_set=vx_set*cosf(DATA_pack_imu_recive.YAW*PI_mu)+vy_set*sinf(DATA_pack_imu_recive.YAW*PI_mu);//左右
    //	chassis_task.vy_set=vy_set*cosf(DATA_pack_imu_recive.YAW*PI_mu)-vx_set*sinf(DATA_pack_imu_recive.YAW*PI_mu);//前进
}
//float ecd_angle_difference=DATA_pack_imu_recive.YAW;
float watch_yaw;
void chassis_follow_gimbal(void)//底盘跟随
{
    float vx_set, vy_set, wz_set;
    chassis_task.vx_set = Data_C.VX;
    chassis_task.vy_set = Data_C.VY;
    chassis_task.wz_set = Data_C.Turn * 20;
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
    chassis_task.A =  -chassis_task.vx_set + chassis_task.vy_set + chassis_task.wz_set; //
    chassis_task.B =  chassis_task.vx_set + chassis_task.vy_set + chassis_task.wz_set; //
    chassis_task.C =  chassis_task.vx_set - chassis_task.vy_set + chassis_task.wz_set; //
    chassis_task.D =	-chassis_task.vx_set - chassis_task.vy_set + chassis_task.wz_set; //
    chassis_task.chassis_motor_set[0] = chassis_task.A;
    chassis_task.chassis_motor_set[1] = chassis_task.B;
    chassis_task.chassis_motor_set[2] = chassis_task.C;
    chassis_task.chassis_motor_set[3] = chassis_task.D;
}

void chassis_speed_loop(void)
{
    if (DATA_pack_recevie.rc.rc_flag == 1)
    {
        out_incr[0] = out_incr[1] = out_incr[2] = out_incr[3] = 0;
    }
    else
    {
        //out_incr[0]=chassis_task.value_3508_ID[0]=PID_Cal_position(&M3508_1,motor1.re_speed,chassis_task.chassis_motor_set[0]);
        //out_incr[1]=chassis_task.value_3508_ID[1]=PID_Cal_position(&M3508_2,motor2.re_speed,chassis_task.chassis_motor_set[1]);
        //out_incr[2]=chassis_task.value_3508_ID[2]=PID_Cal_position(&M3508_3,motor3.re_speed,chassis_task.chassis_motor_set[2]);
        //out_incr[3]=chassis_task.value_3508_ID[3]=PID_Cal_position(&M3508_4,motor4.re_speed,chassis_task.chassis_motor_set[3]);//re_speed机械转子值
    }
}
void send_to_chassis_3508(void)
{
    Set_Shoot_Motor_Current((int16_t)out_incr[0], (int16_t)out_incr[1], (int16_t)out_incr[2], (int16_t)out_incr[3]);
}
void Qibeng_Turn(void)
{
    if (DATA_pack_recevie.rc.Qibeng == 1) //气泵开关，遥控器拨轮控制，上拨一下开，下拨一下关
    {
        HAL_GPIO_WritePin(GPIOH, GPIO_PIN_2, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOH, GPIO_PIN_4, GPIO_PIN_RESET);
    }
    else if (DATA_pack_recevie.rc.Qibeng == 0)
    {
        HAL_GPIO_WritePin(GPIOH, GPIO_PIN_4, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOH, GPIO_PIN_2, GPIO_PIN_RESET);
    }
}


float a_x_1000;
float a_x_1;
float w_x_1000;
float w_x_1;
float b;
float sin_value, motor_speed;
float speed_rate = 810;
float  set_mode_bufu(void)
{
    rand_num();
    a_x_1000 = rand() % 265 + 780;
    //	a_x_1 = a_x_1000/1000.0f;
    a_x_1 = 0.9;
    w_x_1000 = rand() % 116 + 1884;
    //w_x_1 = w_x_1000/1000.0f;
    w_x_1 = 1.9;
    b = 2.09f - a_x_1;
    sin_value = sin(w_x_1 * usTms / 1000.0f);
    motor_speed = (a_x_1 * sin_value + b) * speed_rate;
    return motor_speed;
}
