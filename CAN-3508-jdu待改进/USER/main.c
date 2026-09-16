#include "stm32f4xx.h"
#include "usart.h"
#include "delay.h"
#include "can.h"   //PA11（RX）,PA12(TX)
#include "time.h"
#include "pid.h"


int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);    //初始化延时函数
	uart_init(115200);	//初始化串口波特率为115200
  CAN1_Mode_Init();
	PID_init();
	TIM4_PID_control(10000-1,84-1);		

	
  while(1)
 {
	
 }

}



