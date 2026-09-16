#ifndef		_RM_PID_H_
#define		_RM_PID_H_

#include "delay.h"

#define YAW_ANGLE_BIG_ERROR 0
#define YAW_SPEED_BIG_ERROR 1

#define PITCH_ANGLE_BIG_ERROR 2
#define PITCH_SPEED_BIG_ERROR 3

#define PITCH_ANGLE 4
#define PITCH_SPEED 5

#define YAW_ANGLE  6
#define YAW_SPEED 7

#define CHASSIS_FOLLOW 8

#define FEED_MOTOR_SINGLE 9
#define FEED_MOTOR_DOUBLE_ANGLE 10
#define FEED_MOTOR_DOUBLE_SPEED 11

#define FIRE_MOTOR_SPEED_L 12
#define FIRE_MOTOR_SPEED_R 13

#define ALL_PID 14

#define ALL_PID_INCR 6

typedef struct PID
{
	float kp;
	float ki;
	float kd;
	
	float pout;
	float iout;
	float dout;
	
	float poutmax;
	float ioutmax;
	float doutmax;
	float outmax;
	
	float set;
	float real;
	float out;
	
	float err;							//����ƫ��ֵ
	float err_last;					//��һ��ƫ��ֵ
	float err_llast;				//���ϴ�ƫ��ֵ
	float integral;					//�ۼ�ƫ��ֵ
	void(*f_pid_init)(struct PID *pid, float kp, float ki, float kd, float poutmax, float ioutmax, float doutmax, float outmax);			//������ʼ��pid
	void(*f_pid_reset)(struct PID *pid);
}PID;

typedef struct
{
	 //PID ������
    float Kp;
    float Ki;
    float Kd;

    float max_out;  //������
    float max_iout; //���������
	
    float set;
    float fdb;	

    float out;
    float Pout;
    float Iout;
    float Dout;
    float Dbuf[3];  //΢���� 0���� 1��һ�� 2���ϴ�
    float error[3]; //����� 0���� 1��һ�� 2���ϴ�	
	
}PID_INCR;


void pid_incr_init(PID_INCR *pid, const float PID[3], float max_out, float max_iout);
float pid_incr_calc(PID_INCR *pid, float ref, float set);
void ALL_Pid_Incr_Configuration(void);
extern PID_INCR pid_incr[ALL_PID_INCR];
extern float out_incr[ALL_PID_INCR];
extern float out_limit[ALL_PID_INCR];

extern PID pid[ALL_PID];
extern float out[ALL_PID];
static void pid_init(PID *pid, float kp, float ki, float kd, float poutmax, float ioutmax, float doutmax, float outmax);
static void pid_reset(PID *pid);

void All_Pid_Configuration(PID pid[]);
float Calculate_Current_Value(PID *pid, float set, float real);
float Calculate_Current_Value_For_Err(PID *pid, float err);
void Look(void);


#endif
