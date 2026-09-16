/**
  ****************************(C) COPYRIGHT 2022 DJI****************************
  * @file       can_receive.c/h
  * @brief      there is CAN interrupt function  to receive motor data,
  *             and CAN send function to send motor current to control motor.
  *             这里是CAN中断接收函数，接收电机数据,CAN发送函数发送电机电流控制电机.
  * @note       
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0          RM              1. done
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2022 DJI****************************
  */

#include "CAN_receive.h"
#include "main.h"

//CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan;

uint8_t can1_rx_data[8];

CAN_RxHeaderTypeDef hcan1RxFrame;

Board_Can_info_t Board_Can_info;

Board_Order_info_t Board_Order_info[5];


__WEAK void CAN1_rxDataHandler(uint32_t canId, uint8_t *rxBuf);



/* Private functions ---------------------------------------------------------*/
/**
 *	@brief	CAN 标识符过滤器复位成默认配置
 */
static void CAN_Filter_ParamsInit(CAN_FilterTypeDef *sFilterConfig)
{
	sFilterConfig->FilterIdHigh = 0;						
	sFilterConfig->FilterIdLow = 0;							
	sFilterConfig->FilterMaskIdHigh = 0;					// 不过滤
	sFilterConfig->FilterMaskIdLow = 0;						// 不过滤
	sFilterConfig->FilterFIFOAssignment = CAN_FILTER_FIFO0;	// 过滤器关联到FIFO0
	sFilterConfig->FilterBank = 0;							// 设置过滤器0
	sFilterConfig->FilterMode = CAN_FILTERMODE_IDMASK;		// 标识符屏蔽模式
	sFilterConfig->FilterScale = CAN_FILTERSCALE_32BIT;		// 32位宽
	sFilterConfig->FilterActivation = ENABLE;				// 激活滤波器
	sFilterConfig->SlaveStartFilterBank = 0;
}

/* Exported functions --------------------------------------------------------*/
/**
 *	@brief	CAN1 初始化
 */
void CAN1_Init(void)
{
	CAN_FilterTypeDef sFilterConfig;
	// 配置CAN标识符滤波器
	CAN_Filter_ParamsInit(&sFilterConfig);
	HAL_CAN_ConfigFilter(&hcan, &sFilterConfig);
	// 使能接收中断
	HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
	// 开启CAN1
	HAL_CAN_Start(&hcan);
}


/**
 *	@brief	CAN 接收中断回调函数
 */


int mode_set;
static void CAN_Rx_Callback(CAN_HandleTypeDef *hcan)
{
	if(hcan->Instance == CAN1)
	{
		HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &hcan1RxFrame, can1_rx_data);
		
		CAN1_rxDataHandler(hcan1RxFrame.StdId, can1_rx_data);
		
	}

}


/**
  * @brief          hal CAN fifo call back, receive motor data
  * @param[in]      hcan, the point to CAN handle
  * @retval         none
  */
/**
  * @brief          hal库CAN回调函数,接收电机数据
  * @param[in]      hcan:CAN句柄指针
  * @retval         none
  */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{	
	if(hcan->Instance == CAN1)
	{		
		 CAN_Rx_Callback(hcan);
	}	
}		

void get_Centrol_measure(Board_Order_info_t *ptr)                                    
{                                                                   
	ptr->Twinkle_state = can1_rx_data[0];																
	ptr->All_Led_Close = can1_rx_data[1];  
	ptr->All_Led_Open =  can1_rx_data[2];  
	ptr->Board_Work =    can1_rx_data[3];  
	ptr->Set_Color  =    can1_rx_data[4];
	ptr->start_permission = can1_rx_data[5];
	ptr->RainBow_Flag =  can1_rx_data[6];
	ptr->count_t =  can1_rx_data[7];
	
}


/***************************
  五块符叶主控，烧哪块取消哪块的注释
***************************/
void Board_RX(uint32_t ID, uint8_t *data)
{  
	Board_Can_info.offline_cnt_max = OFFLINE_TIME_MAX; 
	
	switch(ID)
	{
		
//		case CAN_BOARD_ID_1_F:
//		{
//			get_Centrol_measure(&Board_Order_info[0]);
//			Board_Can_info.B1_offline_cnt= 0;
//			
//			break;
//		}	
////	
//	  case CAN_BOARD_ID_2_F:
//		{
//			get_Centrol_measure(&Board_Order_info[1]);
//			Board_Can_info.B2_offline_cnt= 0;
//			break;
//		}	
//		
//		case CAN_BOARD_ID_3_F:
//		{
//			get_Centrol_measure(&Board_Order_info[2]);
//			Board_Can_info.B3_offline_cnt= 0;
//			
//			break;
//		}	
//		
		case CAN_BOARD_ID_4_F:
		{
			get_Centrol_measure(&Board_Order_info[3]);
			Board_Can_info.B4_offline_cnt= 0;
			
			break;
		}	
////		
//		case CAN_BOARD_ID_5_F:
//		{
//			get_Centrol_measure(&Board_Order_info[4]);
//			Board_Can_info.B5_offline_cnt= 0;			
//			break;
//		}	
		
		default:
		{
				break;
		}
		
	}
}




/* rxData Handler [Weak] functions -------------------------------------------*/
/**
 *	@brief	[__WEAK] 需要在  中实现具体的 CAN1 数据处理协议 
 */
__WEAK void CAN1_rxDataHandler(uint32_t canId, uint8_t *rxBuf)
{
	
}




