#include "sys.h"
#include "delay.h"
#include "led.h"
#include "usart.h"	
#include "timer.h"
#include "adc.h"
#include "MPU.h"
#include "can.h"
#include "canuser.h"
#include "DShot.h"
#include "J6006.h"

//#define RecordData
//#define TransData

#define dataLength      1500
#define totalDataLength 9000


double a_I = 1;
double b_I = 0;
double c_I = 0;
double d_I = 0;
double e_I = 1;
double f_I = 2;

double NewPart_phi[3], Z_lsm = 0, rho2 = 0.99;
double As_I[3],  Bs_I[3],  Ys_I[2],  Xs_I[2];

float TargetVel = 100;

float K = 0.2f;
float Mag = 0.5;
float dPhase = PI_d2;
Data_t Data;

u16 DIndex = 0;

u16 TickCNT = 0;

float VelRatio = 1;

int main(void)
{
	Stm32_Clock_Init(336,8,2,7);//设置时钟,168Mhz 
	delay_init(168);			      //延时初始化   
	uart_init();
 	PWM_Init();
	MPU_Init();
	J6006_MOTOR_DATA_INIT();
	
	//BTR=42M/((6+7+1)*3)=1Mbps
//	CAN1_Mode_Init(1,6,7,3,0);//u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode， 1M
	CAN1_Mode_Init(1,6,7,12,0);//u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode， 250k
	CAN2_Mode_Init(1,6,7,3,0);//u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode， 1M
	InitControl();
	
	uint8_t Enable_Cmd[8] = {0x2B, 0x03, 0x01, 0x00, 0x0A, 0x01, 0x00, 0x00};
	CAN_DATA Enable_Msg;
	CAN_SetHead(&Enable_Msg, 0x600, 0, 0, 8);
	CAN_SetDATA(&Enable_Msg, Enable_Cmd);
	CAN_Trans(CAN1, &Enable_Msg); // 发给 CAN2 激活底盘

	delay_ms(100);
	
	J6006_yaw.MIT_V_set= 0.0f;
	J6006_yaw.MIT_T_set= 0.0f;
	
	delay_ms(1000);
	
  while(1)
	{
		if(Tick == 0) continue;
		Tick = 0;
		
		uint8_t Chassis_Cmd[8];
    Chassis_Cmd[0] = 0x2B; 
    Chassis_Cmd[1] = 0x00;
    Chassis_Cmd[2] = 0x00;
    Chassis_Cmd[3] = 0x00;
    Chassis_Cmd[4] = (uint8_t)out_cmd_l; // 直接取全局变量
    Chassis_Cmd[5] = (uint8_t)out_cmd_r; // 直接取全局变量
    Chassis_Cmd[6] = 0x00;
    Chassis_Cmd[7] = 0x00;
		
		CAN_DATA Chassis_Msg;
		CAN_SetHead(&Chassis_Msg, 0x600, 0, 0, 8);
		CAN_SetDATA(&Chassis_Msg, Chassis_Cmd);
		CAN_Trans(CAN1, &Chassis_Msg);
//		CANx_SendStdData(CAN1, 0x600, Chassis_Cmd);
		
		
		J6006_yaw.MIT_V_set= HCMD / VelRatio;
		J6006_pitch.MIT_V_set= VCMD / VelRatio;
		
		MIT_mode_send(CAN1, &CAN_Motor1, &J6006_yaw);
		MIT_mode_send(CAN2, &CAN_Motor1, &J6006_pitch);
		
		TickCNT ++;
		if(TickCNT > 2000)
		{
			TickCNT = 0;
			pwmWriteDigital(0x55);
			SendSignal(LaserSig);
		}
		
		Wired_Send();
		
		#ifdef TransData
//		Data.vel = FOC.Velocity;
//		Data.pos = NewPart_phi[0];
//		Data.Ia = NewPart_phi[1];
//		Data.Ib = NewPart_phi[2];
//		Data.Ic = Z_lsm;

		if(TransEmpty == 1)
		{
			TransEmpty = 0;
			transCNT = 0;
			Data.Res = 0x0201;
//			Data.vel = FOC.ECValue;
//			Data.pos = FOC.ECValue_360;
			Data.vel ++;
			if(Data.vel > 360) Data.vel=0;
			Data.pos = sinf(Data.vel * PI_m2 / 360);
			Data.Ia = 0;
			Data.Ib = 0;
			Data.Ic = 0;
			MYDMA_Enable(DMA2_Stream7, (u32)(&Data), 12);
		}

		#endif
		//TransData
	}
}



