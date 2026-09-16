#include "PID.h"


FP_32 error_Angle_I, error_Angle, error_Gyro_I[3];
FP_32 magHold;
FP_32 PTerm, ITerm, DTerm, ATerm, GTerm;
FP_32 axis_PID[3];

FP_32 Delta[3] = {0};
FP_32 Delta0[3]; //低通滤波

FP_32 Level_Comm_LPF_a; //姿态遥控信号的低通滤波参数
FP_32 Level_Comm_LPF_b;
FP_32 Throt_Comm_LPF_a; //油门遥控信号的低通滤波参数
FP_32 Throt_Comm_LPF_b;

FP_32 DTerm_LPFaDIFF_a;
FP_32 DTerm_LPFaDIFF_b;
FP_32 PTerm_LPF_a;
FP_32 PTerm_LPF_b;

FP_32 Rc_Comm[4] = {0.0f};
FP_32 Rc_Comm0[4];
FP_32 rc_Comm[4];

FP_32 lastGyro[3];

Attitude_Data      Gyro_Ctrl;     //用于反馈的角速度
Attitude_Data      Gyro_Ctrl0; 

FP_32 Kp[8];
FP_32 Ki[8];
FP_32 Kd[8];
int16_t rcCommand[8] = {0};
int16_t initialThrottleHold;
int16_t REAR_R, FRONT_R, REAR_L, FRONT_L, REAR, FRONT, R, L;
int16_t temp;
uint8_t axis;
uint8_t rcDelayCommand;
uint8_t reqdata;
uint8_t armed = 0u;


/*初始化用于PID参数动态调整的初始化参数*/
void  Para_Init( void )
{
	DTerm_LPFaDIFF_a = (2 * deltaTime * DTerm_LPFaDIFF_freq) / (2 + deltaTime * DTerm_LPFaDIFF_freq);
	DTerm_LPFaDIFF_b = (2 - deltaTime * DTerm_LPFaDIFF_freq) / (2 + deltaTime * DTerm_LPFaDIFF_freq);
	PTerm_LPF_a = (deltaTime * PTerm_LPF_freq) / (2 + deltaTime * PTerm_LPF_freq);
	PTerm_LPF_b = (2-deltaTime * PTerm_LPF_freq) / (2 + deltaTime * PTerm_LPF_freq);
	Level_Comm_LPF_a = (deltaTime * Level_Comm_LPF_freq) / (2 + deltaTime * Level_Comm_LPF_freq);
	Level_Comm_LPF_b = (2-deltaTime * Level_Comm_LPF_freq) / (2 + deltaTime * Level_Comm_LPF_freq);
	Throt_Comm_LPF_a = (deltaTime * Throt_Comm_LPF_freq) / (2 + deltaTime * Throt_Comm_LPF_freq);
	Throt_Comm_LPF_b = (2-deltaTime * Throt_Comm_LPF_freq) / (2 + deltaTime * Throt_Comm_LPF_freq);
	
  Kp[ROLL]     = 15.0f;     Ki[ROLL]     = 10;      Kd[ROLL]    = 7;
  Kp[PITCH]    = 15;     Ki[PITCH]    = 10.0f;      Kd[PITCH]   = 7;
  Kp[YAW]      = 85.0f;     Ki[YAW]      = 10;      Kd[YAW]     = 7;
  Kp[PIDALT]   = 47;     Ki[PIDALT]   = 0.0f;       Kd[PIDALT]  = 0;
  Kp[PIDVEL]   = 0;      Ki[PIDVEL]   = 0;       Kd[PIDVEL]  = 0;
  Kp[PIDLEVEL] = 10;     Ki[PIDLEVEL] = 0;
  Kp[PIDMAG]   = 75;     Ki[PIDMAG]   = 0;	
}

void CheckMode( void )
{
	if (rcData1[THROTTLE] < MINCHECK)
	{
		error_Angle_I  = 0;
		error_Gyro_I[ROLL]  = 0;
		error_Gyro_I[PITCH] = 0;
		error_Gyro_I[YAW]   = 0;
		rcDelayCommand = rcDelayCommand + 1;
		if (rcData1[YAW] > MAXCHECK)
		{
			if (rcDelayCommand == 160)
			{
				armed = 1;
				initialPress = Press;
				magHold = angle.Attitude.Z;
				MPU_Gyro_Trim.X += (int16_t)(Gyro.Attitude.X / scale_Gyro);
				MPU_Gyro_Trim.Y += (int16_t)(Gyro.Attitude.Y / scale_Gyro);
				MPU_Gyro_Trim.Z += (int16_t)(Gyro.Attitude.Z / scale_Gyro);
			}
		}
		else if (rcData1[YAW] < MINCHECK)
		{
			if (rcDelayCommand == 160)
			{
				armed = 0;
			}
		}
		else if (rcData1[ROLL] < MINCHECK)
		{
			if (rcDelayCommand == 160)
			{
				reqdata = 10;//SD卡开始记录数据
			}
		}
		else if (rcData1[ROLL] > MAXCHECK)
		{
			if (rcDelayCommand == 160)
			{
				reqdata = 0;//SD卡开始记录数据
			}
		}
		else rcDelayCommand = 0;
	}
	if(reqdata > 0)
	{
		DataRecord();
	}
}


/*对横滚（ROLL）、俯仰（PITCH）以及航向（YAW或Heading）三轴的PID控制器参数以及参考指令进行动态调整*/
void PID( void )
{
	for(axis=0; axis<3; axis++)
	{
		temp = min(abs(rcData1[axis] - MIDRC), 500);
		if(temp > DEADBAND) temp -=DEADBAND;
		else temp = 0;
		
		rc_Comm[axis]	= rcCommand[axis];
		rcCommand[axis] = temp;
		if (rcData1[axis] < MIDRC) rcCommand[axis] = -rcCommand[axis];
		if (axis == ROLL) rcCommand[axis] = -rcCommand[axis];
		Rc_Comm[axis] = Level_Comm_LPF_a * (rcCommand[axis] + rc_Comm[axis]) * 0.08f + Level_Comm_LPF_b * Rc_Comm0[axis];
		Rc_Comm0[axis] = Rc_Comm[axis];
	}

	rc_Comm[THROTTLE] = rcCommand[THROTTLE];
	rcCommand[THROTTLE] = MINTHROTTLE;
	rcCommand[THROTTLE] += (MAXTHROTTLE - MINTHROTTLE) * (rcData1[THROTTLE] - MINCHECK) / (MAXCHECK - MINCHECK);
	//rcCommand[THROTTLE] += - 100 * (Accel.Z - 1) - 150 * EstVel_Acc - 800 * EstVel - 4 * EstVel_I;
	Rc_Comm[THROTTLE] = Throt_Comm_LPF_a * (rcCommand[THROTTLE] + rc_Comm[THROTTLE]) + Throt_Comm_LPF_b * Rc_Comm0[THROTTLE];
	Rc_Comm0[THROTTLE] = Rc_Comm[THROTTLE];

	Gyro_Ctrl.A[0]  = Gyro.A[0] * 57.3f + angle.A[1] * Gyro.A[2];
	Gyro_Ctrl.A[1]  = Gyro.A[1] * 57.3f - angle.A[0] * Gyro.A[2];
	
	for(axis=0; axis<3; axis++)
	{
		if (axis < 2)  //两个水平轴
		{
			error_Angle          = Rc_Comm[axis] - angle.A[axis];
#ifdef GPS
			if(GPS_sat > 3) error_Angle = error_Angle - 57.3f * atan(Body_vel.A[1-axis] / 10); //如收到三颗以上的卫星，采用速度反馈
#endif
			ATerm                = error_Angle * Kp[PIDLEVEL] / 10;
			
			Gyro_Ctrl.A[axis]    = ATerm - Gyro_Ctrl.A[axis];
			error_Gyro_I[axis]   = Constrain(error_Gyro_I[axis] + Gyro_Ctrl.A[axis] * deltaTime, -10000, +10000);   //角速度积分
			Delta0[axis]         = Delta[axis];
			Delta[axis]          = DTerm_LPFaDIFF_a * (Gyro_Ctrl.A[axis] - lastGyro[axis]) + DTerm_LPFaDIFF_b * Delta0[axis];
			lastGyro[axis]       = Gyro_Ctrl.A[axis];
			DTerm                = 3 * (Delta[axis] * Kd[axis]) / 10;//角速度微分反馈
			
			axis_PID[axis]       = Gyro_Ctrl.A[axis] * Kp[axis] / 10 + error_Gyro_I[axis] * Ki[axis] / 1000 + DTerm;
		}
		else // 方位轴
		{
			PTerm                = magHold - angle.Attitude.Z;
			if (PTerm < -180)
			{
				PTerm += 360;
			}
      if (PTerm > +180)
			{
				PTerm -= 360;
			}
			error_Angle_I        = Constrain(error_Angle_I + PTerm, -10000, +10000);
			ATerm                = PTerm * Kp[PIDMAG] / 10 + error_Angle_I * Ki[PIDMAG] / 10000;//角度反馈

			error_Gyro_I[axis]   = Constrain(error_Gyro_I[axis] + 20 * Rc_Comm[axis] + Gyro.A[axis], -10000, +10000);   //角速度积分
			Delta0[axis]         = Delta[axis];
			Delta[axis]          = DTerm_LPFaDIFF_a * (Gyro.A[axis] * 57.3f - lastGyro[axis]) + DTerm_LPFaDIFF_b * Delta0[axis];
			lastGyro[axis]       = Gyro.A[axis] * 57.3f;
			DTerm                = 3 * (Delta[axis] * Kd[axis]) / 10;//角速度微分反馈
			GTerm                = (Rc_Comm[axis] + Gyro.A[axis]* 57.3f) * Kp[axis] / 10 + error_Gyro_I[axis] * Ki[axis] / 1000;//角速度反馈。对于方位轴的速度而言，正号对应负反馈
			
			//axis_PID[axis]       = ATerm + GTerm + DTerm;
			axis_PID[axis]       = 0;
		}
	}
// 	REAR   = Constrain(PIDMIX(   0, 1.4, -1), 1000, MAXTHROTTLE); //八旋翼
// 	FRONT  = Constrain(PIDMIX(   0,-1.4, -1), 1000, MAXTHROTTLE);
// 	L      = Constrain(PIDMIX(+1.4,   0, -1), 1000, MAXTHROTTLE);	
// 	R      = Constrain(PIDMIX(-1.4,   0, -1), 1000, MAXTHROTTLE);
// 	REAR_R = Constrain(PIDMIX(-1,+1,+1), 1000, MAXTHROTTLE);
// 	FRONT_R= Constrain(PIDMIX(-1,-1,+1), 1000, MAXTHROTTLE);
// 	REAR_L = Constrain(PIDMIX(+1,+1,+1), 1000, MAXTHROTTLE);
// 	FRONT_L= Constrain(PIDMIX(+1,-1,+1), 1000, MAXTHROTTLE);
	
	REAR_R = Constrain(PIDMIX(-1,+1,-1), 1000, MAXTHROTTLE); //四旋翼
	FRONT_R= Constrain(PIDMIX(-1,-1,+1), 1000, MAXTHROTTLE);
	REAR_L = Constrain(PIDMIX(+1,+1,+1), 1000, MAXTHROTTLE);
	FRONT_L= Constrain(PIDMIX(+1,-1,-1), 1000, MAXTHROTTLE);
	
	if (armed & (rcData1[THROTTLE] > MINCHECK)) //
	{
		PwmOut(3,  REAR_R); //11
		PwmOut(4,  FRONT_R); //12 最靠近电路板边缘的引脚
		PwmOut(5,  R); //5
		PwmOut(6,  L); //6
		PwmOut(7,  REAR); //7
		PwmOut(8,  FRONT); //8
		PwmOut(2,  REAR_L); //10
		PwmOut(1,  FRONT_L); //9
	}
	else
	{
		PwmOut(3,  980); //REAR
		PwmOut(4,  980); //FRONT
		PwmOut(5,  980); //R
		PwmOut(6,  980); //L	
		PwmOut(7,  980); //REAR_R
		PwmOut(8,  980); //FRONT_R
		PwmOut(2,  980); //REAR_L
		PwmOut(1,  980); //FRONT_L
	}
} //PID()end



