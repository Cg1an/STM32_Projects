/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

 #include "RS485_work.h"
 #include "usbd_customhid.h"
#include "usbd_ctlreq.h"
#include "i2c.h"




/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint8_t a=0;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId Device01Handle;
osThreadId Device02Handle;
osThreadId Device03Handle;
osThreadId Device04Handle;
osThreadId Device05Handle;
osThreadId Device06Handle;
osThreadId Device07Handle;
osThreadId Device08Handle;
osThreadId Device09Handle;
osThreadId Device0AHandle;
osThreadId Device0BHandle;
osThreadId Device0CHandle;
osThreadId Device0DHandle;
osThreadId Device0EHandle;
osThreadId Device0FHandle;
osThreadId RS485_SlaveHandle;
osThreadId RS485_MasterHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void Device01_task(void const * argument);
void Device02_task(void const * argument);
void Device03_task(void const * argument);
void Device04_task(void const * argument);
void Device05_task(void const * argument);
void Device06_task(void const * argument);
void Device07_task(void const * argument);
void Device08_task(void const * argument);
void Device09_task(void const * argument);
void Device0A_task(void const * argument);
void Device0B_task(void const * argument);
void Device0C_task(void const * argument);
void Device0D_task(void const * argument);
void Device0E_task(void const * argument);
void Device0F_task(void const * argument);
void RS485_Slave_task(void const * argument);
void RS485_Master_task(void const * argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
	
	    /* definition and creation of defaultTask */
      osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
      defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);
	    //默认启动任务
  if((freertos_task_list&Device_task_1_ID)!=OFF)
	{
	     osThreadDef(Device01, Device01_task, osPriorityIdle, 0, 128);
       Device01Handle = osThreadCreate(osThread(Device01), NULL);
	}
	if((freertos_task_list&Device_task_2_ID)!=OFF)
	{
	     osThreadDef(Device02, Device02_task, osPriorityIdle, 0, 128);
       Device01Handle = osThreadCreate(osThread(Device02), NULL);
	}
	if((freertos_task_list&Device_task_3_ID)!=OFF)
	{
		   /* definition and creation of Device03 */
      osThreadDef(Device03, Device03_task, osPriorityIdle, 0, 128);
      Device03Handle = osThreadCreate(osThread(Device03), NULL);
	}
	if((freertos_task_list&Device_task_4_ID)!=OFF)
	{
       /* definition and creation of Device04 */
       osThreadDef(Device04, Device04_task, osPriorityIdle, 0, 128);
       Device04Handle = osThreadCreate(osThread(Device04), NULL);
	}
	if((freertos_task_list&Device_task_5_ID)!=OFF)
	{
       /* definition and creation of Device05 */
       osThreadDef(Device05, Device05_task, osPriorityIdle, 0, 128);
       Device05Handle = osThreadCreate(osThread(Device05), NULL);
	}
	if((freertos_task_list&Device_task_6_ID)!=OFF)
  {
       /* definition and creation of Device06 */
       osThreadDef(Device06, Device06_task, osPriorityIdle, 0, 128);
       Device06Handle = osThreadCreate(osThread(Device06), NULL);
	}
	if((freertos_task_list&Device_task_7_ID)!=OFF)
	{
       /* definition and creation of Device07 */
       osThreadDef(Device07, Device07_task, osPriorityIdle, 0, 128);
       Device07Handle = osThreadCreate(osThread(Device07), NULL);
	}
	if((freertos_task_list&Device_task_8_ID)!=OFF)
	{
	     /* definition and creation of Device08 */
       osThreadDef(Device08, Device08_task, osPriorityIdle, 0, 128);
       Device08Handle = osThreadCreate(osThread(Device08), NULL); 
	}
	if((freertos_task_list&Device_task_9_ID)!=OFF)
	{
		   /* definition and creation of Device09 */
       osThreadDef(Device09, Device09_task, osPriorityIdle, 0, 128);
       Device09Handle = osThreadCreate(osThread(Device09), NULL);	
	}
	      osThreadDef(Device0F, Device0F_task, osPriorityIdle, 0, 128);
       Device0FHandle = osThreadCreate(osThread(Device0F), NULL);	
	if(RS485_WORK_SLAVE==OPEN) //非主即从
	{
		   /* definition and creation of RS485_Slave */
       osThreadDef(RS485_Slave, RS485_Slave_task, osPriorityIdle, 0, 128);
       RS485_SlaveHandle = osThreadCreate(osThread(RS485_Slave), NULL);
	
	}
	else
	{
	     osThreadDef(RS485_Master, RS485_Master_task, osPriorityIdle, 0, 128);
       RS485_MasterHandle = osThreadCreate(osThread(RS485_Master), NULL);
	}
	
	 /* definition and creation of myTask10 */
//	if((freertos_task_list&Device_task_1_ID)!=OFF)   //不是主机就是从机
//	{
//     osThreadDef(myTask10, StartTask10, osPriorityIdle, 0, 128);
//     myTask10Handle = osThreadCreate(osThread(myTask10), NULL);
//	}
//	else
//	{
//      /* definition and creation of myTask11 */
//      osThreadDef(myTask11, StartTask11, osPriorityIdle, 0, 128);
//      myTask11Handle = osThreadCreate(osThread(myTask11), NULL);	
//	}
	return ;
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  //osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of Device01 */
  osThreadDef(Device01, Device01_task, osPriorityIdle, 0, 128);
  Device01Handle = osThreadCreate(osThread(Device01), NULL);

  /* definition and creation of Device02 */
  osThreadDef(Device02, Device02_task, osPriorityIdle, 0, 128);
  Device02Handle = osThreadCreate(osThread(Device02), NULL);

  /* definition and creation of Device03 */
  osThreadDef(Device03, Device03_task, osPriorityIdle, 0, 128);
  Device03Handle = osThreadCreate(osThread(Device03), NULL);

  /* definition and creation of Device04 */
  osThreadDef(Device04, Device04_task, osPriorityIdle, 0, 128);
  Device04Handle = osThreadCreate(osThread(Device04), NULL);

  /* definition and creation of Device05 */
  osThreadDef(Device05, Device05_task, osPriorityIdle, 0, 128);
  Device05Handle = osThreadCreate(osThread(Device05), NULL);

  /* definition and creation of Device06 */
  osThreadDef(Device06, Device06_task, osPriorityIdle, 0, 128);
  Device06Handle = osThreadCreate(osThread(Device06), NULL);

  /* definition and creation of Device07 */
  osThreadDef(Device07, Device07_task, osPriorityIdle, 0, 128);
  Device07Handle = osThreadCreate(osThread(Device07), NULL);

  /* definition and creation of Device08 */
  osThreadDef(Device08, Device08_task, osPriorityIdle, 0, 128);
  Device08Handle = osThreadCreate(osThread(Device08), NULL);

  /* definition and creation of Device09 */
  osThreadDef(Device09, Device09_task, osPriorityIdle, 0, 128);
  Device09Handle = osThreadCreate(osThread(Device09), NULL);

  /* definition and creation of Device0A */
  osThreadDef(Device0A, Device0A_task, osPriorityIdle, 0, 128);
  Device0AHandle = osThreadCreate(osThread(Device0A), NULL);

  /* definition and creation of Device0B */
  osThreadDef(Device0B, Device0B_task, osPriorityIdle, 0, 128);
  Device0BHandle = osThreadCreate(osThread(Device0B), NULL);

  /* definition and creation of Device0C */
  osThreadDef(Device0C, Device0C_task, osPriorityIdle, 0, 128);
  Device0CHandle = osThreadCreate(osThread(Device0C), NULL);

  /* definition and creation of Device0D */
  osThreadDef(Device0D, Device0D_task, osPriorityIdle, 0, 128);
  Device0DHandle = osThreadCreate(osThread(Device0D), NULL);

  /* definition and creation of Device0E */
  osThreadDef(Device0E, Device0E_task, osPriorityIdle, 0, 128);
  Device0EHandle = osThreadCreate(osThread(Device0E), NULL);

  /* definition and creation of Device0F */
  //osThreadDef(Device0F, Device0F_task, osPriorityIdle, 0, 128);
  Device0FHandle = osThreadCreate(osThread(Device0F), NULL);

  /* definition and creation of RS485_Slave */
  osThreadDef(RS485_Slave, RS485_Slave_task, osPriorityIdle, 0, 128);
  RS485_SlaveHandle = osThreadCreate(osThread(RS485_Slave), NULL);

  /* definition and creation of RS485_Master */
  osThreadDef(RS485_Master, RS485_Master_task, osPriorityIdle, 0, 128);
  RS485_MasterHandle = osThreadCreate(osThread(RS485_Master), NULL);

  /* USER CODE BEGIN RTOS_THREADS */

  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
	
//变量统计
uint8_t  CH423_20_1RETURNKEY;//键盘返回变量
uint32_t CH423_20_1SETLED;   // LED控制变量
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN StartDefaultTask */

  /* Infinite loop */
  for(;;)
  {
	   HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_RESET);
		 osDelay(300);
		 HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_SET);
		 osDelay(300);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_Device01_task */
/**
* @brief Function implementing the Device01 thread.
* @param argument: Not used
* @retval None
*/
uint8_t  iic_init_clock=0;
/* USER CODE END Header_Device01_task */
void Device01_task(void const * argument)
{
  /* USER CODE BEGIN Device01_task */
	//这个任务是电源管理器任务
   CH351_7KEY_ONLY_init();
	 CH351_7KEY_ONLY_init_CN1();
	 CH453_12NUM_3LED_10KEY_init();
  /* Infinite loop */
  for(;;)
  {
	  // CH453_WRITE3LED(0x00);
		 CH351_8LED_WRITE(0x0f);
	   CH351_24KEY_GET7();
		 CH351_24KEY_GET7_CN1();
     HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_RESET);
		 //CH453_WRITE3LED(CH453_12NUM_3LED[12]);
		 CH351_8LED_WRITE_CN1(~CH453_12NUM_3LED[12]);
		 HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_SET);
		 osDelay(20);
	   //CH453_3_10_10KEY= CH453_READ10KEY(); 
		 CH453_WRITE12NUM(CH453_12NUM_3LED,12);
		 osDelay(20);
		 
		 HAL_I2C_MspDeInit(&hi2c2);
		 osDelay(20);
		 HAL_I2C_MspInit(&hi2c2);
		 iic_init_clock++;
		 // MX_I2C2_Init();
	   
  }
  /* USER CODE END Device01_task */
}

/* USER CODE BEGIN Header_Device02_task */
/**
* @brief Function implementing the Device02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Device02_task */
void Device02_task(void const * argument)
{
  /* USER CODE BEGIN Device02_task */
	//CH351_7KEY_ONLY_init();
  /* Infinite loop */
  for(;;)
  {
     //CH351_24KEY_GET7();
     osDelay(1000);
  }
  /* USER CODE END Device02_task */
}

/* USER CODE BEGIN Header_Device03_task */
/**
* @brief Function implementing the Device03 thread.
* @param argument: Not used
* @retval None
*/
//cechangbanganlunsi
/* USER CODE END Header_Device03_task */
void Device03_task(void const * argument)
{
  /* USER CODE BEGIN Device03_task */
	CH351_lunshi_init();
  /* Infinite loop */
  for(;;)
  {
		 //this task usb to drive use one ch351
     CH351_24KEY_ALL();
     CH351_8LED_ALL(0);//~lunshi_LED_8);
		 HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_SET);
     osDelay(3);
//		 HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_RESET);
//     osDelay(10);
  }
  /* USER CODE END Device03_task */
}

/* USER CODE BEGIN Header_Device04_task */
/**
* @brief Function implementing the Device04 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Device04_task */
void Device04_task(void const * argument)
{
  /* USER CODE BEGIN Device04_task */
  /* Infinite loop */
  for(;;)
  {
     osDelay(1);
  }
  /* USER CODE END Device04_task */
}

/* USER CODE BEGIN Header_Device05_task */
/**
* @brief Function implementing the Device05 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Device05_task */
void Device05_task(void const * argument)
{
  /* USER CODE BEGIN Device05_task */
  /* Infinite loop */
  for(;;)
  {
     osDelay(1);
  }
  /* USER CODE END Device05_task */
}

/* USER CODE BEGIN Header_Device06_task */
/**
* @brief Function implementing the Device06 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Device06_task */
void Device06_task(void const * argument)
{
  /* USER CODE BEGIN Device06_task */
  /* Infinite loop */
  for(;;)
  {
     osDelay(1);
  }
  /* USER CODE END Device06_task */
}

/* USER CODE BEGIN Header_Device07_task */
/**
* @brief Function implementing the Device07 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Device07_task */
void Device07_task(void const * argument)
{
  /* USER CODE BEGIN Device07_task */
  /* Infinite loop */
  for(;;)
  {
     osDelay(1);
  }
  /* USER CODE END Device07_task */
}

/* USER CODE BEGIN Header_Device08_task */
/**
* @brief Function implementing the Device08 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Device08_task */
void Device08_task(void const * argument)
{
  /* USER CODE BEGIN Device08_task */
  /* Infinite loop */
  for(;;)
  {
     osDelay(1);
  }
  /* USER CODE END Device08_task */
}

/* USER CODE BEGIN Header_Device09_task */
/**
* @brief Function implementing the Device09 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Device09_task */
void Device09_task(void const * argument)
{
  /* USER CODE BEGIN Device09_task */
  /* Infinite loop */
  for(;;)
  {
     osDelay(1);
  }
  /* USER CODE END Device09_task */
}

/* USER CODE BEGIN Header_Device0A_task */
/**
* @brief Function implementing the Device0A thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Device0A_task */
void Device0A_task(void const * argument)
{
  /* USER CODE BEGIN Device0A_task */
  /* Infinite loop */
  for(;;)
  {
     osDelay(1);
  }
  /* USER CODE END Device0A_task */
}

/* USER CODE BEGIN Header_Device0B_task */
/**
* @brief Function implementing the Device0B thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Device0B_task */
void Device0B_task(void const * argument)
{
  /* USER CODE BEGIN Device0B_task */
  /* Infinite loop */
  for(;;)
  {
     osDelay(1);
  }
  /* USER CODE END Device0B_task */
}

/* USER CODE BEGIN Header_Device0C_task */
/**
* @brief Function implementing the Device0C thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Device0C_task */
void Device0C_task(void const * argument)
{
  /* USER CODE BEGIN Device0C_task */
  /* Infinite loop */
  for(;;)
  {
     osDelay(1);
  }
  /* USER CODE END Device0C_task */
}

/* USER CODE BEGIN Header_Device0D_task */
/**
* @brief Function implementing the Device0D thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Device0D_task */
void Device0D_task(void const * argument)
{
  /* USER CODE BEGIN Device0D_task */
  /* Infinite loop */
  for(;;)
  {
     osDelay(1);
  }
  /* USER CODE END Device0D_task */
}

/* USER CODE BEGIN Header_Device0E_task */
/**
* @brief Function implementing the Device0E thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Device0E_task */
void Device0E_task(void const * argument)
{
  /* USER CODE BEGIN Device0E_task */
  /* Infinite loop */
  for(;;)
  {
     osDelay(1);
  }
  /* USER CODE END Device0E_task */
}

/* USER CODE BEGIN Header_Device0F_task */
/**
* @brief Function implementing the Device0F thread.
* @param argument: Not used
* @retval None
*/
uint8_t USB_TXTX[64]={0X00,0xA5,0xFF,0xFF,0xFF};
/* USER CODE END Header_Device0F_task */
void Device0F_task(void const * argument)
{
  /* USER CODE BEGIN Device0F_task */
	  queue_init();
  /* Infinite loop */
  for(;;)
  {
	   osDelay(20);
		 queue_data_out(USB_TXTX,&usb_rx_queue);    
	   USBD_CUSTOM_HID_SendReport(&hUsbDeviceHS, USB_TXTX, 64);	

  }
  /* USER CODE END Device0F_task */
}

/* USER CODE BEGIN Header_RS485_Slave_task */
/**
* @brief Function implementing the RS485_Slave thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_RS485_Slave_task */
void RS485_Slave_task(void const * argument)
{
  /* USER CODE BEGIN RS485_Slave_task */
	RS485_init_slave(); 
  /* Infinite loop */
  for(;;)
  {
     osDelay(1);
		 if(receive_point==1)//收到数据需要回传
		 {   
		     RS485_OPEN_TX;
		     osDelay(1);
				 HAL_UART_Transmit_DMA(&huart2,(uint8_t *)Slave_RS485_TX_buffer,RS485_TX_length); 
		     osDelay(1);				
				// RS485_OPEN_RX;	
         receive_point=0;//关闭发送				 
		 }
  }
  /* USER CODE END RS485_Slave_task */
}

/* USER CODE BEGIN Header_RS485_Master_task */
/**
* @brief Function implementing the RS485_Master thread.
* @param argument: Not used
* @retval None
*/
uint8_t  hi[10]={"hello"};
uint8_t  word[10]={"world"};
uint8_t test;
/* USER CODE END Header_RS485_Master_task */
void RS485_Master_task(void const * argument)
{
  /* USER CODE BEGIN RS485_Master_task */
	RS485_init_Master(); 
  /* Infinite loop */
  for(;;)
  {

	   //if(usb_receive_point==1)
		 // {		     
				 Split_usb_data();
				// usb_receive_point=0;	 
		 //}
     osDelay(5);
  }
  /* USER CODE END RS485_Master_task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
