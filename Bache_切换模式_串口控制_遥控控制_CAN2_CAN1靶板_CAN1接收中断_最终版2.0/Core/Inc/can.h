/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.h
  * @brief   This file contains all the function prototypes for
  *          the can.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#ifndef __CAN_H__
#define __CAN_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern CAN_HandleTypeDef hcan2;
extern CAN_HandleTypeDef hcan1;
uint8_t CANx_ReceiveStdData(CAN_HandleTypeDef *hcan, uint32_t *pStdId, uint8_t *pData, uint8_t *pLen);
uint8_t CANx_SendStdData(CAN_HandleTypeDef *hcan, uint32_t std_id, uint8_t *pData);

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_CAN2_Init(void);
void MX_CAN1_Init(void);
/* can.h */
void CAN1_Filter_Config(void);
void CAN2_Filter_Config(void); // 你原本可能已经有了

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __CAN_H__ */

