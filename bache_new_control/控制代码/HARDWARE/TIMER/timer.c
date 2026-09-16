#include "timer.h"
#include "led.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//定时器 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/4
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved
//********************************************************************************
//修改说明
//V1.1 20140504
//新增TIM14_PWM_Init函数,用于PWM输出
////////////////////////////////////////////////////////////////////////////////// 	 

uint16_t LaserSig[66];
s32 Cnt1, Cnt2, pCnt2;

u8 Tick;

//定时器5中断服务程序	 
void TIM5_IRQHandler(void)
{ 		    		  		
	u32 counter;
	if(TIM5->SR&0X0002)//通道1中断
	{
		counter = TIM5->CCR1;
		if(PAin(0))
		{
			Cnt1 = counter;
		}
		else
		{
			if(counter > Cnt1)
				Cnt2 = counter - Cnt1;
			else
				Cnt2 = ARR_TIM5 - Cnt1 + counter;
			
			if(Cnt2 > 5000) Cnt2 = pCnt2;//如果采集到的编码器脉宽不正常，采用上次的。
			else pCnt2 = Cnt2;
		}
	}
	
	TIM5->SR&=~(1<<1);//清除中断标志位 	    
}
//--------------end-----------------


//定时器7中断服务程序	 
void TIM7_IRQHandler(void)
{ 		    		  		
	if(TIM7->SR&0X0001)//溢出中断
	{
		TIM7->SR&=~(1<<0);//清除中断标志位 	    
		Tick = 1;
	}
}
//--------------end-----------------


//TIM1 PWM初始化 
//PWM输出初始化
void PWM_Init()
{		 					 
	RCC->APB2ENR|=1<<0; 	//TIM1时钟使能    
	RCC->APB1ENR|=1<<3;	  //TIM5时钟使能  
	RCC->APB1ENR|=1<<5;	  //TIM7时钟使能  
	RCC->AHB1ENR|=1<<0;   //使能PORTA时钟	
	RCC->AHB1ENR|=1<<1;   //使能PORTB时钟	
	GPIO_Set(GPIOA,PIN1,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);//复用功能,上拉输出
	GPIO_Set(GPIOA,PIN0|PIN8|PIN9|PIN10|PIN11,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);//复用功能,上拉输出
	GPIO_Set(GPIOB,PIN13|PIN14|PIN15,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);
	GPIO_AF_Set(GPIOA,0,2);	//AF2 
	GPIO_AF_Set(GPIOA,8,1);	//AF1 
	GPIO_AF_Set(GPIOA,9,1);	//AF1 
	GPIO_AF_Set(GPIOA,10,1);	//AF1 
	GPIO_AF_Set(GPIOA,11,1);	//AF1 
	GPIO_AF_Set(GPIOB,13,1);	//AF1 
	GPIO_AF_Set(GPIOB,14,1);	//AF1 
	GPIO_AF_Set(GPIOB,15,1);	//AF1 
	
	TIM1->ARR = 2000;	    //设定计数器自动重装值，PWM周期2ms
	TIM1->PSC = 167;		  //预分频器，167
	TIM1->CCMR1|=6<<4;  	//CH1 PWM1模式（channel 1 is active as long as TIMx_CNT<TIMx_CCR1）
//	TIM1->CCMR1|=6<<12;  	//CH2 PWM1模式
//	TIM1->CCMR2|=6<<4;  	//CH3 PWM1模式
//	TIM1->CCMR2|=7<<12;  	//CH4 PWM2模式
	TIM1->CCMR1|=1<<3; 		//CH1 预装载使能
//	TIM1->CCMR1|=1<<11; 	//CH2 预装载使能
//	TIM1->CCMR2|=1<<3; 		//CH3 预装载使能
//	TIM1->CCMR2|=1<<11; 	//CH4 预装载使能
	TIM1->CCER|=0x01;     //OC1输出使能，高电平有效
	TIM1->EGR|=0x01;      //马上装载PSC	
	
	TIM1->CR1|=0<<5;    	 //普通计数模式1
	//TIM1->CR1|=1<<8;     //tDTS=2*tCK_INT
	TIM1->CR1|=1<<0;    	 //使能定时器 	
	TIM1->DIER |= 0x01<<9; //使能CC1的DMA_Request
	TIM1->BDTR|=0xC08<<0;  //设置死区时间
	TIM1->BDTR|=0x8000;    //总使能定时器
	
	MYDMA_Config(DMA2_Stream1,6,M2P,(u32)&TIM1->CCR1);//DMA2,STEAM1,CH6,外设为TIM1,存储器为SendBuff,长度为:SEND_BUF_SIZE.
	DMA2_Stream1->CR|=1<<11;		//外设数据长度:16位
	DMA2_Stream1->CR|=1<<13;		//存储器数据长度:16位
	MY_NVIC_Init(3,3,DMA2_Stream1_IRQn,2);
		 
	//TIM1->CCR4 = FOC.Val_ARR - 1;
	
 	TIM5->ARR=ARR_TIM5;  	//设定计数器自动重装值
	TIM5->PSC=20;  	        //预分频器（0.25us）
	TIM5->CCMR1&=((uint16_t)0x0000);	//清洗CCxS位，并设置为不分频不滤波
	TIM5->CCMR1|=((uint16_t)0x0001);	//配置CCxS位
	TIM5->CCER|=0x00ff; 		//通道1：CC1P=1,CC1NP=1(上升沿与下降沿都捕获)，同时使能通道
	TIM5->DIER|=1<<1;       //允许通道1中断
	TIM5->CR1|=0x01;        //使能定时器5
  MY_NVIC_Init(0,3,TIM5_IRQn,2);	//抢占0，子优先级3，组2
	
	TIM7->ARR=ARR_TIM7;  	//设定计数器自动重装值
	TIM7->PSC=83;  	        //预分频器（1us）
	TIM7->DIER|=1<<0;       //允许溢出中断
	TIM7->CR1|=0x01;        //使能定时器7
  MY_NVIC_Init(0,2,TIM7_IRQn,2);	//抢占0，子优先级3，组2
}
//--------------end-----------------





