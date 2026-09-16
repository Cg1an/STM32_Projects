/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "cmsis_os.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#define OPEN 1
#define OFF  0
//定义设备开关状态

uint8_t   CH423_6LED_5LED_2KEY=0;    //车载电源
uint8_t   CH423_20LED_1KEY=0;        //数化同传
uint8_t   CH423_8LED_3KEY=0;         //安全防护
uint8_t   CH453_12NUM_3LED_10KEY=0;  //电源管理器
uint8_t   LCD_CH351KEY_CH45210KEY=0; //BD时钟接线盒
uint8_t   CH351_20LED_2KEY=0;        //网络通信控制设备
uint8_t   CH351_24LED_2KEY=0;        //综合接口设备
uint8_t   CH351_8LED_24KEY=0;        //司机右面板
uint8_t   OLED1_3_9KEY=0;            //同步设备
uint8_t   RS485_WORK_SLAVE=0;        //RS485通信任务
uint8_t   DECEIVE_ID=0;              //设备ID
uint16_t  FUNCTION_ID;               //启用功能id
uint8_t   SWITCH_MODE=0;             //开关状态


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
//		HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_SET);
//    HAL_Delay(500);
//		HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_RESET);
//    HAL_Delay(500);
//		HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_SET);
//    HAL_Delay(500);
//		HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_RESET);
//    HAL_Delay(500);
//		HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,GPIO_PIN_SET);
//    HAL_Delay(500);
//		HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,GPIO_PIN_RESET);
//    HAL_Delay(500);
//		HAL_GPIO_WritePin(LED4_GPIO_Port,LED4_Pin,GPIO_PIN_SET);
//    HAL_Delay(500);
//		HAL_GPIO_WritePin(LED4_GPIO_Port,LED4_Pin,GPIO_PIN_RESET);
//    HAL_Delay(500);
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
void GET_SWITCH(void)
{
    SWITCH_MODE+=HAL_GPIO_ReadPin(SWITCH1_GPIO_Port,SWITCH1_Pin);
	  SWITCH_MODE=SWITCH_MODE<<1;
	  SWITCH_MODE+=HAL_GPIO_ReadPin(SWITCH2_GPIO_Port,SWITCH2_Pin);
	  SWITCH_MODE=SWITCH_MODE<<1;
	  SWITCH_MODE+=HAL_GPIO_ReadPin(SWITCH3_GPIO_Port,SWITCH3_Pin);
	  SWITCH_MODE=SWITCH_MODE<<1;
	  SWITCH_MODE+=HAL_GPIO_ReadPin(SWITCH4_GPIO_Port,SWITCH4_Pin);
}
void CN_SELECT_init(void)
{
	 //首先根据拨码开关判断ID
	 //然后根据ID对相应的任务启动和初始化
	 //之后完成
	  DEVICE_ID_LIST_INIT();//地址表初始化
	  freertos_task_list=0; //任务列表初始化
	 	GET_SWITCH();         //拨码开关初始化
		switch (SWITCH_MODE)
		{
		   case Slave_1_ID://从机拨码开关  
			 {
			    //0X01			
			    freertos_task_list=Device_task_1_ID|Device_task_2_ID; //定义从机设备挂载的任务类型
				  RS485_WORK_SLAVE=1;   					                                      //打开从机接受功能
					DECEIVE_ID=SWITCH_MODE; 			                                        //定义从机设备ID
				  break;
			 }
			 case Slave_2_ID: 
       {	
          freertos_task_list=Device_task_3_ID;
				  RS485_WORK_SLAVE=1;   
					DECEIVE_ID=SWITCH_MODE;
    		  break;
					
					
			 }
			 case Slave_3_ID:  break;
			 case Slave_4_ID:  break;
			 case Slave_5_ID:  break;
			 case Slave_6_ID:  break;
			 case Slave_7_ID:  break;
			 case Slave_8_ID:  break;
			 case Slave_9_ID:  break;
			 case Slave_10_ID: break;	 			 
			 case Master_1_ID: 
			 {
			 
			 	  RS485_WORK_SLAVE=0;   
			    break; 
			 }
	
	 }
	 
	 
	 return;
		 
		 	 //CH423_6LED_5LED_2KEY=1;
		 
//	// GET_SWITCH();
//   if(SWITCH_MODE==0x0f)//没有开关或者全部拉高，默认主机模式
//	 {
//	     RS485_WORK_SLAVE=0; 
//	 }
//	 else
//	 {
//			 CH423_20LED_1KEY=1;
//			 //CH423_6LED_5LED_2KEY=1;
//			 //CH423_8LED_3KEY=1;
//			 //CH453_12NUM_3LED_10KEY=1;
//			 //CH351_20LED_2KEY=1;
//			 //LCD_CH351KEY_CH45210KEY=1;
//			 //CH351_24LED_2KEY=1;
//			 //CH351_8LED_24KEY=1;
//			 //OLED1_3_9KEY=1;
//			 RS485_WORK_SLAVE=1;
//			 FUNCTION_ID=0x55;
//			 DECEIVE_ID=1;  
//	 }

}
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  MX_DMA_Init();
	MX_USB_DEVICE_Init();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C2_Init();
  MX_SPI3_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  MX_USART1_UART_Init();
  MX_I2C3_Init();
  /* USER CODE BEGIN 2 */
  CN_SELECT_init();
  /* USER CODE END 2 */

  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();
  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
//外部中断函数
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
   if(GPIO_Pin==CH351_INT_Pin)
	 {
	    int a;
		  a++; 
	 }

}
/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM7 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM7) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {


		
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

