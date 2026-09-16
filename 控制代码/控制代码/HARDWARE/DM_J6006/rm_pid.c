#include "main.h"

PID pid[ALL_PID];		
float out[ALL_PID] = {0};





//pidֵ��ʼ��
static void pid_init(PID *pid, float kp, float ki, float kd, float poutmax, float ioutmax, float doutmax, float outmax)
{	
	pid->kp = kp;
	pid->ki = ki;
	pid->kd = kd;
	
	pid->poutmax = poutmax;
	pid->ioutmax = ioutmax;
	pid->doutmax = doutmax;
	pid->outmax = outmax;
	
	pid->f_pid_reset = pid_reset;
	pid->f_pid_reset(pid);
	
	pid->err = 0;
	pid->err_last = 0;
	pid->err_llast = 0;
	pid->integral = 0;
	
}
//pid���ֵ����
static void pid_reset(PID *pid)
{
	
	pid->pout = 0;
	pid->iout = 0;
	pid->dout = 0;
	pid->out  = 0;
	
}


void All_Pid_Configuration(PID pid[])
{	

		/*PITCH*/
	pid[PITCH_ANGLE].f_pid_init = pid_init;//28 0 0  30
	pid[PITCH_ANGLE].f_pid_init(&pid[PITCH_ANGLE], 20.0f, 0.0f, 0.0f, 500.0f, 50.0f, 100.0f, 500.0f);	
	pid[PITCH_SPEED].f_pid_init = pid_init;//33 0.18 50    200 0 30
	pid[PITCH_SPEED].f_pid_init(&pid[PITCH_SPEED], 100.0f, 0.01f, 0.0f, 30000.0f, 6000.0f, 5000.0f, 30000.0f);
	/*YAW*/
	pid[YAW_ANGLE].f_pid_init = pid_init;//15 0 0	
	pid[YAW_ANGLE].f_pid_init(&pid[YAW_ANGLE], 19.0f, 0.0f, 300.0f, 1000.0f, 50.0f, 500.0f, 1000.0f);
	pid[YAW_SPEED].f_pid_init = pid_init;//240 4 50
	pid[YAW_SPEED].f_pid_init(&pid[YAW_SPEED],350.0f, 0.0f, 50.0f, 30000.0f, 1000.0f, 30000.0f, 30000.0f); 

	/*FEED_MOTOR*/
	pid[FEED_MOTOR_SINGLE].f_pid_init = pid_init;
	pid[FEED_MOTOR_SINGLE].f_pid_init(&pid[FEED_MOTOR_SINGLE], 80.0f, 0.0f, 0.0f, 5000.0f, 0.0f, 0.0f, 5000.0f);	
	pid[FEED_MOTOR_DOUBLE_ANGLE].f_pid_init = pid_init;
	pid[FEED_MOTOR_DOUBLE_ANGLE].f_pid_init(&pid[FEED_MOTOR_DOUBLE_ANGLE], 12.5f, 0.0f, 100.0f, 5000.0f, 0.0f, 5000.0f, 5000.0f);
	/*FIRE_MOTOR*/
	pid[FIRE_MOTOR_SPEED_L].f_pid_init = pid_init;
	pid[FIRE_MOTOR_SPEED_L].f_pid_init(&pid[FIRE_MOTOR_SPEED_L], 15.0f, 0.0f, 0.0f, 15000.0f, 1000.0f, 0.0f, 15000.0f);
	pid[FIRE_MOTOR_SPEED_R].f_pid_init = pid_init;
	pid[FIRE_MOTOR_SPEED_R].f_pid_init(&pid[FIRE_MOTOR_SPEED_R], 15.0f, 0.0f, 0.0f, 15000.0f, 1000.0f, 0.0f, 15000.0f);
	/*VISION*/
	
	pid[YAW_ANGLE_BIG_ERROR].f_pid_init = pid_init;
	pid[YAW_ANGLE_BIG_ERROR].f_pid_init(&pid[YAW_ANGLE_BIG_ERROR], 16.0f, 0.0f, 150.0f, 1000.0f, 50.0f, 500.0f, 1000.0f);

	pid[YAW_SPEED_BIG_ERROR].f_pid_init = pid_init;
	pid[YAW_SPEED_BIG_ERROR].f_pid_init(&pid[YAW_SPEED_BIG_ERROR], 200.0f, 0.0f, 10.0f, 30000.0f, 1500.0f, 5000.0f, 30000.0f);

  pid[PITCH_ANGLE_BIG_ERROR].f_pid_init = pid_init;
	pid[PITCH_ANGLE_BIG_ERROR].f_pid_init(&pid[PITCH_ANGLE_BIG_ERROR],8.0f, 0.0f, 50.0f, 400.0f, 50.0f, 100.0f, 500.0f);

	pid[PITCH_SPEED_BIG_ERROR].f_pid_init = pid_init;
	pid[PITCH_SPEED_BIG_ERROR].f_pid_init(&pid[PITCH_SPEED_BIG_ERROR], 200.0f, 0.0f, 10.5f, 30000.0f, 6000.0f, 10000.0f, 30000.0f);
}

//pid��������ֵ 
float Calculate_Current_Value(PID *pid, float set, float real)
{
	//����������һ�ε����ֵ
	pid->f_pid_reset = pid_reset;
	pid->f_pid_reset(pid);
	
	pid->set = set ;
	pid->real = real;
	
	pid->err_last = pid->err;
	pid->err = pid->set - pid->real;
	pid->integral += pid->err;
	
	pid->pout = pid->kp * pid->err;
	pid->pout = pid->pout < pid->poutmax ? pid->pout : pid->poutmax;
	pid->pout = pid->pout > -pid->poutmax ? pid->pout : -pid->poutmax;
	
	pid->iout = pid->ki * pid->integral;
	pid->iout = pid->iout < pid->ioutmax  ? pid->iout : pid->ioutmax;
	pid->iout = pid->iout > -pid->ioutmax ? pid->iout : -pid->ioutmax;
	
	pid->dout = pid->kd * (pid->err - pid->err_last);
	pid->dout = pid->dout < pid->doutmax ? pid->dout : pid->doutmax;
	pid->dout = pid->dout > -pid->doutmax ? pid->dout : -pid->doutmax;
	
	pid->out = pid->pout + pid->iout + pid->dout;
	pid->out = pid->out < pid->outmax ? pid->out : pid->outmax;
	pid->out = pid->out > -pid->outmax ? pid->out : -pid->outmax;
	
	return pid->out;
}
float Calculate_Current_Value_For_Err(PID *pid, float err)
{
	pid->f_pid_reset = pid_reset;
	pid->f_pid_reset(pid);
	
	
	pid->err_last = pid->err;
	pid->err = err;
	pid->integral += pid->err;
	
	pid->pout = pid->kp * pid->err;
	pid->pout = pid->pout < pid->poutmax ? pid->pout : pid->poutmax;
	pid->pout = pid->pout > -pid->poutmax ? pid->pout : -pid->poutmax;
	
	pid->iout = pid->ki * pid->integral;
	pid->iout = pid->iout < pid->ioutmax  ? pid->iout : pid->ioutmax;
	pid->iout = pid->iout > -pid->ioutmax ? pid->iout : -pid->ioutmax;
	
	pid->dout = pid->kd * (pid->err - pid->err_last);
	pid->dout = pid->dout < pid->doutmax ? pid->dout : pid->doutmax;
	pid->dout = pid->dout > -pid->doutmax ? pid->dout : -pid->doutmax;
	
	pid->out = pid->pout + pid->iout + pid->dout;
	pid->out = pid->out < pid->outmax ? pid->out : pid->outmax;
	pid->out = pid->out > -pid->outmax ? pid->out : -pid->outmax;
	
	return pid->out;
}

