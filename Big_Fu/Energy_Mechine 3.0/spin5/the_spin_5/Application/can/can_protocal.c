/**
 * @file        can_potocol.c
 * @author      RobotPilots@2020
 * @Version     V1.0
 * @date        9-September-2020
 * @brief       CAN Potocol.
 */

/* Includes ------------------------------------------------------------------*/
#include "can_protocal.h"
#include "main.h"

Big_Fu_info_t Big_Fu_info[5];
uint8_t can_Dafu_data[8];

/************** 信息接收处理 ***************/
/**
 *	@brief	CAN1 接收数据 数据处理
 */

void CAN1_rxDataHandler(uint32_t canId, uint8_t *rxBuf)
{
    Board_RX(canId, can1_rx_data);
}
/************** 信息发送处理 ***************/
/**
 *	@brief	CAN1 发送数据 数据处理
     五块符叶主控，烧哪块取消哪块的注释
 */


void Board_Info_Tx()
{
    //   memcpy(can_Dafu_data, &Big_Fu_info[0], 8);
    //	 Board_Tx(CAN_BOARD_ID_1,can_Dafu_data,1,0x08);

    //	 memcpy(can_Dafu_data, &Big_Fu_info[1], 8);
    //	 Board_Tx(CAN_BOARD_ID_2,can_Dafu_data,1,0x08);
    //
    //	 memcpy(can_Dafu_data, &Big_Fu_info[2], 8);
    //	 Board_Tx(CAN_BOARD_ID_3,can_Dafu_data,1,0x08);
    //
    //	 memcpy(can_Dafu_data, &Big_Fu_info[3], 8);
    //	 Board_Tx(CAN_BOARD_ID_4,can_Dafu_data,1,0x08);
    ////
    //	 memcpy(can_Dafu_data, &Big_Fu_info[4], 8);
    //	 Board_Tx(CAN_BOARD_ID_5,can_Dafu_data,1,0x08);
    if (BOARD_NUMBER == 0)
    {
        memcpy(can_Dafu_data, &Big_Fu_info[0], 8);
        Board_Tx(CAN_BOARD_ID_1, can_Dafu_data, 1, 0x08);
    }
    else if (BOARD_NUMBER == 1)
    {
        memcpy(can_Dafu_data, &Big_Fu_info[1], 8);
        Board_Tx(CAN_BOARD_ID_2, can_Dafu_data, 1, 0x08);
    }
    else if (BOARD_NUMBER == 2)
    {
        memcpy(can_Dafu_data, &Big_Fu_info[2], 8);
        Board_Tx(CAN_BOARD_ID_3, can_Dafu_data, 1, 0x08);
    }
    else if (BOARD_NUMBER == 3)
    {
        memcpy(can_Dafu_data, &Big_Fu_info[3], 8);
        Board_Tx(CAN_BOARD_ID_4, can_Dafu_data, 1, 0x08);
    }
    else if (BOARD_NUMBER == 4)
    {
        memcpy(can_Dafu_data, &Big_Fu_info[4], 8);
        Board_Tx(CAN_BOARD_ID_5, can_Dafu_data, 1, 0x08);
    }
}

