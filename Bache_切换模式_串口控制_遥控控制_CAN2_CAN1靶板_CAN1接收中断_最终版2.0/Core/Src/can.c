/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.c
  * @brief   This file provides code for the configuration
  *          of the CAN instances.
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
/* Includes ------------------------------------------------------------------*/
#include "can.h"

/* USER CODE BEGIN 0 */
// 定义一个8字节的数组用于存放发送数据
CAN_RxHeaderTypeDef RxHeader;


/* USER CODE END 0 */

CAN_HandleTypeDef hcan1;
CAN_HandleTypeDef hcan2;

/* CAN2 init function */
/**
 * @brief  尝试接收标准帧数据 (从 FIFO0 读取)
 * @param  hcan:    CAN句柄指针 (例如 &hcan2)
 * @param  pStdId:  [输出] 用于存放读取到的 ID 的变量指针
 * @param  pData:   [输出] 用于存放数据的数组指针 (确保至少8字节)
 * @param  pLen:    [输出] 用于存放数据长度的变量指针
 * @retval 0:成功读到数据, 1:没有数据或读取失败
 */
uint8_t CANx_ReceiveStdData(CAN_HandleTypeDef *hcan, uint32_t *pStdId, uint8_t *pData, uint8_t *pLen)
{
    
    // 1. 检查 FIFO0 里面有没有信 (Message Pending?)
    // 如果 FillLevel 为 0，说明空空如也，直接返回 1
    if (HAL_CAN_GetRxFifoFillLevel(hcan, CAN_RX_FIFO0) == 0)
    {
        return 1; // 没数据，返回失败/空闲
    }

    // 2. 尝试从 FIFO0 拿出一封信
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, pData) != HAL_OK)
    {
        return 1; // 硬件出错
    }

    // 3. 把信封上的信息 (ID 和 长度) 传出去
    *pStdId = RxHeader.StdId; // 把 ID 赋值给外部变量
    *pLen = RxHeader.DLC;     // 把长度赋值给外部变量
//		Motor_Parse_ControlMode();

    return 0; // 成功拿到数据
}

/**
 * @brief  发送标准帧数据 (Standard ID Data Frame)
 * @param  hcan:   CAN句柄指针 (例如 &hcan1 或 &hcan2)
 * @param  std_id: 目标ID (例如 0x600)
 * @param  pData:  要发送的数据数组指针 (uint8_t data[8])
 * @param  len:    数据长度 (0-8)
 * @retval 0:成功, 1:失败
 */
uint8_t CANx_SendStdData(CAN_HandleTypeDef *hcan, uint32_t std_id, uint8_t *pData)
{
    CAN_TxHeaderTypeDef tx_header;
    uint32_t used_mailbox;

    // 1. 配置报文头 (信封)
    tx_header.StdId = std_id;         // 标准ID
    tx_header.ExtId = 0;              // 扩展ID (标准帧不用)
    tx_header.IDE = CAN_ID_STD;       // 使用标准帧
    tx_header.RTR = CAN_RTR_DATA;     // 数据帧
    tx_header.DLC = 8;              // 数据长度 (比如 8)
    tx_header.TransmitGlobalTime = DISABLE;

   
    // 3. 尝试添加到发送邮箱
    // 如果返回 HAL_OK，说明发送请求已成功交给硬件
    if (HAL_CAN_AddTxMessage(hcan, &tx_header, pData, &used_mailbox) != HAL_OK)
    {
        return 1; // 发送失败 (通常是因为邮箱满了)
    }

    return 0; // 发送成功
}


/* CAN1 init function */
void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 3;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_9TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_4TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = ENABLE;
  hcan1.Init.AutoWakeUp = ENABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */

  /* USER CODE END CAN1_Init 2 */

}

/**
  * @brief  配置 CAN1 过滤器 (接收所有数据)
  * 注意：CAN1 和 CAN2 共享过滤器组。
  * CAN2 配置了 SlaveStartFilterBank=14，所以 CAN1 只能用 0~13。
  */
void CAN1_Filter_Config(void)
{
    CAN_FilterTypeDef  sFilterConfig;

    sFilterConfig.FilterBank = 0;                       // CAN1 使用 0 号过滤器 (范围 0-13)
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;   // 掩码模式
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;  // 32位宽

    // Mask 和 Id 全为 0，表示“不过滤，接收所有消息”
    // 如果只想接收特定 ID，需要修改这里
    sFilterConfig.FilterIdHigh = 0x0000;
    sFilterConfig.FilterIdLow = 0x0000;
    sFilterConfig.FilterMaskIdHigh = 0x0000;
    sFilterConfig.FilterMaskIdLow = 0x0000;

    sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;  // 接收到的报文放入 FIFO0
    sFilterConfig.FilterActivation = ENABLE;            // 激活过滤器
    
    // 重要：这个参数设置 CAN1 和 CAN2 的分界线
    // 必须保持为 14，与 CAN2 的配置一致，否则会打架
    sFilterConfig.SlaveStartFilterBank = 14;            

    if (HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
    {
        Error_Handler();
    }
}

/* CAN2 init function */
void MX_CAN2_Init(void)
{

  /* USER CODE BEGIN CAN2_Init 0 */

  /* USER CODE END CAN2_Init 0 */

  /* USER CODE BEGIN CAN2_Init 1 */

  /* USER CODE END CAN2_Init 1 */
  hcan2.Instance = CAN2;
  hcan2.Init.Prescaler = 21;
  hcan2.Init.Mode = CAN_MODE_NORMAL;
  hcan2.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan2.Init.TimeSeg1 = CAN_BS1_5TQ;
  hcan2.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan2.Init.TimeTriggeredMode = DISABLE;
  hcan2.Init.AutoBusOff = ENABLE;
  hcan2.Init.AutoWakeUp = DISABLE;
  hcan2.Init.AutoRetransmission = ENABLE;
  hcan2.Init.ReceiveFifoLocked = DISABLE;
  hcan2.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN2_Init 2 */

  /* USER CODE END CAN2_Init 2 */

}

void CAN2_Filter_Config(void)
{
    CAN_FilterTypeDef  sFilterConfig;

    // STM32F4中 CAN2 的过滤器通常由 CAN1 管理 (从 Bank 14 开始)
    sFilterConfig.FilterBank = 14; 
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
    
    // 设置为 0 表示接收总线上所有 ID (为了调试方便)
    // 实际项目中建议根据 CAN_BASE_ID_RX + NodeID 进行过滤
    sFilterConfig.FilterIdHigh = 0x0000;
    sFilterConfig.FilterIdLow = 0x0000;
    sFilterConfig.FilterMaskIdHigh = 0x0000;
    sFilterConfig.FilterMaskIdLow = 0x0000;
    
    sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
    sFilterConfig.FilterActivation = ENABLE;
    sFilterConfig.SlaveStartFilterBank = 14;

    if (HAL_CAN_ConfigFilter(&hcan2, &sFilterConfig) != HAL_OK)
    {
        Error_Handler();
    }
}
static uint32_t HAL_RCC_CAN1_CLK_ENABLED=0;

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    HAL_RCC_CAN1_CLK_ENABLED++;
    if(HAL_RCC_CAN1_CLK_ENABLED==1){
      __HAL_RCC_CAN1_CLK_ENABLE();
    }

    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**CAN1 GPIO Configuration
    PD0     ------> CAN1_RX
    PD1     ------> CAN1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* USER CODE BEGIN CAN1_MspInit 1 */
		HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 6, 0); 
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
  /* USER CODE END CAN1_MspInit 1 */
  }
  else if(canHandle->Instance==CAN2)
  {
  /* USER CODE BEGIN CAN2_MspInit 0 */

  /* USER CODE END CAN2_MspInit 0 */
    /* CAN2 clock enable */
    __HAL_RCC_CAN2_CLK_ENABLE();
    HAL_RCC_CAN1_CLK_ENABLED++;
    if(HAL_RCC_CAN1_CLK_ENABLED==1){
      __HAL_RCC_CAN1_CLK_ENABLE();
    }

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**CAN2 GPIO Configuration
    PB12     ------> CAN2_RX
    PB13     ------> CAN2_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN CAN2_MspInit 1 */

  /* USER CODE END CAN2_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    HAL_RCC_CAN1_CLK_ENABLED--;
    if(HAL_RCC_CAN1_CLK_ENABLED==0){
      __HAL_RCC_CAN1_CLK_DISABLE();
    }

    /**CAN1 GPIO Configuration
    PD0     ------> CAN1_RX
    PD1     ------> CAN1_TX
    */
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_0|GPIO_PIN_1);

  /* USER CODE BEGIN CAN1_MspDeInit 1 */
		HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
  /* USER CODE END CAN1_MspDeInit 1 */
  }
  else if(canHandle->Instance==CAN2)
  {
  /* USER CODE BEGIN CAN2_MspDeInit 0 */

  /* USER CODE END CAN2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN2_CLK_DISABLE();
    HAL_RCC_CAN1_CLK_ENABLED--;
    if(HAL_RCC_CAN1_CLK_ENABLED==0){
      __HAL_RCC_CAN1_CLK_DISABLE();
    }

    /**CAN2 GPIO Configuration
    PB12     ------> CAN2_RX
    PB13     ------> CAN2_TX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_12|GPIO_PIN_13);

  /* USER CODE BEGIN CAN2_MspDeInit 1 */

  /* USER CODE END CAN2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
