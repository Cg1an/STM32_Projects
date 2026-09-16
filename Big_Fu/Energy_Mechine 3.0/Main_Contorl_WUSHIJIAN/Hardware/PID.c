#include "main.h"

PID M3508_1,M3508_2,M3508_3,M3508_4;
PID DM10010L_Follow_Ang,DM10010L_Follow_Speed;
PID DM10010L_Ang;

void pid_init(PID *pid,float kp,float ki,float kd, float Sum_error_max,float pmax, float imax, float dmax,float omax)
{
	pid->OMAX=omax;
	pid->PMAX=pmax;
	pid->IMAX=imax;
	pid->DMAX=dmax;
	pid->Sum_error_max=Sum_error_max;
	
	pid->Kp=kp;
	pid->Ki=ki;
	pid->Kd=kd;
	
	pid->Out=0.0f;
	pid->Pout=0.0f;
	pid->Iout=0.0f;
	pid->Dout=0.0f;
	pid->error[0]=0.0f;
	pid->error[1]=0.0f;
	pid->error[2]=0.0f;
	pid->set=0.0f;
	pid->real=0.0f;
}
float  PID_Cal_position(PID *Pid, float real, double set,float i_threshold)//get为实际值  set为目标值     _位置式
{	
	Pid->set = set;
	Pid->real = real;
	Pid->error[0] = Pid->set - Pid->real;
	
	Pid->sum_error += Pid->error[0];
  
	Pid->Pout=Pid->Kp * Pid->error[0] ;
	Pid->Pout > Pid->PMAX ? Pid->Pout = Pid->PMAX : Pid->Pout ;
	Pid->Pout < -Pid->PMAX ? Pid->Pout = -Pid->PMAX : Pid->Pout ;

	
	if(fabs(Pid->error[0]) < i_threshold)
	{
		Pid->sum_error += Pid->error[0];                                         //对误差进行积分（累加）
		
		Pid->Iout=Pid->Ki * Pid->sum_error ;
		Pid->Iout > Pid->IMAX ? Pid->Iout=Pid->IMAX :Pid->Iout;
		Pid->Iout < -Pid->IMAX ?Pid->Iout = -Pid->IMAX :Pid->Iout;
	}
	else
	{
		Pid->sum_error = 0;																								 // 误差大于阈值:远离目标值,清零积分(或保持不变)
		Pid->Iout = 0;
	}

	Pid->Dout=Pid->Kd * ( Pid->error[0] - Pid->error[1] );
	Pid->Dout > Pid->DMAX ? Pid->Dout = Pid->DMAX : Pid->Dout;
	Pid->Dout < -Pid->DMAX ? Pid->Dout = -Pid->DMAX : Pid->Dout;
	
	Pid->Out=Pid->Pout+Pid->Iout+Pid->Dout;
	Pid->Out > Pid->OMAX ? Pid->Out = Pid->OMAX : Pid->Out;
	Pid->Out < -Pid->OMAX ? Pid->Out = -Pid->OMAX : Pid->Out;
	Pid->error[1] = Pid->error[0];
	return Pid->Out;
}
void Chassis_PID_Init(void)
{
 pid_init(&M3508_1, 12.40f, 1.0f, 0.0f,4000.0f, 16000.0f, 12000.0f, 0.0f, 16000.0f);
// pid_init(&M3508_2, 1.50f, 0.0f, 10.0f, 16000.0f, 12000.0f, 0.0f,6000.0f);
// pid_init(&M3508_3, 1.50f, 0.0f, 10.0f, 16000.0f, 12000.0f, 0.0f,6000.0f);
// pid_init(&M3508_4, 1.50f, 0.0f, 10.0f, 16000.0f, 12000.0f, 0.0f,6000.0f);
	
//	pid_init(&DM10010L_Follow_Ang, -60.0f, 0.0f, 20.0f, 5000.0f, 5000.0f, 0.0f, 3000.0f);
//  pid_init(&DM10010L_Follow_Speed, 25.0f, 0.0f, 30.0f, 12000.0f, 11000.0f, 10000.0f,12000.0f);
//		
//	pid_init(&DM10010L_Ang, -0.0010f, 0.0f, 0.0f, 5000.0f, 5000.0f, 0.0f, 3000.0f);
}
