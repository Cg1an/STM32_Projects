/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *  2023 RobotPiolts
  *  锟斤拷锟斤拷锟斤拷锟斤拷小锟斤拷锟截达拷锟斤拷
  *  锟斤拷锟竭ｏ拷zs
	*  锟绞硷拷锟斤拷szhangszu@163.com
	*  QQ:   1273890169
  * 
  * 
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "can.h"
#include "dma.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "WS2812.h"
#include "can_protocal.h"
#include "Flag_State.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint8_t up_state;
uint16_t cntt;
uint16_t down_cntt;
uint16_t Twinkle_time_cnt, Twinkle_time_cnt_times;
Rings_Key_t Rings_Key;
Rings_Key_record_t Rings_Key_record;
uint16_t buffer_time;

uint16_t flag_mode,flag_color,flag_turn,flag_down;
uint16_t flag_mode_last,flag_color_last;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define YES 1
#define NO  0

//锟斤拷锟斤拷叶锟斤拷锟斤拷锟斤拷牛锟?-4

#define BOARD_NUMBER   1

//锟斤拷锟斤拷叶锟斤拷锟斤拷锟斤拷牛锟?-4


#define RED_NUM       150
#define GREEN_NUM     150
#define BLUE_NUM      150

int wait_time;
int chen1=200;
int flag_;
uint8_t color_R,color_G,color_B;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
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

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_CAN_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM1_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  CAN1_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	 HAL_Delay(2000);
		
	Big_Fu_info_Init(Big_Fu_info);
	WS_WriteAll_RGB_FRAME_DOWN(0,0,0);//点亮PCB下边沿函数 PA1       tim2, TIM_CHANNEL_2
	WS_WriteAll_RGB_FRAME(0,0,0);///////点亮边框灯条函数  PA9       tim1, TIM_CHANNEL_2
	WS_WriteAll_RGB_REC(0,0,0);/////////点亮箭头灯带函数  PA8       tim1, TIM_CHANNEL_1
	WS_WriteAll_RGB_FRAME_UP(0,0,0); ///点亮PCB上边沿函数 PC6       tim3, TIM_CHANNEL_1
	WS_CloseAll_Circle();///////////////关闭PCB靶环函数   PA0       tim2, TIM_CHANNEL_1
	LED_Blue_rectangle_current(0,0,0,0);	

	while (1)
  {	
			Board_Info_Tx();
		/*读取微动开关数据*/
			Rings_Key.one_ring   = HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13);  
			Rings_Key.two_ring   = HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_14);  
			Rings_Key.three_ring = HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_0);  
			Rings_Key.four_ring  = HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_1);  
			Rings_Key.five_ring  = HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_2);  
			Rings_Key.six_ring   = HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_3);  
			Rings_Key.seven_ring = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4);  
			Rings_Key.eight_ring = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_5);  
			Rings_Key.nine_ring  = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_6);  
		  Rings_Key.ten_ring   = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_7);    
//       Rings_Key.ten_ring=1;
      //如果按键损坏，临时解决方案为拉高整个环的引脚
		
		
		 if(!Board_Order_info[BOARD_NUMBER].RainBow_Flag) //不为流水效果
 {			
		/* 颜色实时更新*/
		Big_Fu_info[BOARD_NUMBER].Color =  Board_Order_info[BOARD_NUMBER].Set_Color;  
		
		/*颜色跳变：所有计数计时变量以及标志位清零*/
		if(Big_Fu_info[BOARD_NUMBER].Color != Big_Fu_info[BOARD_NUMBER].Last_Color)//颜色变化
		{
			Twinkle_time_cnt = 0;
			Twinkle_time_cnt_times = 0;
			buffer_time = 0;
			WS_WriteAll_RGB_FRAME_DOWN(0,0,0);
			WS_WriteAll_RGB_FRAME(0,0,0);
			WS_WriteAll_RGB_REC(0,0,0);
			WS_WriteAll_RGB_FRAME_UP(0,0,0);	
			Big_Fu_info[BOARD_NUMBER].Single_Hit_State = 0;
			Big_Fu_info[BOARD_NUMBER].Hit_LED_state = 0;
			Big_Fu_info[BOARD_NUMBER].Twinkle_Already = 0;
			
			WS_CloseAll_Circle();
			
			Rings_Key_record.one_ring_record = 0;
			Rings_Key_record.two_ring_record = 0;
			Rings_Key_record.three_ring_record = 0;
			Rings_Key_record.four_ring_record = 0;
			Rings_Key_record.five_ring_record = 0;
			Rings_Key_record.six_ring_record = 0;
			Rings_Key_record.seven_ring_record = 0;
			Rings_Key_record.eight_ring_record = 0;
			Rings_Key_record.nine_ring_record = 0;
			Rings_Key_record.ten_ring_record = 0;
			cntt = 0;
		}
		
		Big_Fu_info[BOARD_NUMBER].Last_Color = Big_Fu_info[BOARD_NUMBER].Color;

		if(Board_Order_info[BOARD_NUMBER].Board_Work == 0)
		{
		    WS_WriteAll_RGB_FRAME_DOWN(0,0,0);
				WS_WriteAll_RGB_REC(0,0,0);
				WS_WriteAll_RGB_FRAME_UP(0,0,0);	
			  WS_WriteAll_RGB_FRAME(0,0,0);
		   	WS_CloseAll_Circle();
			  Big_Fu_info[BOARD_NUMBER].Single_Hit_State = 0;
				Big_Fu_info[BOARD_NUMBER].Hit_LED_state = 0;
			  Big_Fu_info[BOARD_NUMBER].Twinkle_Already = 0;
			
			  Rings_Key_record.one_ring_record = 0;
				Rings_Key_record.two_ring_record = 0;
				Rings_Key_record.three_ring_record = 0;
				Rings_Key_record.four_ring_record = 0;
				Rings_Key_record.five_ring_record = 0;
				Rings_Key_record.six_ring_record = 0;
				Rings_Key_record.seven_ring_record = 0;
				Rings_Key_record.eight_ring_record = 0;
				Rings_Key_record.nine_ring_record = 0;
				Rings_Key_record.ten_ring_record = 0;
		   	cntt = 0;
				buffer_time = 0;
		}
		/*蓝色模式*/
		if(Big_Fu_info[BOARD_NUMBER].Color == BLUE)		
		{
			/*待击打显示*/
			if(Big_Fu_info[BOARD_NUMBER].Hit_LED_state == NO && Big_Fu_info[BOARD_NUMBER].Single_Hit_State == 0 && Board_Order_info[BOARD_NUMBER].Board_Work == 1)
			{			
        WS_Write_Cross(0,0,BLUE_NUM);
				WS_Write_RGB_TWO_RING(0,0,BLUE_NUM);
				WS_Write_RGB_SIX_RING(0,0,BLUE_NUM);
				WS_Write_RGB_NINE_RING(0,0,BLUE_NUM);
	
				if(cntt<=50)
				cntt++;
				if(cntt <= 30)
				{				
				LED_Blue_rectangle_current(0,0,0,BLUE_NUM);	
				WS_WriteAll_RGB_FRAME_UP(0,0,BLUE_NUM);
        WS_WriteAll_RGB_FRAME_DOWN(0,0,BLUE_NUM);	
				}
				if(cntt >= 50)
				{
					HAL_TIM_PWM_Stop_DMA(&htim2, TIM_CHANNEL_2);
					HAL_TIM_PWM_Stop_DMA(&htim3, TIM_CHANNEL_1);
					cntt = 51;
				}
				/* 判断是否击打 */	
				if(Rings_Key.one_ring == 0
				||Rings_Key.two_ring == 0
		   	||Rings_Key.three_ring == 0
				||Rings_Key.four_ring == 0
	    	||Rings_Key.five_ring == 0
			  ||Rings_Key.six_ring == 0
				||Rings_Key.seven_ring == 0
			  ||Rings_Key.eight_ring == 0
			  ||Rings_Key.nine_ring == 0
			  ||Rings_Key.ten_ring == 0)
			{
		    Big_Fu_info[BOARD_NUMBER].Hit_LED_state = 1;  
				buffer_time++;
				WS_CloseAll_Circle();
		  }
			else
			{			
        buffer_time	 = 0;			  
			}
			/* buffer_time的设置是防止弹丸瞬间击打到两个环的中间部分导致两环电平同时变化从而同时点亮 */	
			
				if(buffer_time > 0)
				{
					  if(Rings_Key.one_ring == 0)  
						{					
							Rings_Key_record.one_ring_record = 1;
						}
						else if(Rings_Key.two_ring == 0)   
						{
							Rings_Key_record.two_ring_record = 1;
						}
						else if(Rings_Key.three_ring == 0) 
						{
							Rings_Key_record.three_ring_record = 1;
						}
						else if(Rings_Key.four_ring == 0)  
						{
							Rings_Key_record.four_ring_record = 1;
						}
						else if(Rings_Key.five_ring == 0)  
						{
							Rings_Key_record.five_ring_record = 1;
						}
						else if(Rings_Key.six_ring == 0)   
						{
							Rings_Key_record.six_ring_record = 1;
						}
						else if(Rings_Key.seven_ring == 0) 
						{
							Rings_Key_record.seven_ring_record = 1;
						}
						else if(Rings_Key.eight_ring == 0) 
						{
							Rings_Key_record.eight_ring_record = 1;
						}
						else if(Rings_Key.nine_ring == 0)  
						{
							Rings_Key_record.nine_ring_record = 1;
						}
						else if(Rings_Key.ten_ring == 0)  
						{
							Rings_Key_record.ten_ring_record = 1;						
						}
						
						 if(Rings_Key_record.one_ring_record && Rings_Key_record.two_ring_record)
						{
							Rings_Key_record.two_ring_record = 0;
						}
						else if(Rings_Key_record.two_ring_record && Rings_Key_record.three_ring_record)
						{
							Rings_Key_record.three_ring_record = 0;
						}
						else if(Rings_Key_record.three_ring_record && Rings_Key_record.four_ring_record)
						{
							Rings_Key_record.four_ring_record = 0;
						}
						else if(Rings_Key_record.four_ring_record && Rings_Key_record.five_ring_record)
						{
							Rings_Key_record.five_ring_record = 0;
						}
						else if(Rings_Key_record.five_ring_record && Rings_Key_record.six_ring_record)
						{
							Rings_Key_record.six_ring_record = 0;
						}
						else if(Rings_Key_record.six_ring_record && Rings_Key_record.seven_ring_record)
						{
							Rings_Key_record.seven_ring_record = 0;
						}
						else if(Rings_Key_record.seven_ring_record && Rings_Key_record.eight_ring_record)
						{
							Rings_Key_record.eight_ring_record = 0 ;
						}
						else if(Rings_Key_record.eight_ring_record && Rings_Key_record.nine_ring_record)
						{
							Rings_Key_record.nine_ring_record = 0;
						}
						else if(Rings_Key_record.nine_ring_record && Rings_Key_record.ten_ring_record)
						{
							Rings_Key_record.ten_ring_record = 0;
						}
						
				}
			}
      /*击中确认*/
			if(Big_Fu_info[BOARD_NUMBER].Hit_LED_state == YES && Board_Order_info[BOARD_NUMBER].Twinkle_state == NO && Board_Order_info[BOARD_NUMBER].Board_Work == 1)
			{
				Big_Fu_info[BOARD_NUMBER].Single_Hit_State =YES;
				cntt = 0;
				down_cntt = 0;
	
			}
			/*显示击中灯效*/
			if(Big_Fu_info[BOARD_NUMBER].Single_Hit_State == YES && Board_Order_info[BOARD_NUMBER].Twinkle_state == NO  )
			{
//				WS_WriteAll_RGB_REC(0,0,BLUE_NUM);//点亮箭头灯带函数
			 	LED_rectangle_current_REC(0,0,0,BLUE_NUM);	
					
        WS_WriteAll_RGB_FRAME(0,0,BLUE_NUM);
			
		
        if(Rings_Key_record.one_ring_record == 1)  
				{					
					WS_Write_RGB_ONE_RING(0,0,BLUE_NUM);
					
				}
		  	if(Rings_Key_record.two_ring_record == 1)   
				{
					WS_Write_RGB_TWO_RING(0,0,BLUE_NUM);					
				}
				if(Rings_Key_record.three_ring_record == 1) 
				{
					WS_Write_RGB_THREE_RING(0,0,BLUE_NUM);
				}
				if(Rings_Key_record.four_ring_record == 1)  
				{
					WS_Write_RGB_FOUR_RING(0,0,BLUE_NUM);
				}
				if(Rings_Key_record.five_ring_record == 1)  
				{
					WS_Write_RGB_FIVE_RING(0,0,BLUE_NUM);
				}
				if(Rings_Key_record.six_ring_record == 1)   
				{
					WS_Write_RGB_SIX_RING(0,0,BLUE_NUM);
				}
				if(Rings_Key_record.seven_ring_record == 1) 
				{
					WS_Write_RGB_SEVEN_RING(0,0,BLUE_NUM);
				}
				if(Rings_Key_record.eight_ring_record == 1) 
				{
					WS_Write_RGB_EIGHT_RING(0,0,BLUE_NUM);
				}
				if(Rings_Key_record.nine_ring_record == 1)  
				{
					WS_Write_RGB_NINE_RING(0,0,BLUE_NUM);
				}
				if(Rings_Key_record.ten_ring_record == 1)  
				{					
					WS_Write_RGB_TEN_RING(0,0,BLUE_NUM);
					WS_Write_RGB_EIGHT_RING(0,0,BLUE_NUM);
					WS_Write_RGB_SIX_RING(0,0,BLUE_NUM);
					WS_Write_RGB_FOUR_RING(0,0,BLUE_NUM);
					WS_Write_RGB_TWO_RING(0,0,BLUE_NUM);						
				}
				
//					wait_time++;
//				if(wait_time>chen1)
//        {Board_Order_info[BOARD_NUMBER].Board_Work=0,wait_time=0;
//				 }

			}
			
		}
		  /*红色模式*/
if(Big_Fu_info[BOARD_NUMBER].Color == RED)
		{		
      /*待击打显示*/
			if(Big_Fu_info[BOARD_NUMBER].Hit_LED_state == 0 && Big_Fu_info[BOARD_NUMBER].Single_Hit_State == 0 && Board_Order_info[BOARD_NUMBER].Board_Work == 1)
			{				
        WS_Write_Cross(RED_NUM,0,0);
				WS_Write_RGB_TWO_RING(RED_NUM,0,0);
				WS_Write_RGB_SIX_RING(RED_NUM,0,0);
				WS_Write_RGB_NINE_RING(RED_NUM,0,0);
				
				if(cntt<=70)
				cntt++;
				if(cntt <= 40)
				{				
					LED_Blue_rectangle_current(0,RED_NUM,0,0);	
          WS_WriteAll_RGB_FRAME_UP(RED_NUM,0,0);
          WS_WriteAll_RGB_FRAME_DOWN(RED_NUM,0,0);				
				}
				if(cntt >= 70)
				{
					HAL_TIM_PWM_Stop_DMA(&htim2, TIM_CHANNEL_2);
					HAL_TIM_PWM_Stop_DMA(&htim3, TIM_CHANNEL_1);
					cntt = 71;
				
				}
					if(Rings_Key.one_ring == 0
				||Rings_Key.two_ring == 0
		   	||Rings_Key.three_ring == 0
				||Rings_Key.four_ring == 0
	    	||Rings_Key.five_ring == 0
			  ||Rings_Key.six_ring == 0
				||Rings_Key.seven_ring == 0
			  ||Rings_Key.eight_ring == 0
			  ||Rings_Key.nine_ring == 0
			  ||Rings_Key.ten_ring == 0)
			{
		    Big_Fu_info[BOARD_NUMBER].Hit_LED_state = 1;  
				buffer_time++;
				WS_CloseAll_Circle();
		  }
			else
			{			
        buffer_time	 = 0;			  
			}
				if(buffer_time > 0)
				{
					  if(Rings_Key.one_ring == 0)  
						{					
							Rings_Key_record.one_ring_record = 1;
						}
						else if(Rings_Key.two_ring == 0)   
						{
							Rings_Key_record.two_ring_record = 1;
						}
						else if(Rings_Key.three_ring == 0) 
						{
							Rings_Key_record.three_ring_record = 1;
						}
						else if(Rings_Key.four_ring == 0)  
						{
							Rings_Key_record.four_ring_record = 1;
						}
						else if(Rings_Key.five_ring == 0)  
						{
							Rings_Key_record.five_ring_record = 1;
						}
						else if(Rings_Key.six_ring == 0)   
						{
							Rings_Key_record.six_ring_record = 1;
						}
						else if(Rings_Key.seven_ring == 0) 
						{
							Rings_Key_record.seven_ring_record = 1;
						}
						else if(Rings_Key.eight_ring == 0) 
						{
							Rings_Key_record.eight_ring_record = 1;
						}
						else if(Rings_Key.nine_ring == 0)  
						{
							Rings_Key_record.nine_ring_record = 1;
						}
						else if(Rings_Key.ten_ring == 0)  
						{
							Rings_Key_record.ten_ring_record = 1;						
						}
						
						 if(Rings_Key_record.one_ring_record && Rings_Key_record.two_ring_record)
						{
							Rings_Key_record.two_ring_record = 0;
						}
						else if(Rings_Key_record.two_ring_record && Rings_Key_record.three_ring_record)
						{
							Rings_Key_record.three_ring_record = 0;
						}
						else if(Rings_Key_record.three_ring_record && Rings_Key_record.four_ring_record)
						{
							Rings_Key_record.four_ring_record = 0;
						}
						else if(Rings_Key_record.four_ring_record && Rings_Key_record.five_ring_record)
						{
							Rings_Key_record.five_ring_record = 0;
						}
						else if(Rings_Key_record.five_ring_record && Rings_Key_record.six_ring_record)
						{
							Rings_Key_record.six_ring_record = 0;
						}
						else if(Rings_Key_record.six_ring_record && Rings_Key_record.seven_ring_record)
						{
							Rings_Key_record.seven_ring_record = 0;
						}
						else if(Rings_Key_record.seven_ring_record && Rings_Key_record.eight_ring_record)
						{
							Rings_Key_record.eight_ring_record = 0 ;
						}
						else if(Rings_Key_record.eight_ring_record && Rings_Key_record.nine_ring_record)
						{
							Rings_Key_record.nine_ring_record = 0;
						}
						else if(Rings_Key_record.nine_ring_record && Rings_Key_record.ten_ring_record)
						{
							Rings_Key_record.ten_ring_record = 0;
						}
						
				}
			}						
			
     /*击中显示*/
			if(Big_Fu_info[BOARD_NUMBER].Hit_LED_state == YES && Board_Order_info[BOARD_NUMBER].Twinkle_state == NO && Board_Order_info[BOARD_NUMBER].Board_Work == 1)
			{
				Big_Fu_info[BOARD_NUMBER].Single_Hit_State =YES;
				cntt = 0;
				down_cntt = 0;

			}
			if(Big_Fu_info[BOARD_NUMBER].Single_Hit_State == YES && Board_Order_info[BOARD_NUMBER].Twinkle_state == NO )
			{
  			

//				WS_WriteAll_RGB_REC(RED_NUM,0,0);	
				LED_rectangle_current_REC(0,RED_NUM,0,0);	
				WS_WriteAll_RGB_FRAME_DOWN(255,0,0);
				WS_WriteAll_RGB_FRAME(RED_NUM,0,0);
				
				
				if(Rings_Key_record.one_ring_record == 1)  
				{					
					WS_Write_RGB_ONE_RING(RED_NUM,0,0);
					
				}
		  	if(Rings_Key_record.two_ring_record == 1)   
				{
					WS_Write_RGB_TWO_RING(RED_NUM,0,0);				
				}
				if(Rings_Key_record.three_ring_record == 1) 
				{
					WS_Write_RGB_THREE_RING(RED_NUM,0,0);
				}
				if(Rings_Key_record.four_ring_record == 1)  
				{
					WS_Write_RGB_FOUR_RING(RED_NUM,0,0);
				}
				if(Rings_Key_record.five_ring_record == 1)  
				{
					WS_Write_RGB_FIVE_RING(RED_NUM,0,0);
				}
				if(Rings_Key_record.six_ring_record == 1)   
				{
					WS_Write_RGB_SIX_RING(RED_NUM,0,0);
				}
				if(Rings_Key_record.seven_ring_record == 1) 
				{
					WS_Write_RGB_SEVEN_RING(RED_NUM,0,0);
				}
				if(Rings_Key_record.eight_ring_record == 1) 
				{
					WS_Write_RGB_EIGHT_RING(RED_NUM,0,0);
				}
				if(Rings_Key_record.nine_ring_record == 1)  
				{
					WS_Write_RGB_NINE_RING(RED_NUM,0,0);
				}
				if(Rings_Key_record.ten_ring_record == 1)  
				{					
					WS_Write_RGB_TEN_RING(RED_NUM,0,0);
					WS_Write_RGB_EIGHT_RING(RED_NUM,0,0);
					WS_Write_RGB_SIX_RING(RED_NUM,0,0);
					WS_Write_RGB_FOUR_RING(RED_NUM,0,0);
					WS_Write_RGB_TWO_RING(RED_NUM,0,0);						
				}
				
				
//			wait_time++;
//			if(wait_time>chen1){Board_Order_info[BOARD_NUMBER].Board_Work=0,wait_time=0;}
			
			}
		}
	}
//		if(Board_Order_info[BOARD_NUMBER].Board_Work==0)flag_++;
//		if(flag_>200){Board_Order_info[BOARD_NUMBER].Board_Work=1,flag_=0;}
//		
  /* USER CODE END 3 */
  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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
