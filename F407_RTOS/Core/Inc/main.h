/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "CH423S.H"
#include "CH453.h"
#include "CH351.h"
#include "CH452.H"
#include "lcd.h"
#include "lcd_init.h"
#include "oled.h"
#include "key.h"
#include "RS485_work.h"
#include "queue_list.h" 
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;
extern SPI_HandleTypeDef hspi3;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart1;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
#define OPEN 1
#define OFF  0
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED1_Pin GPIO_PIN_3
#define LED1_GPIO_Port GPIOE
#define LED2_Pin GPIO_PIN_4
#define LED2_GPIO_Port GPIOE
#define LED3_Pin GPIO_PIN_5
#define LED3_GPIO_Port GPIOE
#define LED4_Pin GPIO_PIN_6
#define LED4_GPIO_Port GPIOE
#define RS485_RE_Pin GPIO_PIN_1
#define RS485_RE_GPIO_Port GPIOA
#define SWITCH1_Pin GPIO_PIN_12
#define SWITCH1_GPIO_Port GPIOF
#define SWITCH2_Pin GPIO_PIN_13
#define SWITCH2_GPIO_Port GPIOF
#define SWITCH3_Pin GPIO_PIN_14
#define SWITCH3_GPIO_Port GPIOF
#define SWITCH4_Pin GPIO_PIN_15
#define SWITCH4_GPIO_Port GPIOF
#define RS485_RE1_Pin GPIO_PIN_8
#define RS485_RE1_GPIO_Port GPIOC
#define CH351_INT_Pin GPIO_PIN_15
#define CH351_INT_GPIO_Port GPIOA
#define CH351_A0_Pin GPIO_PIN_10
#define CH351_A0_GPIO_Port GPIOC
#define CH351_A1_Pin GPIO_PIN_11
#define CH351_A1_GPIO_Port GPIOC
#define CH351_A2_Pin GPIO_PIN_12
#define CH351_A2_GPIO_Port GPIOC
#define CH351_D0_Pin GPIO_PIN_0
#define CH351_D0_GPIO_Port GPIOD
#define CH351_D1_Pin GPIO_PIN_1
#define CH351_D1_GPIO_Port GPIOD
#define CH351_D2_Pin GPIO_PIN_2
#define CH351_D2_GPIO_Port GPIOD
#define CH351_D3_Pin GPIO_PIN_3
#define CH351_D3_GPIO_Port GPIOD
#define CH351_D4_Pin GPIO_PIN_4
#define CH351_D4_GPIO_Port GPIOD
#define CH351_D5_Pin GPIO_PIN_5
#define CH351_D5_GPIO_Port GPIOD
#define CH351_D6_Pin GPIO_PIN_6
#define CH351_D6_GPIO_Port GPIOD
#define CH351_D7_Pin GPIO_PIN_7
#define CH351_D7_GPIO_Port GPIOD
#define CH351_RD_Pin GPIO_PIN_9
#define CH351_RD_GPIO_Port GPIOG
#define CH351_WR_Pin GPIO_PIN_10
#define CH351_WR_GPIO_Port GPIOG
#define CH351_CS_Pin GPIO_PIN_11
#define CH351_CS_GPIO_Port GPIOG
#define CH351_RST_Pin GPIO_PIN_12
#define CH351_RST_GPIO_Port GPIOG
#define CH351_CS2_Pin GPIO_PIN_13
#define CH351_CS2_GPIO_Port GPIOG
#define LCD_RST_Pin GPIO_PIN_14
#define LCD_RST_GPIO_Port GPIOG
#define CH351_CS3_Pin GPIO_PIN_15
#define CH351_CS3_GPIO_Port GPIOG
#define LCD_CS_Pin GPIO_PIN_6
#define LCD_CS_GPIO_Port GPIOB
#define LCD_DC_Pin GPIO_PIN_7
#define LCD_DC_GPIO_Port GPIOB
#define PE0_INT_Pin GPIO_PIN_0
#define PE0_INT_GPIO_Port GPIOE
#define PE1_KEY_Pin GPIO_PIN_1
#define PE1_KEY_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */
extern uint8_t CH423_20LED_1KEY;
extern uint8_t CH423_8LED_3KEY;
extern uint8_t CH453_12NUM_3LED_10KEY;
extern uint8_t CH423_6LED_5LED_2KEY;
extern uint8_t CH351_20LED_2KEY;
extern uint8_t LCD_CH351KEY_CH45210KEY; 
extern uint8_t CH351_24LED_2KEY;
extern uint8_t CH351_8LED_24KEY; 
extern uint8_t OLED1_3_9KEY;
extern uint8_t RS485_WORK_SLAVE; 
extern uint8_t DECEIVE_ID;
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
