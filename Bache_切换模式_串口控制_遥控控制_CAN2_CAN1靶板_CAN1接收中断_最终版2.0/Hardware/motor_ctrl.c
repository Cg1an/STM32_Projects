/*
 * motor_ctrl.c
 * 功能: 实现电机CAN控制相关的发送、接收和解析功能
 */

/* Includes ------------------------------------------------------------------*/
#include "motor_ctrl.h"
#include "gpio.h"
#include <stdio.h> // 用于printf调试（如不需要可移除）

/* Variables -----------------------------------------------------------------*/
// 引用 main.c 或 can.c 中定义的 hcan2
extern CAN_HandleTypeDef hcan2;

// 定义全局缓冲区 (对应头文件中的 extern 声明)


/* Functions -----------------------------------------------------------------*/

/**
  * @brief 配置 CAN2 过滤器 (必须配置才能接收数据)
  */
//void CAN2_Filter_Config(void)
//{
//    CAN_FilterTypeDef  sFilterConfig;

//    // STM32F4中 CAN2 的过滤器通常由 CAN1 管理 (从 Bank 14 开始)
//    sFilterConfig.FilterBank = 14; 
//    sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
//    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
//    
//    // 设置为 0 表示接收总线上所有 ID (为了调试方便)
//    // 实际项目中建议根据 CAN_BASE_ID_RX + NodeID 进行过滤
//    sFilterConfig.FilterIdHigh = 0x0000;
//    sFilterConfig.FilterIdLow = 0x0000;
//    sFilterConfig.FilterMaskIdHigh = 0x0000;
//    sFilterConfig.FilterMaskIdLow = 0x0000;
//    
//    sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
//    sFilterConfig.FilterActivation = ENABLE;
//    sFilterConfig.SlaveStartFilterBank = 14;

//    if (HAL_CAN_ConfigFilter(&hcan2, &sFilterConfig) != HAL_OK)
//    {
//        Error_Handler();
//    }
//}

///**
//  * @brief 查询电机控制模式和途径
//  * @note  发送 CAN SDO 读指令读取 0x0103 寄存器
//  * @param node_id: 设备节点 ID
//  */
//void Motor_Query_ControlMode(uint8_t node_id)
//{
//    // 1. 配置发送帧头
//    // 使用宏定义 CAN_BASE_ID_TX (0x600) + 节点ID
//    TxHeader.StdId = CAN_BASE_ID_TX + node_id; 
//    TxHeader.RTR = CAN_RTR_DATA;
//    TxHeader.IDE = CAN_ID_STD;
//    TxHeader.DLC = 8;
//    
//    // 2. 填充发送数据 (SDO 读协议)
//    // 指令: 0x40 (CAN_CMD_READ)
//    // 寄存器: REG_CONTROL_MODE (0x0103)
//    TxData[0] = CAN_CMD_READ;       
//    TxData[1] = (uint8_t)(REG_CONTROL_MODE & 0xFF);      // 低字节 0x03
//    TxData[2] = (uint8_t)((REG_CONTROL_MODE >> 8) & 0xFF); // 高字节 0x01
//    TxData[3] = 0x00;               // 子索引
//    TxData[4] = 0x00;               // 数据位填充 0
//    TxData[5] = 0x00;
//    TxData[6] = 0x00;
//    TxData[7] = 0x00;

//    // 3. 发送数据
//    if (HAL_CAN_AddTxMessage(&hcan2, &TxHeader, TxData, &TxMailbox) != HAL_OK)
//    {
//        // 如果这里进来了，说明发送邮箱满了，或者 CAN 有错误
//        // 可以在这里打个断点查看 hcan2.ErrorCode
//        Error_Handler(); 
//    }
//    
//    // 4. 等待并接收数据
//    uint32_t timeout = HAL_GetTick();
//    
//    // 注意：如果波特率不对，或者没接终端电阻，这里也会一直为 0
//    while (HAL_CAN_GetRxFifoFillLevel(&hcan2, CAN_RX_FIFO0) == 0)
//    {
//        if ((HAL_GetTick() - timeout) > 50) 
//        {
//            // 超时退出。建议在这里打断点，看是"根本没发出去"还是"发了没回"
//            return; 
//        }
//    }
//    // 5. 读取数据到 RxData 缓冲区
//    if (HAL_CAN_GetRxMessage(&hcan2, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK)
//    {
//        // 校验返回 ID: 应该是 0x580 + node_id
//        if (RxHeader.StdId == (CAN_BASE_ID_RX + node_id))
//        {
//            // 如果读取成功，调用解析函数
//            //Motor_Parse_ControlMode();
//        }
//    }
//}

/**
  * @brief  解析接收到的控制模式数据
  * @note   根据 RxData 的内容判断当前状态
  */
uint8_t Motor_Parse_ControlMode(uint8_t *pData)
{
    // 1. 检查操作码是否为 读取成功 (0x4B)
    if ((pData[0] != CAN_CMD_READ_SUCCESS)&&(pData[0]!= CAN_CMD_WRITE_SUCCESS))
    {
        // 读取失败或错误帧
        return 1; 
    }

    // 2. 再次确认寄存器地址是否匹配 (0x03 0x01)
    if (pData[1] != (uint8_t)(REG_CONTROL_MODE & 0xFF) || 
        pData[2] != (uint8_t)((REG_CONTROL_MODE >> 8) & 0xFF))
    {
        return 1;
    }

    // 3. 解析具体数据
    // 根据手册 P22: Data5 (RxData[4]) 为控制途径, Data6 (RxData[5]) 为控制模式
    uint8_t current_path = pData[4]; 
    uint8_t current_mode = pData[5];
    if ((current_mode == MODE_OPEN_LOOP) && (current_path == CTRL_PATH_RC_PWM ))
    {
//				che_state = 1;
			  return 2;
    }else if ((current_mode == MODE_CLOSED_LOOP) && (current_path == CTRL_PATH_CAN))
		{
//				che_state = 2;
			  return 3;  
		}
}

