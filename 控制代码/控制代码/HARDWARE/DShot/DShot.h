#ifndef __DShot_H
#define __DShot_H	 
#include "sys.h" 
#include "dma.h"
#include "timer.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//LED端口定义
#define ECRange 16384
#define C2PI 0.0003835f //2pi/16384
#define C2Deg 0.0219726f //360/16384
#define TIM_IT_Update  ((uint16_t)0x0001)
#define tickPeriod 100 //us

typedef struct {
	u16 ID;
	u16 Throttle;
	u16 Deflection_X;
	u16 Deflection_Y;
}DSCMD;

typedef struct {
	float MaxMagnitude;
	float Offset;
	float X;
	float Y;
	float phaseCMD;
	float deflection;
}Deflection;

extern DSCMD DShot_CMD;
extern Deflection deflectionCMD;
extern uint16_t Send_data[16];
extern u16 tick;
extern u16 Veltick;

void LED_Init(void);//初始化		 		
void TIM1_Motor_PWM_Init(void);
void TIM2_Init(void );
void SendSignal(uint16_t *motor);
u16 add_checksum_and_telemetry(u16 packet, u8 telem);
void pwmWriteDigital(int value);

#endif

















