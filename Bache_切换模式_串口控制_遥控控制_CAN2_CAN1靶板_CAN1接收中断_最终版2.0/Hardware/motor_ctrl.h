/*
 * motor_ctrl.h
 * 对应手册: 标准版电机控制器DBMCA2A3用户手册.pdf
 * 功能: 声明电机CAN控制相关的函数、变量和协议宏定义
 */

#ifndef __MOTOR_CTRL_H
#define __MOTOR_CTRL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"  // 必须包含，用于获取 CAN_HandleTypeDef 等 HAL 库定义

/* Macros --------------------------------------------------------------------*/

/* 1. CANopen SDO 协议指令 (参考手册 P19) */
#define CAN_CMD_READ            0x40  // 读指令 [cite: 475]
#define CAN_CMD_READ_SUCCESS    0x4B  // 读取成功返回 [cite: 482]
#define CAN_CMD_WRITE           0x2B  // 写指令 [cite: 453]
#define CAN_CMD_WRITE_SUCCESS   0x60  // 写入成功返回 [cite: 463]
#define CAN_CMD_FAIL            0x80  // 操作失败返回 [cite: 468]

/* 2. CAN ID 基址 (参考手册 P19) */
#define CAN_BASE_ID_TX          0x600 // 主机发送ID基址 (0x600 + NodeID) [cite: 451]
#define CAN_BASE_ID_RX          0x580 // 从机返回ID基址 (0x580 + NodeID) [cite: 458]

/* 3. 寄存器地址表 (参考手册 P22) */
#define REG_CONTROL_MODE        0x0103 // 控制模式和控制途径 [cite: 564]
#define REG_TARGET_DATA         0x0000 // 电机目标数据 [cite: 494]
#define REG_REAL_SPEED_A        0x0203 // A侧实际转速 [cite: 726]
#define REG_REAL_SPEED_B        0x0204 // B侧实际转速 [cite: 733]
#define REG_VOLTAGE             0x0200 // 电压 [cite: 704]
#define REG_ERROR_INFO          0x0206 // 故障信息 [cite: 749]

/* 4. 控制模式定义 (寄存器 0x0103 高字节) (参考手册 P22) */
#define MODE_OPEN_LOOP          0x00   // 速度开环模式 [cite: 565]
#define MODE_CLOSED_LOOP        0x01   // 速度闭环模式 [cite: 566]

/* 5. 控制途径定义 (寄存器 0x0103 低字节) (参考手册 P22-23) */
#define CTRL_PATH_ANALOG_DIR    0x00   // 单极性模拟量+正转反转 [cite: 567]
#define CTRL_PATH_RC_PWM        0x05   // RC-PWM单体模式 [cite: 573]
#define CTRL_PATH_RS485         0x09   // RS485控制 [cite: 577]
#define CTRL_PATH_CAN           0x0A   // CAN控制 [cite: 578]

/* Variables -----------------------------------------------------------------*/
/* * 声明外部变量，以便在 main.c 中可以直接访问接收到的数据
 * 这些变量需要在 .c 文件中进行定义 (即分配内存)
 */
extern uint8_t TxData[8];
extern uint8_t RxData[8];
extern CAN_TxHeaderTypeDef TxHeader;
extern CAN_RxHeaderTypeDef RxHeader;
extern uint32_t TxMailbox;

/* Function Prototypes -------------------------------------------------------*/

/**
  * @brief  配置 CAN2 过滤器 (必须调用，否则无法接收数据)
  */
//void CAN2_Filter_Config(void);

/**
  * @brief  查询电机当前的控制模式和控制途径 (寄存器 0x0103)
  * @param  node_id: 电机的节点ID (0-127)
  */
void Motor_Query_ControlMode(uint8_t node_id);

/**
  * @brief  解析接收到的控制模式数据
  * @note   应在接收成功后调用
  */
//void Motor_Parse_ControlMode(void);
uint8_t Motor_Parse_ControlMode(uint8_t *pData);
#ifdef __cplusplus
}
#endif

#endif /* __MOTOR_CTRL_H */
