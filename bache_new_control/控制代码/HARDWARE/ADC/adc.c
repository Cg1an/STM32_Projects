#include "adc.h"
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

void (* ADCIntHandler)(void);
void ADCInitIntHandler(void);
void ADCRegularIntHandler(void);
void InitCurrOffSet(void);

float Co_curr = 1.0f;//10A--4096
u32 CurrSubCounter[3]={0,0,0}, CurrCounter=0;//初始化时，用于计算curr_Adj。
u16 curr1[5000], Curr1[16];
u16 curr2[5000], Curr2[16];
u16 curr3[5000], Curr3[16];
u32 cIndex = 0;
u8 cCounter = 0;
u8 ccIndex = 0;
u8 flag = 0;
u32 t1, t2;

u32 st1=0, st2;

//初始化ADC
void  Adc_Init(void)
{    
	//先初始化IO口
 	RCC->APB2ENR|=1<<8;    	//使能ADC1时钟 
	RCC->APB2ENR|=1<<9;    	//使能ADC2时钟 
	RCC->APB1ENR|=1<<29;    //使能DAC时钟 
	RCC->AHB1ENR|=1<<0;    	//使能PORTA时钟	  
	RCC->AHB1ENR|=1<<2;    	//使能PORTC时钟	  
//	GPIO_Set(GPIOA,PIN4,GPIO_MODE_A,0,0,GPIO_PUPD_PU);	//PA6, 模拟输出,下拉   
//	GPIO_Set(GPIOA,PIN5,GPIO_MODE_A,0,0,GPIO_PUPD_PU);	//PA7, 模拟输出,下拉   
	GPIO_Set(GPIOA,PIN6,GPIO_MODE_A,0,0,GPIO_PUPD_PU);	//PA6, 模拟输入,下拉   
	GPIO_Set(GPIOA,PIN7,GPIO_MODE_A,0,0,GPIO_PUPD_PU);	//PA7, 模拟输入,下拉   
	GPIO_Set(GPIOC,PIN4,GPIO_MODE_A,0,0,GPIO_PUPD_PU);	//PC4, 模拟输入,下拉   

	RCC->APB2RSTR|=1<<8;   	//ADCs复位
	RCC->APB2RSTR&=~(1<<8);	//复位结束	 
	ADC->CCR=1<<16;			//ADCCLK=PCLK2/4=84/4=21Mhz,ADC时钟最好不要超过36Mhz
 	
	ADC1->CR1=0;   			//CR1设置清零
	ADC1->CR2=0;   			//CR2设置清零
	ADC1->CR1|=0<<24;   //12位模式
	ADC1->CR1|=1<<8;    //扫描模式	
	ADC1->CR1|=1<<7;    //注入通道转换完成中断使能
	
	ADC1->CR2&=~(1<<1);    	//单次转换模式
 	ADC1->CR2&=~(1<<11);   	//右对齐	
	ADC1->CR2|=1<<20;    	  //上升沿触发注入通道
	ADC1->CR2|=0<<16;    	  //TIM1CH4触发注入通道
	//ADC1->CR2|=3<<8;    	//使能DMA
	
	ADC1->JSQR&=~(0X003fffff);
	ADC1->JSQR|=2<<20;     	//3个转换在注入序列中
	
	ADC1->JSQR|=7<<5;     	//通道7，A7   Channel_A
	ADC1->JSQR|=14<<10;     //通道14，C4  Channel_B
	ADC1->JSQR|=6<<15;     	//通道6，A6   Channel_C
	
	//设置通道的采样时间
	ADC1->SMPR1&=~(0x07ffffff);//通道采样时间清空	  
	ADC1->SMPR2&=~(0x3fffffff);//通道采样时间清空	  
 	ADC1->SMPR1|=2<<12; 	//通道14 28个周期,提高采样时间可以提高精确度，但是由于采样窗口的限制，不能太长。	 
	ADC1->SMPR2|=2<<18; 	//通道6  28个周期, 
	ADC1->SMPR2|=2<<21; 	//通道7  28个周期, 
	

	ADCIntHandler = ADCInitIntHandler;
	
	MY_NVIC_Init(1,3,ADC_IRQn,2);	//抢占1，子优先级3，组2		
	ADC1->CR2|=1<<0;	   	//开启AD转换器	  
	DAC->CR |= 0x00010001;
	
	InitCurrOffSet();
}
//---------------end-----------------

//获取电压最高的那一相（不进行电流采集）
u8 GetSkipNum()
{
	if((FOC.ElectricalDegCMD < PI_d3)||(FOC.ElectricalDegCMD > (PI_m2 - PI_d3))) return 0;
	if(FOC.ElectricalDegCMD < PI) return 1;
	return 2;
}
//---------------end-----------------

//初始化时候的ADC中断回调函(采集ADC的采样零偏)
void ADCInitIntHandler()
{
	//U固定由ADC1采集，如果U被剔除，则ADC1采集V；W固定由ADC2采集，如果W被剔除，则ADC2采集V。
	if(ADC1->SR&0X0004)//ADC1和ADC2都采集完成（JEOC中断）
	{
		ADC1->SR &= 0;
		FOC.curr_Adj[0] += ADC1->JDR1; CurrSubCounter[0] ++;
		FOC.curr_Adj[1] += ADC1->JDR2; CurrSubCounter[1] ++;
		FOC.curr_Adj[2] += ADC1->JDR3; CurrSubCounter[2] ++;
		CurrCounter++;
	}
}
//---------------end-----------------

//正常运行时候的ADC中断回调函数
void ADCRegularIntHandler()
{
	u8 skipNum;//被剔除的那一相：U-0，V-1，W-2。
	if(ADC1->SR&0X0004)//ADC1和ADC2都采集完成（JEOC中断）
	{
		ADC1->SR &= 0;
		
		skipNum = GetSkipNum();
		FOC.Current[0] = Co_curr*(FOC.curr_Adj[0] - ADC1->JDR1);
		FOC.Current[1] = Co_curr*(FOC.curr_Adj[1] - ADC1->JDR2);
		FOC.Current[2] = Co_curr*(FOC.curr_Adj[2] - ADC1->JDR3);
		
		if(skipNum == 0)      FOC.Current[0] = - FOC.Current[1] - FOC.Current[2];
		else if(skipNum == 1) FOC.Current[1] = - FOC.Current[0] - FOC.Current[2];
		else                  FOC.Current[2] = - FOC.Current[0] - FOC.Current[1];
			
		if(flag == 0) 
		{
			flag = 1;
			PAout(1) = 1;
			t1 = TIM5->CNT;
		}
		else
		{
			flag = 0;
			PAout(1) = 0;
			t2 = TIM5->CNT - t1;
		}
	}
}
//---------------end-----------------

//计算ADC零偏
void InitCurrOffSet()
{
	while(CurrCounter < 384);
	
	ADCIntHandler = ADCRegularIntHandler;
	
	FOC.curr_Adj[0]/=CurrSubCounter[0];
	FOC.curr_Adj[1]/=CurrSubCounter[1];
	FOC.curr_Adj[2]/=CurrSubCounter[2];
}
//---------------end-----------------

//ADC中断服务程序	 
void ADC_IRQHandler(void)
{
	ADCIntHandler();
}
//---------------end-----------------
