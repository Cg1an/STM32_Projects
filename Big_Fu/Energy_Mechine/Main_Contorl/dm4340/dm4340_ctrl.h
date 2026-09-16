#ifndef __DM4340_CTRL_H__
#define __DM4340_CTRL_H__
#include "main.h"
#include "dm4340_drv.h"

extern int8_t motor_id;

typedef enum
{
	Motor1,
	Motor2,
	Motor3,
	num
} motor_num;

extern motor_t motor[num];

void dm4340_motor_init(void);
void ctrl_send(void);
void can1_rx_callback(void);
void can2_rx_callback(void);
#endif
