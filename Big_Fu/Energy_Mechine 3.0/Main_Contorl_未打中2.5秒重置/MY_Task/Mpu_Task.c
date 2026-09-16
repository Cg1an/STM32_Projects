#include "main.h"

void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
//	mpu_get_data();
//	imu_ahrs_update();
//	imu_attitude_update();
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}
