#ifndef __PID_H__
#define __PID_H__
#include "main.h"

typedef struct PIDset
  {  
		 
     double PMAX;
		 double IMAX;
		 double DMAX;
		 double Sum_error_max;
		
		 float OMAX;
		
		 float Kp,Ki,Kd;
		
		 float Out;
		 
		 float Pout;
		 float Iout;
		 float Dout;
		
     float error[3];     
		 float sum_error;
		 
     float set;
     float real;
  }PID;
	
void Chassis_PID_Init(void);
float  PID_Cal_position(PID *Pid, float real, double set);//get为实际值  set为目标值     _位置式
void pid_init(PID *pid,float kp,float ki,float kd,float Sum_error_max, float pmax, float imax, float dmax,float omax);

#endif

