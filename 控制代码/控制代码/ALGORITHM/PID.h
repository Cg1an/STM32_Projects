#ifndef __PID_H
#define __PID_H

#define Level_Comm_LPF_freq  10
#define Throt_Comm_LPF_freq  2

#define PTerm_LPF_freq      20
#define DTerm_LPFaDIFF_freq 50

#define min(a, b) ((a) < (b) ? (a) : (b))
#define abs(a) ((a) < 0 ? -(a) : (a))
#define YAW_DIRECTION   1
#define PIDMIX(X,Y,Z) (rcCommand[THROTTLE] + axis_PID[ROLL]*X + axis_PID[PITCH]*Y + YAW_DIRECTION * axis_PID[YAW]*Z)

void  Para_Init( void );
void PID( void );
void CheckMode( void );

#endif

