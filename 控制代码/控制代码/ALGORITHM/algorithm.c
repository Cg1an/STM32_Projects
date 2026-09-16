#include "algorithm.h"
#include "delay.h"		 
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

#define a_F 1
#define b_F 0
#define c_F 0
#define d_F 1
#define e_F 0.02
#define f_F 0.0001

//   a+b*s+c*s^2
//------------------
//   d+e*s+f*s^2


FOC_t FOC;
PID_t PID_Vel;
PID_t PID_lsm;
//float A[6], B[6], C[6], D[6], E[6]; //用于计算各个扇区内开关时间的公式的参数

float Theta, aTheta;

s32 prePos, Pos_adj;
double Vel, Vel0;
double As_F[3],  Bs_F[3], vels[2], Vels[2];
float Position;
float Ua, Ub, Uc;

//更新二阶系统的各个Z系数
void upDateDNs(double deltaT, double a, double b, double c, double d, double e, double f, double * As, double * Bs)
{
	double TT = deltaT * deltaT;
	
	Bs[0] = d*TT + 2*e*deltaT + 4*f;
	Bs[1] = (2*d*TT - 8*f)/Bs[0];
	Bs[2] = (d*TT - 2*e*deltaT + 4*f)/Bs[0];
	
	As[0] = (a*TT + 2*b*deltaT + 4*c)/Bs[0];
	As[1] = (2*a*TT - 8*c)/Bs[0];
	As[2] = (a*TT - 2*b*deltaT + 4*c)/Bs[0];
}
//------------------end----------------------

//更新二阶系统的输出及状态
void upDateStatus(double * As, double * Bs, double * Ys, double * Xs, double x)
{
	double y = As[0]*x + As[1]*Xs[0] + As[2]*Xs[1] - Bs[1]*Ys[0] - Bs[2]*Ys[1];
	Xs[1] = Xs[0];
	Xs[0] = x;
	Ys[1] = Ys[0];
	Ys[0] = y;
}
//------------------end----------------------

void SetPID(PID_t * pid, float Kp, float Ki, float Kd, float A_Diff, float MaxInteg, float MaxOutput)
{
	pid->Kp = Kp;
	pid->Ki = Ki;
	pid->Kd = Kd;
	pid->a_Diff = A_Diff;
	pid->MaxInteg = MaxInteg;
	pid->MaxOutput = MaxOutput;
}
//------------------end----------------------

void PID(PID_t * pid, float ref, float feedback)
{
	pid->Error0 = pid->Error;
	pid->Error = ref - feedback;
	pid->Diff = pid->a_Diff*(pid->Error - pid->Error0) + (1-pid->a_Diff)*pid->Diff;//微分滤波
	pid->Integ += pid->Ki * pid->Error;
	if(pid->Integ > pid->MaxInteg) pid->Integ = pid->MaxInteg;//积分限幅
	else if(pid->Integ < -pid->MaxInteg) pid->Integ = -pid->MaxInteg;
	pid->Output = pid->Kp * pid->Error + pid->Ki * pid->Integ + pid->Kd * pid->Diff;
	if(pid->Output > pid->MaxOutput) pid->Output = pid->MaxOutput;//输出限幅
	else if(pid->Output < -pid->MaxOutput) pid->Output = -pid->MaxOutput;
}
//------------------end----------------------

//用于计算LSM输入：
void PID_LSM(PID_t * pid, double * pNewPart_phi, float input)
{
	pid->Error0 = pid->Error;
	pid->Error = input;
	pid->Diff = pid->a_Diff*(pid->Error - pid->Error0) + (1-pid->a_Diff) * pid->Diff;//微分滤波
	pid->Integ += pid->Error;
	
	pNewPart_phi[0] = input;
	pNewPart_phi[1] = pid->Integ;
	pNewPart_phi[2] = pid->Diff;
}
//------------------end----------------------

void FOC_Init()
{
	FOC.Status = currentSample_Verify;
	
	FOC.Val_ARR = 5250;
	
	FOC.Co = 0.5f * FOC.Val_ARR; //Co = 0.5f*T*E; T = Val_ARR * sin60(六边形内接圆半径); E[0] = 1/sin60
	FOC.R_s = 3.1f;       //定子电阻（相间电阻的一半），单位：欧姆
	FOC.L_q = 0.000725f;  //Q轴电感（相间电感的一半），单位：亨
	FOC.L_d = 0.000725f;  //D轴电感
	FOC.CoEs = 0.0001f;     //感应反电动势系数（转速常数的倒数），单位：v/rpm
	FOC.MaxMag = 0.1f;
	FOC.TargetIQ = 0.05f;
	FOC.TargetID = 0;
	FOC.ECRange = 16384;  //编码器值的范围
	FOC.Num_Pole = 7;     //电机极对数
	
	FOC.Kp_I = 0.3f;
	FOC.Ki_I = 0.2f;
	FOC.co_I = 0.0004883f;////1A<->4096, -1A<->0

	
	//电角度相关
	FOC.ECValue_360 = FOC.ECRange / FOC.Num_Pole;
	FOC.CurrentElectricalDeg=0;
	FOC.ElectricalDegCMD=0;
	
	//编码器Offset辨识相关：
	FOC.CurrentIndex_Pole=0;
	FOC.Offset = 50.0f;
	FOC.IdentifyResult = 0;
	FOC.Tolerance = 50;
	
	//初始化滤波器：
	upDateDNs(0.001, a_F, b_F, c_F, d_F, e_F, f_F, As_F, Bs_F);
}
//-------------end---------------

//FOC核心算法，克拉克逆变换/帕克逆变换
void SetPhaseVoltage()
{
	Ua = FOC.Co*(1 + FOC.ICMD_Alpha);
	Ub = FOC.Co*(1 - (FOC.ICMD_Alpha - SQRT_3*FOC.ICMD_Beta)/2);
	Uc = FOC.Co*(1 - (FOC.ICMD_Alpha + SQRT_3*FOC.ICMD_Beta)/2);
}
//------------------end----------------------

void Phase_Pulse(float K) //K代表电压幅值，在0-1之间。
{
	float theta, T1, T2;
	u8 i;
	
	while(FOC.ElectricalDegCMD >= PI_m2) FOC.ElectricalDegCMD -= PI_m2;
	while(FOC.ElectricalDegCMD < 0) FOC.ElectricalDegCMD += PI_m2;
	
	i = FOC.ElectricalDegCMD/PI_d3;
	theta = FOC.ElectricalDegCMD - i*PI_d3;//theta：扇区内，与顺时针侧的扇区边之间的角度
	
	if(K>1) K=1;
	else if(K<0) K=0;
	
	T1 = K * sinf(PI_d3 - theta);
	T2 = K * sinf(theta);
	
	switch(i)
	{
		case 0:
			PHA = FOC.Co * (1 + T1 + T2);
			PHB = FOC.Co * (1 - T1 + T2);
			PHC = FOC.Co * (1 - T1 - T2);
			break;
		case 1:
			PHA = FOC.Co * (1 + T1 - T2);
			PHB = FOC.Co * (1 + T1 + T2);
			PHC = FOC.Co * (1 - T1 - T2);
			break;
		case 2:
			PHB = FOC.Co * (1 + T1 + T2);
			PHC = FOC.Co * (1 - T1 + T2);
			PHA = FOC.Co * (1 - T1 - T2);
			break;
		case 3:
			PHB = FOC.Co * (1 + T1 - T2);
			PHC = FOC.Co * (1 + T1 + T2);
			PHA = FOC.Co * (1 - T1 - T2);
			break;
		case 4:
			PHC = FOC.Co * (1 + T1 + T2);
			PHA = FOC.Co * (1 - T1 + T2);
			PHB = FOC.Co * (1 - T1 - T2);
			break;
		case 5:
			PHC = FOC.Co * (1 + T1 - T2);
			PHA = FOC.Co * (1 + T1 + T2);
			PHB = FOC.Co * (1 - T1 - T2);
			break;
		default: 
			break;
	}
}
//-------------end---------------


//根据编码器信号，获得当前电机电角度位置
void Phase_Pos()
{
	s32 temp = FOC.ECValue - FOC.Offset;
	temp %= FOC.ECValue_360;
	
	FOC.CurrentElectricalDeg = PI_m2 * temp / FOC.ECValue_360;
}
//-------------end---------------


void abc2park()
{
  FOC.IAlpha =  FOC.co_I * FOC.Current[0];
  FOC.IBeta  =  FOC.co_I * (FOC.Current[0] + 2*FOC.Current[1])/SQRT_3;
	
	FOC.ID = cos(FOC.CurrentElectricalDeg) * FOC.IAlpha + sin(FOC.CurrentElectricalDeg)* FOC.IBeta;
  FOC.IQ = -sin(FOC.CurrentElectricalDeg)* FOC.IAlpha + cos(FOC.CurrentElectricalDeg)* FOC.IBeta;
}
//-------------end---------------


void getICMD()
{
	float temp;
	
	FOC.eQ = FOC.TargetIQ - FOC.IQ;
	FOC.eD = FOC.TargetID - FOC.ID;
	
	FOC.integ_Q += FOC.Ki_I * FOC.eQ;
	FOC.integ_D += FOC.Ki_I * FOC.eD;
	
	//对积分幅度进行限制：
	temp = sqrtf(FOC.integ_Q*FOC.integ_Q + FOC.integ_D*FOC.integ_D);
	if(temp > FOC.MaxMag) 
	{
		temp = FOC.MaxMag / temp;
		FOC.integ_Q *= temp;
		FOC.integ_D *= temp;
	}
	
	FOC.ICMD_Q = FOC.Kp_I*FOC.eQ + FOC.integ_Q;
	FOC.ICMD_D = FOC.Kp_I*FOC.eD + FOC.integ_D;
	
	//转速前馈补偿：
	FOC.ICMD_D -= FOC.EAngVel * FOC.L_q * FOC.IQ;
	FOC.ICMD_Q += FOC.EAngVel * FOC.L_d * FOC.ID + FOC.Velocity*FOC.CoEs;
	
	//park2clark
	FOC.ICMD_Alpha =  cos(FOC.CurrentElectricalDeg) * FOC.ICMD_D -  sin(FOC.CurrentElectricalDeg) * FOC.ICMD_Q;
  FOC.ICMD_Beta  =  sin(FOC.CurrentElectricalDeg) * FOC.ICMD_D +  cos(FOC.CurrentElectricalDeg) * FOC.ICMD_Q;
	
	//clark2vector
	FOC.ICMD_Mag = sqrtf(FOC.ICMD_Alpha*FOC.ICMD_Alpha + FOC.ICMD_Beta*FOC.ICMD_Beta);
	FOC.ElectricalDegCMD = atan2f(FOC.ICMD_Beta, FOC.ICMD_Alpha);
	
	if(FOC.ICMD_Mag > FOC.MaxMag) FOC.ICMD_Mag = FOC.MaxMag;
}
//-------------end---------------

void getPos()
{
	
}
//--------------end-----------------


void run()
{
	float temp;
	
	//abc2park
//	FOC.IAlpha =  FOC.co_I * FOC.Current[0];
//  FOC.IBeta  =  FOC.co_I * (FOC.Current[0] + 2*FOC.Current[1])/SQRT_3;
//	
//	FOC.ID = cos(FOC.CurrentElectricalDeg) * FOC.IAlpha + sin(FOC.CurrentElectricalDeg)* FOC.IBeta;
//  FOC.IQ = -sin(FOC.CurrentElectricalDeg)* FOC.IAlpha + cos(FOC.CurrentElectricalDeg)* FOC.IBeta;
	abc2park();
	
	
	//电流环：
	FOC.eQ = FOC.TargetIQ - FOC.IQ;
	FOC.eD = FOC.TargetID - FOC.ID;
	
	FOC.integ_Q += FOC.Ki_I * FOC.eQ;
	FOC.integ_D += FOC.Ki_I * FOC.eD;
	
	//电流环对积分幅度进行限制：
	temp = sqrtf(FOC.ICMD_Alpha*FOC.ICMD_Alpha + FOC.ICMD_Beta*FOC.ICMD_Beta);
	if(temp > FOC.MaxMag) 
	{
		temp = FOC.MaxMag / temp;
		FOC.integ_Q *= temp;
		FOC.integ_D *= temp;
	}
	
	FOC.ICMD_Q = FOC.Kp_I*FOC.eQ + FOC.integ_Q;
	FOC.ICMD_D = FOC.Kp_I*FOC.eD + FOC.integ_D;
	
	//转速前馈补偿：
	FOC.ICMD_D -= FOC.EAngVel * FOC.L_q * FOC.IQ;
	FOC.ICMD_Q += FOC.EAngVel * FOC.L_d * FOC.ID + FOC.Velocity*FOC.CoEs;
	
//	FOC.ICMD_D = 0;
//	FOC.ICMD_Q = FOC.MaxMag;
	
	//park2clark
	FOC.ICMD_Alpha =  cos(FOC.CurrentElectricalDeg) * FOC.ICMD_D -  sin(FOC.CurrentElectricalDeg) * FOC.ICMD_Q;
  FOC.ICMD_Beta  =  sin(FOC.CurrentElectricalDeg) * FOC.ICMD_D +  cos(FOC.CurrentElectricalDeg) * FOC.ICMD_Q;
	
	//clark2vector
	FOC.ICMD_Mag = sqrtf(FOC.ICMD_Alpha*FOC.ICMD_Alpha + FOC.ICMD_Beta*FOC.ICMD_Beta);
	FOC.ElectricalDegCMD = atan2f(FOC.ICMD_Beta, FOC.ICMD_Alpha);
	
	if(FOC.ICMD_Mag > FOC.MaxMag) FOC.ICMD_Mag = FOC.MaxMag;
	
	Phase_Pulse(FOC.ICMD_Mag);
}
//--------------end-----------------


void getVel()
{
	Vel = FOC.ECValue - prePos;
	if(fabsl(Vel) > 5000) Vel = Vel0;
	else Vel0 = Vel;
	upDateStatus(As_F, Bs_F, vels, Vels, Vel);
	FOC.Velocity = vels[0]*60000/FOC.ECRange;//最后60000/FOC.ECRange用于将转速单位转为r/min
	FOC.EAngVel = FOC.Velocity*FOC.Num_Pole * PI_m2/60;//转成rad/s
	
	prePos = FOC.ECValue;
}
//--------------end-----------------

