/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "can.h"
#include "usb_otg.h"
#include "gpio.h"
#include "motor_ctrl.h"
#include "usart.h"

#include <stdio.h>   // 必须加：用于 sprintf
#include <string.h>  // 必须加：用于 strlen

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define CMD_TIMEOUT_MS  200  // 超时时间：200毫秒没收到数据就停车
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
// CAN1 接收缓冲 (用于中断和主循环共享数据)
volatile uint8_t  CAN1_Rx_Flag = 0;       // 收到数据标志位
uint32_t CAN1_Rx_ID_Buf;                  // 存 ID
uint8_t  CAN1_Rx_Data_Buf[8];             // 存数据
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t Can_Tx_Data[8] = {0x40, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t Can_Tx_Data_Motor[8] = {0};
uint32_t Rx_id;       // 用来存“是谁发的”
uint8_t Can_Rx_Data[8];  // 用来存“发了什么”
uint8_t  Rx_len;      // 用来存“发了多长”


// 定义电机最大转速 (RPM)
// 根据刚才查到的寄存器 0x0105 的值，您的控制器当前设定是 3000
#define MAX_MOTOR_RPM   3000.0f
#define WHEEL_TRACK_M    0.59f * 1.2f   // 轮距 (米)
#define WHEEL_RADIUS_M   0.12f  // 轮半径 (米)
#define GEAR_RATIO       19.0f   // 减速比
//// m/s 转换成 RPM 的系数
#define MPS_TO_RPM       ((60.0f * GEAR_RATIO) / (2.0f * 3.14159f * WHEEL_RADIUS_M))

// 接收相关变量
uint8_t pc_rx_buffer;           // 1字节临时接收缓存
uint8_t pc_rx_state = 0;        // 状态机状态
uint8_t pc_rx_count = 0;        // 数据计数
uint8_t pc_rx_data_buf[9];      // 包缓存 (Vx:4 + Az:4 + Sum:1)

uint8_t led_state = 0;

// 联合体：用于字节与浮点数互转
typedef union
{
    float value;
    uint8_t bytes[4];
} float_union_t;

// 解析后的目标速度 (全局变量，供底盘任务读取)
volatile float target_vx = 0.0f; // 线速度 m/s
volatile float target_az = 0.0f; // 角速度 rad/s
volatile uint32_t last_pc_cmd_time = 0; // 看门狗时间戳
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
    MX_CAN2_Init();
    MX_CAN1_Init();
    MX_USART6_UART_Init();
    //MX_USB_OTG_FS_PCD_Init();
    CAN1_Filter_Config();
    CAN2_Filter_Config();
    /* USER CODE BEGIN 2 */
    // --- 新增：开启 USART6 接收中断 ---
    // 注意：必须先在 CubeMX 里启用 USART6 并生成代码，这里才会有 huart6
    HAL_UART_Receive_IT(&huart6, &pc_rx_buffer, 1);

    // ============== 必须添加这两行 ==============
    if (HAL_CAN_Start(&hcan2) != HAL_OK)
    {
        Error_Handler(); // 如果启动失败，进入错误处理
    }

    if (HAL_CAN_Start(&hcan1) != HAL_OK)
    {
        Error_Handler(); // 如果启动失败，进入错误处理
    }

    // 【新增】开启 CAN1 的 FIFO0 消息挂起中断
    // 这告诉 HAL 库：一旦 FIFO0 有数据，就调用 HAL_CAN_RxFifo0MsgPendingCallback
    if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
    {
        Error_Handler();
    }

    /* USER CODE END 2 */
    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    uint32_t query_timer = 0; // <--- 补上这一行

    while (1)
    {
        /* USER CODE END WHILE */
        // 1. 【高频执行】非阻塞接收 CAN 数据 (检查有没有人给我回信)
        // ============================================================
        // 这里的关键是：如果没收到数据，马上走，不要等！
        // 假设 CANx_ReceiveStdData 内部是非阻塞的 (即 Rx_len=0 时立即返回)
        if (CANx_ReceiveStdData(&hcan2, &Rx_id, Can_Rx_Data, &Rx_len) == 0)
        {
            // 收到数据了！检查是不是从控制器回来的 (0x580 + ID)
            if (Rx_id == (CAN_BASE_ID_RX + 0))
            {
                // 解析控制模式
                uint8_t mode_res = Motor_Parse_ControlMode(Can_Rx_Data);

                // 更新状态和 LED
                if (mode_res == 2)
                {
                    led_state = 2; // 开环 RCPWM
                    HAL_GPIO_WritePin(GPIOE, LED_R_TEST_Pin, GPIO_PIN_RESET);
                    HAL_GPIO_WritePin(GPIOF, LED_G_TEST_Pin, GPIO_PIN_SET);
									HAL_GPIO_WritePin(GPIOH, GPIO_PIN_11, GPIO_PIN_RESET);   
									HAL_GPIO_WritePin(GPIOH, GPIO_PIN_12, GPIO_PIN_SET); 
                }
                else if (mode_res == 3)
                {
                    led_state = 3; // 闭环 CAN
                    HAL_GPIO_WritePin(GPIOF, LED_G_TEST_Pin, GPIO_PIN_RESET);
                    HAL_GPIO_WritePin(GPIOE, LED_R_TEST_Pin, GPIO_PIN_SET);
									HAL_GPIO_WritePin(GPIOH, GPIO_PIN_12, GPIO_PIN_RESET);   
									HAL_GPIO_WritePin(GPIOH, GPIO_PIN_11, GPIO_PIN_SET); 
                }
            }
        }

        // ============================================================
        // 2. 【新增】CAN1 接收 (传感器/底盘等)
        // ============================================================
        if (CAN1_Rx_Flag == 1)
        {
            CAN1_Rx_Flag = 0; // 清除标志
            // 定义一个临时缓冲区用于存放打印的字符串 (64字节足够存一行)
            char uart_buf[64];

            if (CAN1_Rx_ID_Buf >= 0x01FC && CAN1_Rx_ID_Buf <= 0x01FF)
            {
                // 根据 ID 修改数据的最后一位 (复刻你之前的逻辑)
                if (CAN1_Rx_ID_Buf == 0x01FC)
                {
                    CAN1_Rx_Data_Buf[7] = 0xFC;
                }
                else if (CAN1_Rx_ID_Buf == 0x01FD)
                {
                    CAN1_Rx_Data_Buf[7] = 0xFD;
                }
                else if (CAN1_Rx_ID_Buf == 0x01FE)
                {
                    CAN1_Rx_Data_Buf[7] = 0xFE;
                }
                else if (CAN1_Rx_ID_Buf == 0x01FF)
                {
                    CAN1_Rx_Data_Buf[7] = 0xFF;
                }

                // 格式化打印字符串: "FC: 01 02 ... \r\n"
                // 技巧：rx_msg.id & 0xFF 可以取出最后两位，自动匹配 FC/FD/FE/FF
                sprintf(uart_buf, "%02X: %02X %02X %02X %02X %02X %02X %02X %02X\r\n",
                        (uint8_t)(CAN1_Rx_ID_Buf & 0xFF), // 打印 ID 的低8位作为前缀
                        CAN1_Rx_Data_Buf[0], CAN1_Rx_Data_Buf[1], CAN1_Rx_Data_Buf[2], CAN1_Rx_Data_Buf[3],
                        CAN1_Rx_Data_Buf[4], CAN1_Rx_Data_Buf[5], CAN1_Rx_Data_Buf[6], CAN1_Rx_Data_Buf[7]);
                // 通过串口 6 发送 (非中断发送即可，因为在任务里)
                HAL_UART_Transmit(&huart6, (uint8_t *)uart_buf, strlen(uart_buf), 10);
            } 
        }

        // ============================================================
        // 3. 【低频执行】每隔 1000ms 发送一次查询指令
        // ============================================================
        if (HAL_GetTick() - query_timer > 1000)
        {
            query_timer = HAL_GetTick(); // 更新计时器
            // 发送查询指令 (只负责发，发完就走，不等待回复)
            Can_Tx_Data[0] = 0x40; // 读指令
            Can_Tx_Data[1] = 0x03; // 寄存器 0x0103
            Can_Tx_Data[2] = 0x01;
            // ... 其他数据保持不变 ...
            CANx_SendStdData(&hcan2, 0x600, Can_Tx_Data);
        }

        // ============================================================
        // ★★★ 新增：安全看门狗 (防止串口断开后撞车) ★★★
        // ============================================================
        // 只有在 "CAN闭环模式" (led_state == 3) 下才检查串口超时
        // 如果是按键控制或者 RCPWM 模式，就不管串口
        Mode_change_2(&led_state);

        if (led_state == 3)
        {
            // 检查：当前时间 - 上次收到命令的时间 > 200ms ?
            if ((HAL_GetTick() - last_pc_cmd_time) > CMD_TIMEOUT_MS)
            {
                // 1. 强制归零全局变量 (为了逻辑统一)
                target_vx = 0.0f;
                target_az = 0.0f;
                uint8_t Stop_Cmd[8];
                // 2. 【关键】必须手动发一包 CAN 数据让车停下来！
                // 因为平时 CAN 发送是在串口中断里触发的，
                // 现在串口没数据了，中断进不去，必须在这里补发“停车指令”。
                Stop_Cmd[0] = 0x2B;
                Stop_Cmd[1] = 0x00;
                Stop_Cmd[2] = 0x00;
                Stop_Cmd[3] = 0x00;
                Stop_Cmd[4] = 0x00; // 左轮 0%
                Stop_Cmd[5] = 0x00; // 右轮 0%
                Stop_Cmd[6] = 0x00;
                Stop_Cmd[7] = 0x00;
                // 发送给控制器
                CANx_SendStdData(&hcan2, 0x600, Stop_Cmd);
                // 可选：亮个红灯提示“失联”
                // HAL_GPIO_WritePin(GPIOE, LED_R_TEST_Pin, GPIO_PIN_RESET);
            }
        }

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
    RCC_OscInitStruct.PLL.PLLM = 6;
    RCC_OscInitStruct.PLL.PLLN = 168;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
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
// 定义两个模式对应的寄存器值
// 根据手册 Page 13-14:
// 0x010A = 速度闭环(01) + CAN控制(0A)
// 0x0005 = 速度开环(00) + RC-PWM单体(05)
uint8_t Mode_change_2(uint8_t *current_state)
{
    static uint8_t key_up = 1; // 按键松开标志位 (1:松开, 0:按下)

    // 读取按键状态 (假设低电平有效，即按下为 RESET)
    if (HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == GPIO_PIN_RESET)
    {
        HAL_Delay(20); // 简单的消抖 (20ms对于人手按下是可以接受的阻塞)

        if (HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == GPIO_PIN_RESET)
        {
            // 只有当按键之前是“松开”状态，现在是“按下”状态时，才执行
            if (key_up == 1)
            {
                key_up = 0; // 标记为已按下，防止一直进入这里
                // === 切换逻辑开始 ===
                // 准备 CAN 数据包
                // 写入寄存器 0x0103 (控制模式和途径)
                // 格式: Cmd(0x2B) AddrL(0x03) AddrH(0x01) Sub(0x00) ValL ValH 00 00
                Can_Tx_Data_Motor[0] = 0x2B;
                Can_Tx_Data_Motor[1] = 0x03; // 0x0103 低位
                Can_Tx_Data_Motor[2] = 0x01; // 0x0103 高位
                Can_Tx_Data_Motor[3] = 0x00;

                if (*current_state == 3)
                {
                    // 当前是闭环CAN(3)，切换到 -> 开环RC(2)
                    *current_state = 2;
                    // 目标值: 0x0005 (开环 + RC-PWM)
                    Can_Tx_Data_Motor[4] = 0x05;
                    Can_Tx_Data_Motor[5] = 0x00;
                    // 切换灯光指示
                    HAL_GPIO_WritePin(GPIOE, LED_R_TEST_Pin, GPIO_PIN_RESET); // 红灯亮
                    HAL_GPIO_WritePin(GPIOF, LED_G_TEST_Pin, GPIO_PIN_SET);   // 绿灯灭
									HAL_GPIO_WritePin(GPIOH, GPIO_PIN_11, GPIO_PIN_RESET);   
									HAL_GPIO_WritePin(GPIOH, GPIO_PIN_12, GPIO_PIN_SET);  
										
                }
                else
                {
                    // 当前是其他模式，切换到 -> 闭环CAN(3)
                    *current_state = 3;
                    // 目标值: 0x010A (闭环 + CAN)
                    Can_Tx_Data_Motor[4] = 0x0A;
                    Can_Tx_Data_Motor[5] = 0x01;
                    // 切换灯光指示
                    HAL_GPIO_WritePin(GPIOF, LED_G_TEST_Pin, GPIO_PIN_RESET); // 绿灯亮
                    HAL_GPIO_WritePin(GPIOE, LED_R_TEST_Pin, GPIO_PIN_SET);   // 红灯灭
									
									HAL_GPIO_WritePin(GPIOH, GPIO_PIN_12, GPIO_PIN_RESET);   
									HAL_GPIO_WritePin(GPIOH, GPIO_PIN_11, GPIO_PIN_SET); 
                }

                Can_Tx_Data_Motor[6] = 0x00;
                Can_Tx_Data_Motor[7] = 0x00;
                // 发送 CAN 指令
                CANx_SendStdData(&hcan2, 0x600, Can_Tx_Data_Motor);
                // 注意：这里不要死等回复！
                // 因为按键按下是用户交互，先让灯变色，指令发出去即可。
                // 如果死等回复，会感觉按键“卡顿”。
                return 1; // 返回 1 表示发生了切换
            }
						
        }
    }
    else
    {
        key_up = 1; // 按键已松开，准备好下一次触发
    }

    return 0; // 无切换
}
// --- 新增：串口中断回调函数 ---
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    // 判断是否是上位机串口 (USART6)
    if (huart->Instance == USART6)
    {
        // 状态机解析协议: Head1(0xAA) Head2(0x55) Vx(4B) Az(4B) Sum(1B)
        switch (pc_rx_state)
        {
        case 0: // 找帧头 0xAA
            if (pc_rx_buffer == 0xAA)
            {
                pc_rx_state = 1;
            }
            else
            {
                pc_rx_state = 0;
            }

            break;

        case 1: // 找帧头 0x55
            if (pc_rx_buffer == 0x55)
            {
                pc_rx_state = 2;
                pc_rx_count = 0;
            }
            else
            {
                pc_rx_state = 0;
            }

            break;

        case 2: // 接收 9 字节数据
            pc_rx_data_buf[pc_rx_count++] = pc_rx_buffer;

            if (pc_rx_count >= 9)
            {
                pc_rx_state = 3;
            }

            break;

        case 3: // 校验
        {
            uint8_t checksum = 0;

            for (int i = 0; i < 8; i++)
            {
                checksum += pc_rx_data_buf[i];
            }

            if (checksum == pc_rx_data_buf[8])
            {
                float_union_t v_union, w_union;
                // 提取 Vx
                v_union.bytes[0] = pc_rx_data_buf[0];
                v_union.bytes[1] = pc_rx_data_buf[1];
                v_union.bytes[2] = pc_rx_data_buf[2];
                v_union.bytes[3] = pc_rx_data_buf[3];
                // 提取 Az
                w_union.bytes[0] = pc_rx_data_buf[4];
                w_union.bytes[1] = pc_rx_data_buf[5];
                w_union.bytes[2] = pc_rx_data_buf[6];
                w_union.bytes[3] = pc_rx_data_buf[7];
                // 【核心修改】：只更新全局变量，不要在这里计算PID或发送CAN
                target_vx = v_union.value;
                target_az = w_union.value;
                //char tx_buf1[64];
                // %.2f 表示保留2位小数
                // sprintf 会把数值转换成 ASCII 码填入 tx_buf
                //sprintf(tx_buf1, "Vx: %.2f, Az: %.2f\r\n", target_vx, target_az);
                // 发送 tx_buf 的内容
                // strlen(tx_buf) 会自动计算字符串长度
                // HAL_UART_Transmit(&huart6, (uint8_t *)tx_buf1, strlen(tx_buf1), 20);
                // ========================================================
                // 1. 【第一步】先把 RPM 算出来 (必须放在 sprintf 之前！)
                // ========================================================
                // 确保您在 main.h 或开头定义了 WHEEL_TRACK_M (轮距) 和 MPS_TO_RPM (转换系数)
                float target_rpm_l = (target_vx - target_az * WHEEL_TRACK_M / 2.0f) * MPS_TO_RPM;
                float target_rpm_r = (target_vx + target_az * WHEEL_TRACK_M / 2.0f) * MPS_TO_RPM;
                // ========================================================
                // 2. 【第二步】修改 sprintf 内容，打印 RPM
                // ========================================================
                char tx_buf2[64];
                // 修改这里：把格式改成打印 RPM_L 和 RPM_R
                sprintf(tx_buf2, "RPM_L: %.2f, RPM_R: %.2f\r\n", target_rpm_l, target_rpm_r);
                // 发送数据
                HAL_UART_Transmit(&huart6, (uint8_t *)tx_buf2, strlen(tx_buf2), 20);
                // 更新看门狗时间戳
                // ========================================================
                // 3. 【新增】计算百分比并转换为 int8_t (即控制器的 Hex 格式)
                // ========================================================
                // 3.1 计算相对于最大转速的百分比 (结果范围可能是 -200.0 到 +200.0)
                float percent_l = (target_rpm_l / MAX_MOTOR_RPM) * 100.0f;
                float percent_r = (target_rpm_r / MAX_MOTOR_RPM) * 100.0f;

                // 3.2 限幅保护 (非常重要！)
                // 因为 int8_t 只能存 -128 到 +127。
                // 如果计算出 150%，强转会溢出变成负数，导致电机乱转。必须限制在 -100 到 +100 之间。
                if (percent_l > 100.0f)
                {
                    percent_l = 100.0f;
                }

                if (percent_l < -100.0f)
                {
                    percent_l = -100.0f;
                }

                if (percent_r > 100.0f)
                {
                    percent_r = 100.0f;
                }

                if (percent_r < -100.0f)
                {
                    percent_r = -100.0f;
                }

                // 3.3 强制转换为 int8_t (这就已经是您要的 Hex 数据了)
                // 例如：50.0 -> 50 (0x32), -50.0 -> -50 (0xCE)
                int8_t out_cmd_l = (int8_t)percent_l;
                int8_t out_cmd_r = (int8_t)percent_r;
                // ========================================================
                // 【新增】串口打印十六进制调试信息
                // ========================================================
                //char hex_debug_buf[64]; // 定义一个临时缓冲区
                // 格式化说明：
                // %d   -> 打印十进制 (方便看是正转还是反转，比如 -50)
                // 0x   -> 只是个前缀字符串，没特殊含义
                // %02X -> 打印两位十六进制 (比如 CE)。如果不加 (uint8_t) 强转，-50 会变成 FFFFFFCE
                //                sprintf(hex_debug_buf, "Hex: L=0x%02X (%d%%), R=0x%02X (%d%%)\r\n",
                //                        (uint8_t)out_cmd_l, out_cmd_l,
                //                        (uint8_t)out_cmd_r, out_cmd_r);
                // 发送出去
                //HAL_UART_Transmit(&huart6, (uint8_t *)hex_debug_buf, strlen(hex_debug_buf), 20);
                Can_Tx_Data_Motor[0] = 0x2B;
                Can_Tx_Data_Motor[1] = 0x00;
                Can_Tx_Data_Motor[2] = 0x00;
                Can_Tx_Data_Motor[3] = 0x00;
                Can_Tx_Data_Motor[4] = (uint8_t)out_cmd_l;
                Can_Tx_Data_Motor[5] = (uint8_t)out_cmd_r;
                Can_Tx_Data_Motor[6] = 0x00;
                Can_Tx_Data_Motor[7] = 0x00;
                // 然后调用发送函数
                CANx_SendStdData(&hcan2, 0x600, Can_Tx_Data_Motor);
                last_pc_cmd_time = HAL_GetTick();
            }

            pc_rx_state = 0;
        }
        break;

        default:
            pc_rx_state = 0;
            break;
        }

        HAL_UART_Receive_IT(&huart6, &pc_rx_buffer, 1);
    }
}
/* USER CODE BEGIN 4 */
// CAN 接收回调函数 (当 FIFO0 有数据时自动被调用)
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef RxHeader;
    uint8_t RxData[8];

    // 判断是哪个 CAN 进来的 (CAN1)
    if (hcan->Instance == CAN1)
    {
        // 从 FIFO0 读取数据
        if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK)
        {
            // 把数据搬运到全局变量，让主循环去处理
            // 这样中断处理非常快，几微秒就结束了
            CAN1_Rx_ID_Buf = RxHeader.StdId;
            memcpy(CAN1_Rx_Data_Buf, RxData, 8); // 需包含 string.h
            CAN1_Rx_Flag = 1; // 告诉主循环：有新货到了！
        }
    }

    // 如果您想把 CAN2 也改成中断，可以在这里加 else if (hcan->Instance == CAN2) ...
}
/* USER CODE END 4 */
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
#ifdef USE_FULL_ASSERT
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
