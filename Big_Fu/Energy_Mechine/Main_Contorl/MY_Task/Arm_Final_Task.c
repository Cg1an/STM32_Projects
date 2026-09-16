#include "main.h"

uint8_t nn;
int16_t StartFlag=0,Back_DM;

extern Data_From_C Data_C ;

void Start_ChassisTask(void const * argument)
{
	Chassis_PID_Init();
  /* USER CODE BEGIN Start_Arm_Final_Task */
  /* Infinite loop */
  for(;;)
  {
	Chassis_control();//底盘四个电机控制
  osDelay(1);
	}
}
  /* USER CODE END Start_Arm_Final_Task */



