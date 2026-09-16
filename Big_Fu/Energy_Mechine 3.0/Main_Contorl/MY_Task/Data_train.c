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

typedef enum
{
    ENERGY_IDLE = 0,   // 初始状态
    ENERGY_BOARD0,
    ENERGY_BOARD1,
    ENERGY_BOARD2,
    ENERGY_BOARD3,
    ENERGY_BOARD4,
    ENERGY_FINISH
}Energy_State_t;

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
//	static Energy_State_t energy_state = ENERGY_IDLE;
//static uint32_t lit_start_time = 0;
	#define TIMEOUT_TICKS 2500
        static int current_lit_index = -1;
        static uint32_t lit_start_time = 0;
#define TIMEOUT_TICKS 2500   // 2.5秒

#define TIMEOUT_TICKS 2500
    //    // ===== 【新增】：按键状态机与模式切换状态变量 =====
    //    static uint8_t btn_last_state = 0;
    //    static uint32_t btn_debounce_time = 0;
    //    static uint8_t last_Switch_flag = 0xFF; // 用于检测是否刚刚发生了模式切换

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

        // ===== 1. 修复的按键逻辑：稳定可靠的边缘触发 =====
        //        uint8_t btn_current_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2);
        //        // 当按键被按下，且上次是松开状态时
        //        if (btn_current_state == 1 && btn_last_state == 0)
        //        {
        //            if (HAL_GetTick() - btn_debounce_time > 50) // 50ms 消抖
        //            {
        //                Switch_flag = 1 - Switch_flag; // 切换标志位 0和1互换
        //                btn_debounce_time = HAL_GetTick();
        //            }
        //        }
        //        btn_last_state = btn_current_state;
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
        // ===== 2. 核心修复：模式切换瞬间的“数据大清洗” =====
        // 只有当检测到模式发生变化的那一刻，才会执行一次清洗
        //        if (Switch_flag != last_Switch_flag)
        //        {
        //            last_Switch_flag = Switch_flag; // 更新记录
        //            wjz = 0; // 重置大能量机关的关卡进度
        //            cnt = 0; // 重置小能量机关的计数器
        //            // 【关键】：彻底清空所有的受击状态、进度条和灯效，杜绝"幽灵状态"带入下一个模式！
        //            for (int i = 0; i < 5; i++)
        //            {
        //                Big_Fu_info[i].Single_Hit_State = 0;
        //                Board_Order_info[i].Board_Work = OFF;
        //                Board_Order_info[i].count_t = 0;
        //            }
        //            Board_Info_Tx(); // 立即发送全灭指令给所有灯板
        //            osDelay(30);     // 稍微延时，确保板子完成复位
        //        }
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
       /* 
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
        }*/
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

//if(Switch_flag == 0)
//{
//    Board_Info_Tx();
//    osDelay(30);

//    switch(energy_state)
//    {

//    /*---------------- 初始状态 ----------------*/

//    case ENERGY_IDLE:

//        Board_Order_info[0].Board_Work = ON;
//        Board_Order_info[1].Board_Work = OFF;
//        Board_Order_info[2].Board_Work = OFF;
//        Board_Order_info[3].Board_Work = OFF;
//        Board_Order_info[4].Board_Work = OFF;

//        lit_start_time = osKernelSysTick();

//        energy_state = ENERGY_BOARD0;

//        break;


//    /*---------------- 0号板 ----------------*/

//    case ENERGY_BOARD0:

//        if(Big_Fu_info[0].Single_Hit_State == ON)
//        {
//            Big_Fu_info[0].Single_Hit_State = 0;

//            Board_Order_info[1].Board_Work = ON;

//            lit_start_time = osKernelSysTick();

//            energy_state = ENERGY_BOARD1;
//        }

//        else if(osKernelSysTick() - lit_start_time > TIMEOUT_TICKS)
//        {
//            energy_state = ENERGY_IDLE;
//        }

//        break;


//    /*---------------- 1号板 ----------------*/

//    case ENERGY_BOARD1:

//        if(Big_Fu_info[1].Single_Hit_State == ON)
//        {
//            Big_Fu_info[1].Single_Hit_State = 0;

//            Board_Order_info[2].Board_Work = ON;

//            lit_start_time = osKernelSysTick();

//            energy_state = ENERGY_BOARD2;
//        }

//        else if(osKernelSysTick() - lit_start_time > TIMEOUT_TICKS)
//        {
//            energy_state = ENERGY_IDLE;
//        }

//        break;


//    /*---------------- 2号板 ----------------*/

//    case ENERGY_BOARD2:

//        if(Big_Fu_info[2].Single_Hit_State == ON)
//        {
//            Big_Fu_info[2].Single_Hit_State = 0;

//            Board_Order_info[3].Board_Work = ON;

//            lit_start_time = osKernelSysTick();

//            energy_state = ENERGY_BOARD3;
//        }

//        else if(osKernelSysTick() - lit_start_time > TIMEOUT_TICKS)
//        {
//            energy_state = ENERGY_IDLE;
//        }

//        break;


//    /*---------------- 3号板 ----------------*/

//    case ENERGY_BOARD3:

//        if(Big_Fu_info[3].Single_Hit_State == ON)
//        {
//            Big_Fu_info[3].Single_Hit_State = 0;

//            Board_Order_info[4].Board_Work = ON;

//            lit_start_time = osKernelSysTick();

//            energy_state = ENERGY_BOARD4;
//        }

//        else if(osKernelSysTick() - lit_start_time > TIMEOUT_TICKS)
//        {
//            energy_state = ENERGY_IDLE;
//        }

//        break;


//    /*---------------- 4号板 ----------------*/

//    case ENERGY_BOARD4:

//        if(Big_Fu_info[4].Single_Hit_State == ON)
//        {
//            Big_Fu_info[4].Single_Hit_State = 0;

//            energy_state = ENERGY_FINISH;
//        }

//        else if(osKernelSysTick() - lit_start_time > TIMEOUT_TICKS)
//        {
//            energy_state = ENERGY_IDLE;
//        }

//        break;


//    /*---------------- 全部击中 ----------------*/

//    case ENERGY_FINISH:

//        Board_Order_info[0].Board_Work = OFF;
//        Board_Order_info[1].Board_Work = OFF;
//        Board_Order_info[2].Board_Work = OFF;
//        Board_Order_info[3].Board_Work = OFF;
//        Board_Order_info[4].Board_Work = OFF;

//        energy_state = ENERGY_IDLE;

//        break;
//    }
//}
//static uint32_t lit_start_time = 0;//当前亮起板子的开始时间
//static uint8_t current_board = 0;//当前应该被击中的板子编号
//static uint8_t hit_count = 0;//成功击中了几个板子

//#define TIMEOUT_TICKS 2500


        // 添加全局变量以跟踪当前点亮靶板和开始时间


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
            uint32_t current_time = osKernelSysTick();

            /**************** 超时检测 ****************/
            if (current_lit_index != -1)
            {
                if ((current_time - lit_start_time) > TIMEOUT_TICKS)
                {
                    // 关闭当前点亮的靶板
                    //Board_Order_info[0].Board_Work = OFF;
                    Board_Order_info[1].Board_Work = OFF;
                    Board_Order_info[2].Board_Work = OFF;
                    Board_Order_info[3].Board_Work = OFF;
                    Board_Order_info[4].Board_Work = OFF;
									for (int k = 0; k < 5; k++)
                        {
                            Board_Info_Tx();
                            osDelay(2);
                        }
                    // 回到初始状态：从0号靶重新开始
                    current_lit_index = 0;
                    // 重新点亮0号靶
                    Board_Order_info[0].Board_Work = ON;
                    lit_start_time = current_time;
                    cnt = 500;
                    // 注意：不清除 Single_Hit_State
//												Board_Order_info[1].Single_Hit_State = OFF;
//                    Board_Order_info[2].Single_Hit_State = OFF;
//                    Board_Order_info[3].Single_Hit_State = OFF;
//                    Board_Order_info[4].Single_Hit_State = OFF;
											goto skip_hit_check;
                }
            }

            /**************** 启动延时 ****************/
            cnt++;

            if (cnt > 20000)
            {
                cnt = 500;
            }

            /**************** 初始点亮 ****************/
            if (current_lit_index == -1)
            {
                if (cnt >= 500)
                {
                    current_lit_index = 0;
                    Board_Order_info[0].Board_Work = ON;
                    lit_start_time = current_time;
                }
                else
                {
                    for (int i = 0; i < 5; i++)
                    {
                        Board_Order_info[i].Board_Work = OFF;
                    }

                    rand_num();
                }
            }
            /**************** 击中检测 ****************/
            else
            {
                if (Big_Fu_info[current_lit_index].Single_Hit_State == ON)
                {
                    // 关闭当前靶板
                    //Board_Order_info[current_lit_index].Board_Work = OFF;
									Big_Fu_info[current_lit_index].Single_Hit_State = OFF;
                    int next_index = current_lit_index + 1;

                    if (next_index < 5)
                    {
                        // 点亮下一个靶板
                        current_lit_index = next_index;
                        Board_Order_info[current_lit_index].Board_Work = ON;
                        lit_start_time = current_time;
                    }
                    else
                    {
                        // 五个全部完成
                        current_lit_index = -1;
                        cnt = 0;
                        osDelay(1000);

                        for (int i = 0; i < 5; i++)
                        {
                            Board_Order_info[i].Board_Work = OFF;
                        }

                        // 注意：这里也不清除击中状态
                    }
                }
            }

            /**************** 容错检测 ****************/
						skip_hit_check:
            if (Big_Fu_info[rand_num_0].Single_Hit_State
                    && Big_Fu_info[rand_num_1].Single_Hit_State
                    && Big_Fu_info[rand_num_2].Single_Hit_State
                    && Big_Fu_info[rand_num_3].Single_Hit_State
                    && Big_Fu_info[rand_num_4].Single_Hit_State)
            {
                cnt = 0;

                for (int i = 0; i < 5; i++)
                {
                    Board_Order_info[i].Board_Work = OFF;
                }

                current_lit_index = -1;
            }
        }


//static int current_lit_index = -1;
//        static uint32_t lit_start_time = 0;
//#define TIMEOUT_TICKS 2500   // 2.5秒

//        if (Switch_flag == 0)
//        {
//            wjz = 0;
//            // 使用循环替代重复赋值，更简洁
//            for(int i = 0; i < 5; i++) {
//                Board_Order_info[i].count_t = 0;
//            }
//            
//            Board_Info_Tx();
//            osDelay(30);
//            uint32_t current_time = osKernelSysTick();

//            /**************** 启动延时计数 ****************/
//            // 优化：移到最前面统一处理，防止被中间逻辑打断
//            cnt++;
//            if (cnt > 20000) {
//                cnt = 500;
//            }

//            /**************** 容错检测 (全局结束条件) ****************/
//            // 优先判断是否已经全部满足容错条件
//            if (Big_Fu_info[rand_num_0].Single_Hit_State
//             && Big_Fu_info[rand_num_1].Single_Hit_State
//             && Big_Fu_info[rand_num_2].Single_Hit_State
//             && Big_Fu_info[rand_num_3].Single_Hit_State
//             && Big_Fu_info[rand_num_4].Single_Hit_State)
//            {
//                cnt = 0;
//                current_lit_index = -1;
//                for (int i = 0; i < 5; i++) {
//                    Board_Order_info[i].Board_Work = OFF;
//                    Big_Fu_info[i].Single_Hit_State = OFF; // 【安全补丁】同步清除击中状态
//                }
//                // 已经满足全部条件，直接跳过后面的逻辑
//                continue; // 如果这段代码在while循环中，用continue；如果在独立函数中，用return
//            }

//            /**************** 状态机主逻辑 ****************/
//            if (current_lit_index == -1) 
//            {
//                /* 状态：初始/未启动 */
//                if (cnt >= 500)
//                {
//                    // 准备开始前，先确保所有板子都是干净的关闭状态
//                    for (int i = 0; i < 5; i++) {
//                        Board_Order_info[i].Board_Work = OFF;
//                        Big_Fu_info[i].Single_Hit_State = OFF; // 【安全补丁】
//                    }
//                    
//                    current_lit_index = 0;
//                    Board_Order_info[0].Board_Work = ON;
//                    lit_start_time = current_time;
//                }
//                else
//                {
//                    for (int i = 0; i < 5; i++) {
//                        Board_Order_info[i].Board_Work = OFF;
//                    }
//                    rand_num();
//                }
//            }
//            else 
//            {
//                /* 状态：游戏运行中 (某个靶亮着) */
//                
//                // 1. 优先判断【击中】
//                if (Big_Fu_info[current_lit_index].Single_Hit_State == ON)
//                {
//                    // 【关键优化】：不仅向硬件发送OFF指令，还要在软件上立即清除该击中标志，防止通信延迟带来的误判！
//                    //Board_Order_info[current_lit_index].Board_Work = OFF;
//                    Big_Fu_info[current_lit_index].Single_Hit_State = OFF;
//                    
//                    int next_index = current_lit_index + 1;

//                    if (next_index < 5)
//                    {
//                        // 还有没打完的，点亮下一个
//                        current_lit_index = next_index;
//                        Board_Order_info[current_lit_index].Board_Work = ON;
//                        lit_start_time = current_time;
//                    }
//                    else
//                    {
//                        // 五个全部完成
//                        current_lit_index = -1;
//                        cnt = 0;
//                        osDelay(1000); // 延时显示一下全通状态
//                        
//                        for (int i = 0; i < 5; i++) {
//                            Board_Order_info[i].Board_Work = OFF;
//                            // 全部打完也要重置击中状态
//                            Big_Fu_info[i].Single_Hit_State = OFF;
//                        }
//                    }
//                }
//                // 2. 没击中的话，判断是否【超时】
//                else if ((current_time - lit_start_time) > TIMEOUT_TICKS)
//                {
//                    // 先把所有靶板干脆利落地关掉，并且把之前的击中残存状态全部清空
//                    for(int i = 0; i < 5; i++) {
//                        Board_Order_info[i].Board_Work = OFF;
//                        Big_Fu_info[i].Single_Hit_State = OFF; // 【安全补丁】超时直接全归零
//                    }

//                    // 回到初始状态：从0号靶重新开始
//                    current_lit_index = 0;
//                    Board_Order_info[0].Board_Work = ON;
//                    lit_start_time = current_time;
//                    cnt = 500;
//                }
//            }
//        }

//// ===== 小能量机关模式 (Switch_flag == 0) =====
//        if (Switch_flag == 0)
//        {
//            static uint32_t small_timer_start = 0;
//            // 统一在开头获取一次当前时间即可
//            uint32_t current_time = osKernelSysTick(); 

//            // --- 0. 开局初始化 ---
//            if (wjz == 0)
//            {
//                // 1. 清空所有受击标志
//                Big_Fu_info[0].Single_Hit_State = 0;
//                Big_Fu_info[1].Single_Hit_State = 0;
//                Big_Fu_info[2].Single_Hit_State = 0;
//                Big_Fu_info[3].Single_Hit_State = 0;
//                Big_Fu_info[4].Single_Hit_State = 0;

//                // 2. 进度预留清零
//                Board_Order_info[0].count_t = 0;
//                Board_Order_info[1].count_t = 0;
//                Board_Order_info[2].count_t = 0;
//                Board_Order_info[3].count_t = 0;
//                Board_Order_info[4].count_t = 0;

//                // 3. 初始点亮第 1 个靶子 (指定 0 号)
//                Board_Order_info[0].Board_Work = ON;
//                Board_Order_info[1].Board_Work = OFF;
//                Board_Order_info[2].Board_Work = OFF;
//                Board_Order_info[3].Board_Work = OFF;
//                Board_Order_info[4].Board_Work = OFF;
//                
//                Board_Info_Tx();

//                wjz = 1; // 进入第 1 关
//                small_timer_start = current_time; // 开始 2.5 秒倒计时
//            }

//            // --- 1. 等待击中第 1 个靶子 (0号) ---
//            else if (wjz == 1)
//            {
//                // 【忽略打错】：悄悄清空非目标的受击标志
//                Big_Fu_info[1].Single_Hit_State = 0;
//                Big_Fu_info[2].Single_Hit_State = 0;
//                Big_Fu_info[3].Single_Hit_State = 0;
//                Big_Fu_info[4].Single_Hit_State = 0;

//                // 【超时检测】：2.5 秒内没打中
//                if (current_time - small_timer_start > 2500)
//                {
//                    Board_Order_info[0].Board_Work = ON;
//										Board_Order_info[1].Board_Work = OFF;
//										Board_Order_info[2].Board_Work = OFF;
//										Board_Order_info[3].Board_Work = OFF;
//										Board_Order_info[4].Board_Work = OFF;
//                    Board_Info_Tx();
//                    osDelay(30); 
//                    wjz = 0;       // 重置回开局
//                }
//                // 【命中正确】：成功击中目标 0
//                else if (Big_Fu_info[0].Single_Hit_State == ON)
//                {
//                    Big_Fu_info[0].Single_Hit_State = 0; // 清除本次命中标志
//                    
//                    // TODO: 预留位置 -> 亮起第一节进度条/流水灯效
//                    
//                    Board_Order_info[1].Board_Work = ON; // 点亮下一个靶子(1号)，同时0号保持ON
//                    Board_Info_Tx();
//                    osDelay(30);

//                    wjz = 2; // 推进到第 2 关
//                    small_timer_start = osKernelSysTick(); // 重置 2.5 秒倒计时！
//                }
//            }

//            // --- 2. 等待击中第 2 个靶子 (1号) ---
//            else if (wjz == 2)
//            {
//                Big_Fu_info[0].Single_Hit_State = 0;
//                Big_Fu_info[2].Single_Hit_State = 0;
//                Big_Fu_info[3].Single_Hit_State = 0;
//                Big_Fu_info[4].Single_Hit_State = 0;

//                if (current_time - small_timer_start > 2500)
//                {
//                    Board_Order_info[0].Board_Work = ON;
//										Board_Order_info[1].Board_Work = OFF;
//										Board_Order_info[2].Board_Work = OFF;
//										Board_Order_info[3].Board_Work = OFF;
//										Board_Order_info[4].Board_Work = OFF;
//                    Board_Info_Tx();
//                    osDelay(30); 
//                    wjz = 0;       // 重置回开局
//                }
//                else if (Big_Fu_info[1].Single_Hit_State == ON)
//                {
//                    Big_Fu_info[1].Single_Hit_State = 0;

//                    // TODO: 预留位置 -> 亮起第二节进度条/流水灯效

//                    Board_Order_info[2].Board_Work = ON; // 点亮指定的 2 号，前面的保持亮
//                    Board_Info_Tx(); 
//                    osDelay(30);

//                    wjz = 3;
//                    small_timer_start = osKernelSysTick();
//                }
//            }

//            // --- 3. 等待击中第 3 个靶子 (2号) ---
//            else if (wjz == 3)
//            {
//                Big_Fu_info[0].Single_Hit_State = 0;
//                Big_Fu_info[1].Single_Hit_State = 0;
//                Big_Fu_info[3].Single_Hit_State = 0;
//                Big_Fu_info[4].Single_Hit_State = 0;

//                if (current_time - small_timer_start > 2500)
//                {
//                    Board_Order_info[0].Board_Work = ON;
//										Board_Order_info[1].Board_Work = OFF;
//										Board_Order_info[2].Board_Work = OFF;
//										Board_Order_info[3].Board_Work = OFF;
//										Board_Order_info[4].Board_Work = OFF;
//                    Board_Info_Tx();
//                    osDelay(30); 
//                    wjz = 0;       // 重置回开局
//                }
//                else if (Big_Fu_info[2].Single_Hit_State == ON)
//                {
//                    Big_Fu_info[2].Single_Hit_State = 0;

//                    // TODO: 预留位置 -> 亮起第三节进度条/流水灯效

//                    Board_Order_info[3].Board_Work = ON; // 点亮指定的 3 号
//                    Board_Info_Tx(); 
//                    osDelay(30);

//                    wjz = 4;
//                    small_timer_start = osKernelSysTick();
//                }
//            }

//            // --- 4. 等待击中第 4 个靶子 (3号) ---
//            else if (wjz == 4)
//            {
//                Big_Fu_info[0].Single_Hit_State = 0;
//                Big_Fu_info[1].Single_Hit_State = 0;
//                Big_Fu_info[2].Single_Hit_State = 0;
//                Big_Fu_info[4].Single_Hit_State = 0;

//                if (current_time - small_timer_start > 2500)
//                {
//                    Board_Order_info[0].Board_Work = ON;
//										Board_Order_info[1].Board_Work = OFF;
//										Board_Order_info[2].Board_Work = OFF;
//										Board_Order_info[3].Board_Work = OFF;
//										Board_Order_info[4].Board_Work = OFF;
//                    Board_Info_Tx();
//                    osDelay(30); 
//                    wjz = 0;       // 重置回开局
//                }
//                else if (Big_Fu_info[3].Single_Hit_State == ON)
//                {
//                    Big_Fu_info[3].Single_Hit_State = 0;

//                    // TODO: 预留位置 -> 亮起第四节进度条/流水灯效

//                    Board_Order_info[4].Board_Work = ON; // 点亮指定的 4 号
//                    Board_Info_Tx(); 
//                    osDelay(30);

//                    wjz = 5;
//                    small_timer_start = osKernelSysTick();
//                }
//            }

//            // --- 5. 等待击中第 5 个靶子 (4号) -> 通关 ---
//            else if (wjz == 5)
//            {
//                Big_Fu_info[0].Single_Hit_State = 0;
//                Big_Fu_info[1].Single_Hit_State = 0;
//                Big_Fu_info[2].Single_Hit_State = 0;
//                Big_Fu_info[3].Single_Hit_State = 0;

//                if (current_time - small_timer_start > 2500)
//                {
//                    Board_Order_info[0].Board_Work = ON;
//										Board_Order_info[1].Board_Work = OFF;
//										Board_Order_info[2].Board_Work = OFF;
//										Board_Order_info[3].Board_Work = OFF;
//										Board_Order_info[4].Board_Work = OFF;
//                    Board_Info_Tx();
//                    osDelay(30); 
//                    wjz = 0;       // 重置回开局
//                }
//                else if (Big_Fu_info[4].Single_Hit_State == ON)
//                {
//                    Big_Fu_info[4].Single_Hit_State = 0;

//                    // 通关！小符大满贯
//                    // 注意：通关时通常需要让灯全亮闪烁，或者全灭重新开始
//                    Board_Order_info[0].Board_Work = OFF; 
//                    Board_Order_info[1].Board_Work = OFF; 
//                    Board_Order_info[2].Board_Work = OFF; 
//                    Board_Order_info[3].Board_Work = OFF; 
//                    Board_Order_info[4].Board_Work = OFF;
//                    
//                    // TODO: 预留位置 -> 全亮通关灯效

//                    Board_Info_Tx();
//                    osDelay(30);

//                    wjz = 6; // 转入通关动画
//                }
//            }
//            
//            // --- 6. 通关动画与重置 ---
//            else if (wjz == 6)
//            {
//                osDelay(1000); // 停顿展示1秒
//                wjz = 0;       // 动画结束，彻底重置小能量机关，开启下一轮
//            }
//        }

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

            // ===== 【新增的核心计时与状态变量】 =====
            // 静态变量保证在不同循环周期中不丢失状态
            static uint32_t timer_start = 0;    // 记录计时起点的毫秒时间戳
            static uint8_t stage_sub_state = 0; // 子状态: 0=等待2.5s内首击, 1=等待1s内次击
            static uint8_t first_hit_id = 0xFF; // 记录首先被击中的装甲板ID
            // 获取当前系统毫秒时间 (如果你的工程不使用HAL库，请替换为 osKernelSysTick() 或自定义毫秒级时间函数)
            //            uint32_t current_time = HAL_GetTick();
            uint32_t current_time = osKernelSysTick();

            // --- 0. 开局初始化 ---
            if (wjz == 0)
            {
                Big_Fu_info[0].Single_Hit_State =
                    Big_Fu_info[1].Single_Hit_State =
                        Big_Fu_info[2].Single_Hit_State =
                            Big_Fu_info[3].Single_Hit_State =
                                Big_Fu_info[4].Single_Hit_State = OFF;
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
                Board_Info_Tx(); // 建议初始化时发送一次，确保子板状态同步
                wjz = 1;              // 状态机进入第 1 组
                stage_sub_state = 0;  // 进入等待首击状态
                timer_start = current_time; // 开启第一组的 2.5 秒倒计时
            }
            // --- 1. 第一组击破 (0,1) -> 进度 1/5 ---
            else if (wjz == 1)
            {
//                // 【新增逻辑】：检测是否打错了（打中了没亮的 2、3、4 任意一个）
//                if (Big_Fu_info[2].Single_Hit_State == ON ||
//                        Big_Fu_info[3].Single_Hit_State == ON ||
//                        Big_Fu_info[4].Single_Hit_State == ON)
//                {
//                    wjz = 0;
//                }
//                else
//                {
                    //                    // 1. 熄灭所有灯光，进度归零
                    //                    Board_Order_info[0].Board_Work = ON;
                    //                    Board_Order_info[1].Board_Work = ON;
                    //                    Board_Order_info[2].Board_Work = OFF;
                    //                    Board_Order_info[3].Board_Work = OFF;
                    //                    Board_Order_info[4].Board_Work = OFF;
                    //                    Board_Order_info[0].count_t = 0;
                    //                    Board_Order_info[1].count_t = 0;
                    //                    Board_Order_info[2].count_t = 0;
                    //                    Board_Order_info[3].count_t = 0;
                    //                    Board_Order_info[4].count_t = 0;
                    //                    // 2. 【关键！】清空所有板子的受击标志，防止残留的错误判定带到下一局
                    //                    Big_Fu_info[0].Single_Hit_State = 0;
                    //                    Big_Fu_info[1].Single_Hit_State = 0;
                    //                    Big_Fu_info[2].Single_Hit_State = 0;
                    //                    Big_Fu_info[3].Single_Hit_State = 0;
                    //                    Big_Fu_info[4].Single_Hit_State = 0;
                    //                    // 3. 发送全灭指令，并停顿 1 秒作为“打错惩罚”的视觉展示
                    //                    Board_Info_Tx();
                    //                    osDelay(1000);
                    //                    // 4. 重置状态机，退回开局重新洗牌
                    if (stage_sub_state == 0) // 【等待第一击：2.5秒限制】
                    {
                        if (Big_Fu_info[0].Single_Hit_State == ON || Big_Fu_info[1].Single_Hit_State == ON)
                        {
                            // 记录首先击中的装甲板，并【只清空这一个】标志，以防在同一瞬间另一个也被击中而丢失状态
                            first_hit_id = (Big_Fu_info[0].Single_Hit_State == ON) ? 0 : 1;
                            Big_Fu_info[first_hit_id].Single_Hit_State = 0;
                            // TODO: 在此预留位置 -> 添加首击命中后被点亮的装甲模块特殊灯效、灯臂流动灯效的代码
                            stage_sub_state = 1;        // 切换到等待次击状态
                            timer_start = current_time; // 重置定时器，开启 1 秒倒计时
                        }
                        else if (current_time - timer_start > 2500)
                        {
                            wjz = 0; // 2.5秒超时未击中任何一个，大能量机关重置
                        }
                    }
                    else if (stage_sub_state == 1) // 【等待第二击：1秒限制】
                    {
                        uint8_t target_id = (first_hit_id == 0) ? 1 : 0; // 找出另一个需要击中的装甲板

                        // 如果在1秒内击中了另一个，或者已经到达了1秒超时界限，都算结束本阶段，推进到下一组
                        if (Big_Fu_info[target_id].Single_Hit_State == ON || (current_time - timer_start > 1000))
                        {
                            Big_Fu_info[target_id].Single_Hit_State = 0; // 清空次击标志
                            // 更新全局进度为 1
                            Board_Order_info[0].count_t = 1;
                            Board_Order_info[1].count_t = 1;
                            Board_Order_info[2].count_t = 1;
                            Board_Order_info[3].count_t = 1;
                            Board_Order_info[4].count_t = 1;
                            Board_Order_info[0].Board_Work = OFF;
                            Board_Order_info[1].Board_Work = OFF;
                            Board_Order_info[2].Board_Work = OFF;
                            Board_Order_info[3].Board_Work = OFF;
                            Board_Order_info[4].Board_Work = OFF;
                            // 点亮第二组的两个板
                            Board_Order_info[2].Board_Work = ON;
                            Board_Order_info[3].Board_Work = ON;
                            Board_Info_Tx();
                            osDelay(30);
                            wjz = 2;             // 切换状态到第 2 组
                            stage_sub_state = 0; // 重置为首击状态
                            timer_start = osKernelSysTick(); // 开启第 2 组的 2.5 秒倒计时
                        }
                    }
//                }
            }
            // --- 2. 第二组击破 (2,3) -> 进度 2/5 ---
            else if (wjz == 2)
            {
                // 【新增逻辑】：检测是否打错了（打中了没亮的 2、3、4 任意一个）
//                if (Big_Fu_info[0].Single_Hit_State == ON ||
//                        Big_Fu_info[1].Single_Hit_State == ON ||
//                        Big_Fu_info[4].Single_Hit_State == ON)
//                {
//                    wjz = 0;
//                }
//                else
//                {
                    //                    // 1. 熄灭所有灯光，进度归零
                    //                    Board_Order_info[0].Board_Work = ON;
                    //                    Board_Order_info[1].Board_Work = ON;
                    //                    Board_Order_info[2].Board_Work = OFF;
                    //                    Board_Order_info[3].Board_Work = OFF;
                    //                    Board_Order_info[4].Board_Work = OFF;
                    //                    Board_Order_info[0].count_t = 0;
                    //                    Board_Order_info[1].count_t = 0;
                    //                    Board_Order_info[2].count_t = 0;
                    //                    Board_Order_info[3].count_t = 0;
                    //                    Board_Order_info[4].count_t = 0;
                    //                    // 2. 【关键！】清空所有板子的受击标志，防止残留的错误判定带到下一局
                    //                    Big_Fu_info[0].Single_Hit_State = 0;
                    //                    Big_Fu_info[1].Single_Hit_State = 0;
                    //                    Big_Fu_info[2].Single_Hit_State = 0;
                    //                    Big_Fu_info[3].Single_Hit_State = 0;
                    //                    Big_Fu_info[4].Single_Hit_State = 0;
                    //                    // 3. 发送全灭指令，并停顿 1 秒作为“打错惩罚”的视觉展示
                    //                    Board_Info_Tx();
                    //                    osDelay(1000);
                    //                    // 4. 重置状态机，退回开局重新洗牌
                    if (stage_sub_state == 0)
                    {
                        if (Big_Fu_info[2].Single_Hit_State == ON || Big_Fu_info[3].Single_Hit_State == ON)
                        {
                            first_hit_id = (Big_Fu_info[2].Single_Hit_State == ON) ? 2 : 3;
                            Big_Fu_info[first_hit_id].Single_Hit_State = 0;
                            // TODO: 特殊灯效预留位置
                            stage_sub_state = 1;
                            timer_start = current_time;
                        }
                        else if (current_time - timer_start > 2500)
                        {
                            wjz = 0;
                        }
                    }
                    else if (stage_sub_state == 1)
                    {
                        uint8_t target_id = (first_hit_id == 2) ? 3 : 2;

                        if (Big_Fu_info[target_id].Single_Hit_State == ON || (current_time - timer_start > 1000))
                        {
                            Big_Fu_info[target_id].Single_Hit_State = 0;
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
                            // 点亮第三组 (4,0)
                            Board_Order_info[4].Board_Work = ON;
                            Board_Order_info[0].Board_Work = ON;
                            Board_Info_Tx();
                            osDelay(30);
                            wjz = 3;
                            stage_sub_state = 0;
                            timer_start = osKernelSysTick();
                        }
                    }
//                }
            }
            // --- 3. 第三组击破 (4,0) -> 进度 3/5 ---
            else if (wjz == 3)
            {
                // 【新增逻辑】：检测是否打错了（打中了没亮的 2、3、4 任意一个）
                //                if (Big_Fu_info[2].Single_Hit_State == ON ||
                //                        Big_Fu_info[3].Single_Hit_State == ON ||
                //                        Big_Fu_info[1].Single_Hit_State == ON)
                //                {
                //									                    wjz = 0;
                //                }
                //                    // 1. 熄灭所有灯光，进度归零
                //                    Board_Order_info[0].Board_Work = ON;
                //                    Board_Order_info[1].Board_Work = ON;
                //                    Board_Order_info[2].Board_Work = OFF;
                //                    Board_Order_info[3].Board_Work = OFF;
                //                    Board_Order_info[4].Board_Work = OFF;
                //                    Board_Order_info[0].count_t = 0;
                //                    Board_Order_info[1].count_t = 0;
                //                    Board_Order_info[2].count_t = 0;
                //                    Board_Order_info[3].count_t = 0;
                //                    Board_Order_info[4].count_t = 0;
                //                    // 2. 【关键！】清空所有板子的受击标志，防止残留的错误判定带到下一局
                //                    Big_Fu_info[0].Single_Hit_State = 0;
                //                    Big_Fu_info[1].Single_Hit_State = 0;
                //                    Big_Fu_info[2].Single_Hit_State = 0;
                //                    Big_Fu_info[3].Single_Hit_State = 0;
                //                    Big_Fu_info[4].Single_Hit_State = 0;
                //                    // 3. 发送全灭指令，并停顿 1 秒作为“打错惩罚”的视觉展示
                //                    Board_Info_Tx();
                //                    osDelay(1000);
                //                    // 4. 重置状态机，退回开局重新洗牌
                if (stage_sub_state == 0)
                {
                    if (Big_Fu_info[4].Single_Hit_State == ON || Big_Fu_info[0].Single_Hit_State == ON)
                    {
                        first_hit_id = (Big_Fu_info[4].Single_Hit_State == ON) ? 4 : 0;
                        Big_Fu_info[first_hit_id].Single_Hit_State = 0;
                        // TODO: 特殊灯效预留位置
                        stage_sub_state = 1;
                        timer_start = current_time;
                    }
                    else if (current_time - timer_start > 2500)
                    {
                        wjz = 0;
                    }
                }
                else if (stage_sub_state == 1)
                {
                    uint8_t target_id = (first_hit_id == 4) ? 0 : 4;

                    if (Big_Fu_info[target_id].Single_Hit_State == ON || (current_time - timer_start > 1000))
                    {
                        Big_Fu_info[target_id].Single_Hit_State = 0;
                        Board_Order_info[0].count_t = 3;
                        Board_Order_info[1].count_t = 3;
                        Board_Order_info[2].count_t = 3;
                        Board_Order_info[3].count_t = 3;
                        Board_Order_info[4].count_t = 3;
                        Board_Order_info[0].Board_Work = OFF;
                        Board_Order_info[1].Board_Work = OFF;
                        Board_Order_info[2].Board_Work = OFF;
                        Board_Order_info[3].Board_Work = OFF;
                        Board_Order_info[4].Board_Work = OFF;
                        // 点亮第四组 (2,1)
                        Board_Order_info[2].Board_Work = ON;
                        Board_Order_info[1].Board_Work = ON;
                        Board_Info_Tx();
                        osDelay(30);
                        wjz = 4;
                        stage_sub_state = 0;
                        timer_start = osKernelSysTick();
                    }
                }
            }
            // --- 4. 第四组击破 (2,1) -> 进度 4/5 ---
            else if (wjz == 4)
            {
                // 【新增逻辑】：检测是否打错了（打中了没亮的 2、3、4 任意一个）
                //                if (Big_Fu_info[4].Single_Hit_State == ON ||
                //                        Big_Fu_info[3].Single_Hit_State == ON ||
                //                        Big_Fu_info[0].Single_Hit_State == ON)
                //                {
                //									                    wjz = 0;
                //                }
                //                    // 1. 熄灭所有灯光，进度归零
                //                    Board_Order_info[0].Board_Work = ON;
                //                    Board_Order_info[1].Board_Work = ON;
                //                    Board_Order_info[2].Board_Work = OFF;
                //                    Board_Order_info[3].Board_Work = OFF;
                //                    Board_Order_info[4].Board_Work = OFF;
                //                    Board_Order_info[0].count_t = 0;
                //                    Board_Order_info[1].count_t = 0;
                //                    Board_Order_info[2].count_t = 0;
                //                    Board_Order_info[3].count_t = 0;
                //                    Board_Order_info[4].count_t = 0;
                //                    // 2. 【关键！】清空所有板子的受击标志，防止残留的错误判定带到下一局
                //                    Big_Fu_info[0].Single_Hit_State = 0;
                //                    Big_Fu_info[1].Single_Hit_State = 0;
                //                    Big_Fu_info[2].Single_Hit_State = 0;
                //                    Big_Fu_info[3].Single_Hit_State = 0;
                //                    Big_Fu_info[4].Single_Hit_State = 0;
                //                    // 3. 发送全灭指令，并停顿 1 秒作为“打错惩罚”的视觉展示
                //                    Board_Info_Tx();
                //                    osDelay(1000);
                //                    // 4. 重置状态机，退回开局重新洗牌
                if (stage_sub_state == 0)
                {
                    if (Big_Fu_info[2].Single_Hit_State == ON || Big_Fu_info[1].Single_Hit_State == ON)
                    {
                        first_hit_id = (Big_Fu_info[2].Single_Hit_State == ON) ? 2 : 1;
                        Big_Fu_info[first_hit_id].Single_Hit_State = 0;
                        // TODO: 特殊灯效预留位置
                        stage_sub_state = 1;
                        timer_start = current_time;
                    }
                    else if (current_time - timer_start > 2500)
                    {
                        wjz = 0;
                    }
                }
                else if (stage_sub_state == 1)
                {
                    uint8_t target_id = (first_hit_id == 2) ? 1 : 2;

                    if (Big_Fu_info[target_id].Single_Hit_State == ON || (current_time - timer_start > 1000))
                    {
                        Big_Fu_info[target_id].Single_Hit_State = 0;
                        Board_Order_info[0].count_t = 4;
                        Board_Order_info[1].count_t = 4;
                        Board_Order_info[2].count_t = 4;
                        Board_Order_info[3].count_t = 4;
                        Board_Order_info[4].count_t = 4;
                        Board_Order_info[0].Board_Work = OFF;
                        Board_Order_info[1].Board_Work = OFF;
                        Board_Order_info[2].Board_Work = OFF;
                        Board_Order_info[3].Board_Work = OFF;
                        Board_Order_info[4].Board_Work = OFF;
                        // 点亮第五组 (3,4)
                        Board_Order_info[4].Board_Work = ON;
                        Board_Order_info[3].Board_Work = ON;
                        Board_Info_Tx();
                        osDelay(30);
                        wjz = 5;
                        stage_sub_state = 0;
                        timer_start = osKernelSysTick();
                    }
                }
            }
            // --- 5. 第五组击破 (3,4) -> 进度 5/5 (通关) ---
            else if (wjz == 5)
            {
                // 【新增逻辑】：检测是否打错了（打中了没亮的 2、3、4 任意一个）
                //                if (Big_Fu_info[2].Single_Hit_State == ON ||
                //                        Big_Fu_info[0].Single_Hit_State == ON ||
                //                        Big_Fu_info[1].Single_Hit_State == ON)
                //                {
                //									                    wjz = 0;
                //                }

                //                    // 1. 熄灭所有灯光，进度归零
                //                    Board_Order_info[0].Board_Work = ON;
                //                    Board_Order_info[1].Board_Work = ON;
                //                    Board_Order_info[2].Board_Work = OFF;
                //                    Board_Order_info[3].Board_Work = OFF;
                //                    Board_Order_info[4].Board_Work = OFF;
                //                    Board_Order_info[0].count_t = 0;
                //                    Board_Order_info[1].count_t = 0;
                //                    Board_Order_info[2].count_t = 0;
                //                    Board_Order_info[3].count_t = 0;
                //                    Board_Order_info[4].count_t = 0;
                //                    // 2. 【关键！】清空所有板子的受击标志，防止残留的错误判定带到下一局
                //                    Big_Fu_info[0].Single_Hit_State = 0;
                //                    Big_Fu_info[1].Single_Hit_State = 0;
                //                    Big_Fu_info[2].Single_Hit_State = 0;
                //                    Big_Fu_info[3].Single_Hit_State = 0;
                //                    Big_Fu_info[4].Single_Hit_State = 0;
                //                    // 3. 发送全灭指令，并停顿 1 秒作为“打错惩罚”的视觉展示
                //                    Board_Info_Tx();
                //                    osDelay(1000);
                //                    // 4. 重置状态机，退回开局重新洗牌
                //                    wjz = 0;
                //                }
                if (stage_sub_state == 0)
                {
                    if (Big_Fu_info[3].Single_Hit_State == ON || Big_Fu_info[4].Single_Hit_State == ON)
                    {
                        first_hit_id = (Big_Fu_info[3].Single_Hit_State == ON) ? 3 : 4;
                        Big_Fu_info[first_hit_id].Single_Hit_State = 0;
                        // TODO: 特殊灯效预留位置
                        stage_sub_state = 1;
                        timer_start = current_time;
                    }
                    else if (current_time - timer_start > 2500)
                    {
                        wjz = 0;
                    }
                }
                else if (stage_sub_state == 1)
                {
                    uint8_t target_id = (first_hit_id == 3) ? 4 : 3;

                    if (Big_Fu_info[target_id].Single_Hit_State == ON || (current_time - timer_start > 1000))
                    {
                        Big_Fu_info[target_id].Single_Hit_State = 0;
                        // 全局进度更新为 5 (通关)
                        Board_Order_info[0].count_t = 5;
                        Board_Order_info[1].count_t = 5;
                        Board_Order_info[2].count_t = 5;
                        Board_Order_info[3].count_t = 5;
                        Board_Order_info[4].count_t = 5;
                        Board_Order_info[0].Board_Work = OFF;
                        Board_Order_info[1].Board_Work = OFF;
                        Board_Order_info[2].Board_Work = OFF;
                        Board_Order_info[3].Board_Work = OFF;
                        Board_Order_info[4].Board_Work = OFF;

                        for (int k = 0; k < 5; k++)
                        {
                            Board_Info_Tx();
                            osDelay(2);
                        }

                        osDelay(30);
                        wjz = 6; // 必须推进到一个新的状态，避免大满贯逻辑被立即执行卡死
                    }
                }
            }
            // --- 6. 检测大满贯 (动画展示与重置) ---
            else if (wjz == 6)
            {
                osDelay(1000); // 停顿1秒，完美展示全亮灯效
                Board_Order_info[0].count_t = 6;
                Board_Order_info[1].count_t = 6;
                Board_Order_info[2].count_t = 6;
                Board_Order_info[3].count_t = 6;
                Board_Order_info[4].count_t = 6;

                for (int k = 0; k < 5; k++)
                {
                    Board_Info_Tx();
                    osDelay(2);
                }

                wjz = 0; // 动画结束，彻底重置游戏
                osDelay(30);
            }
        }

        /*
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
        Board_Order_info[0].count_t = 1;
        Board_Order_info[1].count_t = 1;
        Board_Order_info[2].count_t = 1;
        Board_Order_info[3].count_t = 1;
        Board_Order_info[4].count_t = 1;
        Board_Order_info[0].Board_Work = OFF;
        Board_Order_info[1].Board_Work = OFF;
        Board_Order_info[2].Board_Work = OFF;
        Board_Order_info[3].Board_Work = OFF;
        Board_Order_info[4].Board_Work = OFF;
        Board_Order_info[2].Board_Work = ON;
        Board_Order_info[3].Board_Work = ON;
        // 全局进度更新为 1
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
        Board_Order_info[4].Board_Work = ON;
        Board_Order_info[0].Board_Work = ON;
        Board_Info_Tx();
        osDelay(30);

        }

        // --- 3. 第三组击破 (4,0) -> 进度 3/5 ---
        if (Big_Fu_info[4].Single_Hit_State == ON && Big_Fu_info[0].Single_Hit_State == ON)
        {
        Big_Fu_info[4].Single_Hit_State = 0;
        // 注意：原来代码这里只清了4号，稳妥起见我们把0号也清一下，避免残留
        Big_Fu_info[0].Single_Hit_State = 0;
        // 全局进度更新为 3
        Board_Order_info[0].count_t = 3;
        Board_Order_info[1].count_t = 3;
        Board_Order_info[2].count_t = 3;
        Board_Order_info[3].count_t = 3;
        Board_Order_info[4].count_t = 3;
        Board_Order_info[0].Board_Work = OFF;
        Board_Order_info[1].Board_Work = OFF;
        Board_Order_info[2].Board_Work = OFF;
        Board_Order_info[3].Board_Work = OFF;
        Board_Order_info[4].Board_Work = OFF;
        Board_Order_info[2].Board_Work = ON;
        Board_Order_info[1].Board_Work = ON;
        Board_Info_Tx();
        osDelay(30);

        }

        // --- 4. 第四组击破 (2,1) -> 进度 4/5 ---
        if (Big_Fu_info[2].Single_Hit_State == ON && Big_Fu_info[1].Single_Hit_State == ON)
        {
        Big_Fu_info[2].Single_Hit_State = 0;
        Big_Fu_info[1].Single_Hit_State = 0;
        // 全局进度更新为 4
        Board_Order_info[0].count_t = 4;
        Board_Order_info[1].count_t = 4;
        Board_Order_info[2].count_t = 4;
        Board_Order_info[3].count_t = 4;
        Board_Order_info[4].count_t = 4;
        Board_Order_info[0].Board_Work = OFF;
        Board_Order_info[1].Board_Work = OFF;
        Board_Order_info[2].Board_Work = OFF;
        Board_Order_info[3].Board_Work = OFF;
        Board_Order_info[4].Board_Work = OFF;
        Board_Order_info[4].Board_Work = ON;
        Board_Order_info[3].Board_Work = ON;
        Board_Info_Tx();
        osDelay(30);

        }

        // --- 5. 第五组击破 (3,4) -> 进度 5/5 (通关) ---
        if (Big_Fu_info[3].Single_Hit_State == ON && Big_Fu_info[4].Single_Hit_State == ON)
        {
        Big_Fu_info[3].Single_Hit_State = 0;
        Big_Fu_info[4].Single_Hit_State = 0;
        // 全局进度更新为 5
        Board_Order_info[0].count_t = 5;
        Board_Order_info[1].count_t = 5;
        Board_Order_info[2].count_t = 5;
        Board_Order_info[3].count_t = 5;
        Board_Order_info[4].count_t = 5;
        Board_Order_info[0].Board_Work = OFF;
        Board_Order_info[1].Board_Work = OFF;
        Board_Order_info[2].Board_Work = OFF;
        Board_Order_info[3].Board_Work = OFF;
        Board_Order_info[4].Board_Work = OFF;

        // 【核心修复1】：必须连发5次！确保所有5个板子都立刻收到 count_t = 5！
        for (int k = 0; k < 5; k++)
        {
            Board_Info_Tx();
            osDelay(2); // 加2ms微小延时，防止CAN发送邮箱爆满丢包
        }

        osDelay(30);
        }
        // --- 6. 检测大满贯 (检测 count_t 是否等于 5) ---
        // 【核心修复2】：必须加上 else！保证打完第5组后，等到下一个周期再执行大满贯，给子板亮灯的时间！
        else if (Board_Order_info[0].count_t == 5
             && Board_Order_info[1].count_t == 5
             && Board_Order_info[2].count_t == 5
             && Board_Order_info[3].count_t == 5
             && Board_Order_info[4].count_t == 5)
        {
        osDelay(1000); // 停顿1秒，完美展示全亮灯效
        Board_Order_info[0].count_t = 6;
        Board_Order_info[1].count_t = 6;
        Board_Order_info[2].count_t = 6;
        Board_Order_info[3].count_t = 6;
        Board_Order_info[4].count_t = 6;

        // 【核心修复3】：同样连发5次，确保所有板子都收到全灭指令！
        for (int k = 0; k < 5; k++)
        {
            Board_Info_Tx();
            osDelay(2);
        }

        // 【核心修复4】：一定要把 wjz 清零！否则游戏直接死机，没法重新开局！
        wjz = 0;
        osDelay(30);
        }
        }
        */
        Board_Info_Tx();
        osDelay(30);
    }

    /* USER CODE END StartDefaultTask */
}

//            // --- 5. 第五组击破 (3,4) -> 进度 5/5 (通关) ---
//            if (Big_Fu_info[3].Single_Hit_State == ON && Big_Fu_info[4].Single_Hit_State == ON)
//            {
//                Big_Fu_info[3].Single_Hit_State = 0;
//                Big_Fu_info[4].Single_Hit_State = 0;
//							  // 全局进度更新为 5
//                Board_Order_info[0].count_t = 5;
//                Board_Order_info[1].count_t = 5;
//                Board_Order_info[2].count_t = 5;
//                Board_Order_info[3].count_t = 5;
//                Board_Order_info[4].count_t = 5;
//                Board_Order_info[0].Board_Work = OFF;
//                Board_Order_info[1].Board_Work = OFF;
//                Board_Order_info[2].Board_Work = OFF;
//                Board_Order_info[3].Board_Work = OFF;
//                Board_Order_info[4].Board_Work = OFF;
//                Board_Info_Tx();
//                osDelay(30);
////                // 全局进度更新为 5
////                Board_Order_info[0].count_t = 5;
////                Board_Order_info[1].count_t = 5;
////                Board_Order_info[2].count_t = 5;
////                Board_Order_info[3].count_t = 5;
////                Board_Order_info[4].count_t = 5;
////                Board_Info_Tx();
////                osDelay(30);
//            }

//            // --- 6. 检测大满贯 (检测 count_t 是否等于 5) ---
//            if (Board_Order_info[0].count_t == 5
//                    && Board_Order_info[1].count_t == 5
//                    && Board_Order_info[2].count_t == 5
//                    && Board_Order_info[3].count_t == 5
//                    && Board_Order_info[4].count_t == 5)
//            {
////                Board_Order_info[0].Board_Work = OFF;
////                Board_Order_info[1].Board_Work = OFF;
////                Board_Order_info[2].Board_Work = OFF;
////                Board_Order_info[3].Board_Work = OFF;
////                Board_Order_info[4].Board_Work = OFF;
//								osDelay(1000);
//								Board_Order_info[0].count_t = 6;
//                Board_Order_info[1].count_t = 6;
//                Board_Order_info[2].count_t = 6;
//                Board_Order_info[3].count_t = 6;
//                Board_Order_info[4].count_t = 6;
//                Board_Info_Tx();
//								osDelay(30);
//
//
//                //					switch(wcis)
//                //					{
//                //						case 0:
//                //						{
//                //							Board_Order_info[0].Board_Work = OFF;
//                //							Board_Order_info[1].Board_Work = OFF;
//                //							Board_Order_info[2].Board_Work = OFF;
//                //							Board_Order_info[3].Board_Work = OFF;
//                //							Board_Order_info[4].Board_Work = OFF;
//                //							wcis ++;
//                //						}break;
//                //						case 1:
//                //						{
//                //							osDelay(300);
//                //							Board_Order_info[0].Board_Work = ON;
//                //							Board_Order_info[1].Board_Work = ON;
//                //							Board_Order_info[2].Board_Work = ON;
//                //							Board_Order_info[3].Board_Work = ON;
//                //							Board_Order_info[4].Board_Work = ON;
//                //							wcis ++;
//                //						}break;
//                //						case 2:
//                //						{
//                //							osDelay(300);
//                //							Board_Order_info[0].Board_Work = OFF;
//                //							Board_Order_info[1].Board_Work = OFF;
//                //							Board_Order_info[2].Board_Work = OFF;
//                //							Board_Order_info[3].Board_Work = OFF;
//                //							Board_Order_info[4].Board_Work = OFF;
//                //							wcis ++;
//                //						}break;
//                //						case 3:
//                //						{
//                //							osDelay(300);
//                //							Board_Order_info[0].Board_Work = ON;
//                //							Board_Order_info[1].Board_Work = ON;
//                //							Board_Order_info[2].Board_Work = ON;
//                //							Board_Order_info[3].Board_Work = ON;
//                //							Board_Order_info[4].Board_Work = ON;
//                //							wcis ++;
//                //						}break;
//                //						case 4:
//                //						{
//                //							osDelay(300);
//                //							Board_Order_info[0].Board_Work = OFF;
//                //							Board_Order_info[1].Board_Work = OFF;
//                //							Board_Order_info[2].Board_Work = OFF;
//                //							Board_Order_info[3].Board_Work = OFF;
//                //							Board_Order_info[4].Board_Work = OFF;
//                //							wcis ++;
//                //						}break;
//                //						case 5:
//                //						{
//                //							osDelay(300);
//                //							Board_Order_info[0].Board_Work = ON;
//                //							Board_Order_info[1].Board_Work = ON;
//                //							Board_Order_info[2].Board_Work = ON;
//                //							Board_Order_info[3].Board_Work = ON;
//                //							Board_Order_info[4].Board_Work = ON;
//                //							wcis ++;
//                //						}break;
//                //						case 6:
//                //						{
//                //							osDelay(300);
//                //							Big_Fu_info[rand_num_0].Single_Hit_State=
//                //							Big_Fu_info[rand_num_1].Single_Hit_State=
//                //							Big_Fu_info[rand_num_2].Single_Hit_State=
//                //							Big_Fu_info[rand_num_3].Single_Hit_State=
//                //							Big_Fu_info[rand_num_4].Single_Hit_State=0;
//                //							Board_Order_info[0].count_t = 0;
//                //							Board_Order_info[1].count_t = 0;
//                //							Board_Order_info[2].count_t = 0;
//                //							Board_Order_info[3].count_t = 0;
//                //							Board_Order_info[4].count_t = 0;
//                //							Board_Order_info[0].Board_Work = OFF;
//                //							Board_Order_info[1].Board_Work = OFF;
//                //							Board_Order_info[2].Board_Work = OFF;
//                //							Board_Order_info[3].Board_Work = OFF;
//                //							Board_Order_info[4].Board_Work = OFF;
//                //							wcis = 0;
//                //							wjz = 0;
//                //						}break;
//                //					}
//            }


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
