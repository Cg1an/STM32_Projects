#include "time.h"
#include "can.h"
#include "pid.h"
#include "sys.h"

#define a 0.9
float value1=0;      //低通滤波
int16_t Spend1=0.0f;     //***Spend1 电机1速度  Spend4  电机4速度
int16_t set_Spend=2000;
int16_t electricity1;   //电机1和4的电流值
//float  Corner;
float  lsat_electricity ;
float  limt_electricity=12000.0f;

void TIM4_PID_control(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);  ///使能TIM7时钟
	
  TIM_TimeBaseInitStructure.TIM_Period = arr;   //自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);//初始化TIM7
	
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE); //允许定时器6更新中断
	TIM_Cmd(TIM4,ENABLE); //开启定时器7
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn; //定时器6中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


void TIM4_IRQHandler(void)
{              
	
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET) //溢出中断
{	
	Spend1=read_spend();
	electricity1=Inremental_PI2(set_Spend , Spend1 );  //（设定，实际
	//electricity1=Low_Pass1(electricity1);
//	if(electricity1>=limt_electricity )electricity1=limt_electricity;   //limt_electricity=15000
//	if(electricity1<=-limt_electricity)  electricity1=-limt_electricity;  //电流1的限幅
	
	Set_motor_urrent(0,0,0,0);  //electricity1为电机1的电流值，

	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);  //清除中断标志位
}

}

float Low_Pass1(float data1)       //低通
{
   if(value1==0)
	 {
     value1=data1;
		 return value1;
	 }
	 else
	 {
     value1=(1-a)*data1+a*value1;
		 return value1;
	 }
}



