#ifndef __CAN_POTOCAL_H
#define __CAN_POTOCAL_H

/* Includes ------------------------------------------------------------------*/
#include "CAN_receive.h"
#include "CAN_send.h"
#include "struct_typedef.h"
#include "string.h"
/* Exported functions --------------------------------------------------------*/





void Board_RX(uint32_t ID, uint8_t *data);

void Board_Info_Tx(void);
#endif

