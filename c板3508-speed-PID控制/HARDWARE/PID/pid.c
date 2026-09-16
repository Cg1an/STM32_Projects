#include "pid.h"
#include "sys.h"
#include "math.h"

#define S_P	3.98  
#define S_I	0.0  
#define S_D	0.0

struct _pid       //pid结构体部分	
{ 
//速度pid		
	float SetSpeed;   //设定值
	float ActualSpeed;   //实际值
	float Serr;     //偏差
	float Serr_last;  //上上一个偏差
  float Serr_next;  //上一个偏差
	float SKp,SKi,SKd;   //系数
	float Svoltage;   //电压值
 	float Sintegral;    //积分值
	float Svoltage1;
	
//位置pid		
  float SetLocation;   //设定值
	float ActualLocation;   //实际值
	float Lerr;     //偏差
	float Lerr_last;  //上一个偏差
	float LKp,LKi,LKd;   //系数
	float Lvoltage;   //电压值
 	float Lintegral;    //积分值
	float Lvoltage1;	
}pid;

void PID_init()     //pid初始化
{ 
   pid.ActualSpeed=0.0;
   pid.SetSpeed=0.0;
   pid.Serr=0.0;
   pid.Serr_last=0.0;
	 pid.Serr_next=0.0;
   pid.Sintegral=0.0;
   pid.SKd=1.19;     //1.15    6.85
   pid.SKi=0.068 ;     //0.06   0.2        0.05
   pid.SKp=1.87;     //1.98    1.58       1.21  1.98
   pid.Svoltage=0.0;
  // pid.Svoltage1=0.0;
}

extern   u8 res;


int16_t Inremental_PI2(float s_spend,float a_spend)	//spend1  速度  位置式
{
	int index;
	 pid.SetSpeed=s_spend;
	 pid.ActualSpeed=a_spend;
	 pid.Serr=pid.SetSpeed-pid.ActualSpeed;
	if( pid.Serr>300||pid.Serr<(-300))
	{
    index=0;	
	}
	else
	{
    index=1;
		pid.Sintegral+=pid.Serr;
	}
 	 pid.Svoltage=pid.SKp*pid.Serr+pid.SKi*pid.Sintegral+pid.SKd*(pid.Serr- pid.Lerr_last);
	 pid.Lerr_last=pid.Serr;
	
	 return pid.Svoltage;	
}
//int16_t Inremental_PI2(float s_spend,float a_spend)	//spend1    
//{
//	 pid.SetSpeed=s_spend;
//	
//	 pid.ActualSpeed=a_spend;

//	 pid.Serr=pid.SetSpeed-pid.ActualSpeed;
//	
// 	 pid.Svoltage+=pid.SKp*(pid.Serr-pid.Serr_last)+pid.SKi*pid.Serr+pid.LKd*(pid.Serr-pid.Serr_last);
//	
//	 pid.Serr_last=pid.Serr;
//	
//	 return pid.Svoltage;	
//}



//float spend_control( float left_spend,float right_spend ,int16_t set_spend )    //输入值为左，右电机速度，目标速度
//{
//   static float Velocity,Encoder_Least,Encoder;     //速度 ， 上一个编码器， 编码器
//   static float Encoder_Integral;      //编码器积分
//   float spendkp = -120.0f, spendki=-0.060f;
//   float spend_Least,spend;
//   float spend_Integral;               //速度积分       
//	spend_Least =(float)(left_spend+right_spend)-(float)set_spend; //=获取最新速度偏差测量速度（左右编码器之和）-目标速度（此处为零）
//  spend = 0.8; //===一阶低通滤波器
//  spend += spend_Least*0.02; //===一阶低通滤波器
//  spend_Integral +=spend; //===积分出位移 积分时间：10ms 
//  if(spend_Integral>10000) spend_Integral=10000; //===积分限幅 
//  if(spend_Integral<-10000) spend_Integral=-10000; //===积分限幅
//  Velocity=spendkp*spend+spendki*spend_Integral; //===速度控制
// 
//	return Velocity;
//}









