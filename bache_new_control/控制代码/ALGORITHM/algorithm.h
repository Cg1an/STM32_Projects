#ifndef __ALG_H
#define __ALG_H	
#include "sys.h" 
#include "timer.h"
#include "adc.h"
#include <math.h>
#include "LSM.h" 
#include "Matrix.h" 

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//ADC 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/6
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									   
////////////////////////////////////////////////////////////////////////////////// 
#define ARR_TIM5 20000
#define ARR_TIM7 1000


typedef struct
{
	uint32_t Status;//当前状态：0-编码器Offset辨识；1-验证编码器offset辨识结果；2-验证电流信号的采集功能；3-正常运行
	
	//电机参数
	uint32_t Val_ARR;
	float R_s;  //定子电阻（相间电阻的一半），单位：欧姆
	float L_q;  //Q轴电感（相间电感的一半），单位：亨
	float L_d;  //D轴电感
	float CoEs; //感应反电动势系数（转速常数的倒数），单位：v/rpm

	uint32_t  ECRange; //编码器值的范围
	uint32_t  Num_Pole;   //电机极对数
	
	float Co;
	float MaxMag;
	
	//电流相关：
	float Current[3];
	float curr_Adj[3];
	float IAlpha;
	float IBeta;
	float IQ;
	float ID; 
	float TargetIQ;
	float TargetID;
	float ICMD_Mag;
	float ICMD_Alpha;
	float ICMD_Beta;
	float ICMD_Q;
	float ICMD_D;
	float Kp_I;
	float Ki_I;
	float co_I;//1A<->4096, -1A<->0
	float eQ;
	float eD;
	float integ_Q;
	float integ_D;
	
	//电角度相关：
	int32_t ECValue_360;//每360电角度所对应的编码器值
	float CurrentElectricalDeg;
	float ElectricalDegCMD;
	
	//转速：
	float Velocity;
	float EAngVel;//电角速度，单位：弧度每秒
	
	//编码器相关：
	uint16_t ECValue;
	uint16_t Reserved_ec;
	
	//编码器Offset辨识相关：
	float Sum1;
	float Sum2;
	float Offset;
	int16_t CurrentIndex_Pole;
	int8_t IdentifyResult;//1-辨识成功；-1-电角度与编码器值的增加方向相反；-2-存在采样错误
	uint8_t Tolerance;//offset辨识过程中采样值的容许误差
} FOC_t;
//-------------end---------------

typedef struct
{
	float Kp;
	float Ki;
	float Kd;
	float Error;
	float Error0;
	float Diff;
	float a_Diff;
	float Integ;
	float MaxInteg;
	float MaxOutput;
	float Output;
} PID_t;
//-------------end---------------

typedef struct
{
	uint16_t Head;
	uint16_t Res;
	float vel;
	float pos;
	float Ia;
	float Ib;
	float Ic;
} Data_t;
//-------------end---------------

typedef enum 
{
	offset_Identify = 0,
	offset_Verify,
	currentSample_Verify,
	runFOC,
	currentPIDAdjust,
	velocityPIDAdjust
}FOCStatus_t;
//-------------end---------------

extern FOC_t FOC;
extern PID_t PID_Vel;
extern PID_t PID_lsm;
extern s32 Cnt2;
extern float Theta;
extern float Position;
extern float Ua, Ub, Uc;
extern u8 Tick;

#define sin60     (0.8660254f)
#define PI        (3.1415926f)
#define PI_d3     (1.04719755f)
#define PI_d2     (1.5707963f)
#define PI_m2     (6.2831853f)
#define SQRT_3    (1.7320508f)

void FOC_Init(void);

void abc2park(void);
void getICMD(void);
void run(void);

void getPos(void);
void getVel(void);

void SetPID(PID_t * pid, float Kp, float Ki, float Kd, float A_Diff, float MaxInteg, float MaxOutput);
void PID(PID_t * pid, float ref, float input);
void PID_LSM(PID_t * pid, double * pNewPart_phi, float input);
void upDateDNs(double deltaT, double a, double b, double c, double d, double e, double f, double * As, double * Bs);
void upDateStatus(double * As, double * Bs, double * Ys, double * Xs, double x);
#endif 















