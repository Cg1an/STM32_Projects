#ifndef _CAN_H


#define _CAN_H


#include "sys.h"
void CAN_Configure(void);
void Set_CloudMotor_Current(int16_t gimbal_yaw_iq);

void Set_ChassisMotor_Current(int16_t cm1_iq, int16_t cm2_iq, int16_t cm3_iq, int16_t cm4_iq);
#endif