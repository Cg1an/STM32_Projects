#include "sys.h"
#include "usart.h"	  
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif
//////////////////////////////////////////////////////////////////////////////////	   
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//串口1初始化 
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2014/5/2
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved
//********************************************************************************
#define ResLength_U1 20
#define ResLength_U2 6
//#define ResLength_U3 20
#define ResLength_U3 9


#define MAX_MOTOR_RPM    3000.0f
#define WHEEL_TRACK_M    (0.59f * 1.2f)  // 轮距 (米) 
#define WHEEL_RADIUS_M   0.12f           // 轮半径 (米)
#define GEAR_RATIO       19.0f           // 减速比
#define MPS_TO_RPM       ((60.0f * GEAR_RATIO) / (2.0f * 3.14159f * WHEEL_RADIUS_M))

int8_t out_cmd_l = 0;  // 左轮最终输出百分比
int8_t out_cmd_r = 0;  // 右轮最终输出百分比

// 联合体：用于字节与 IEEE-754 浮点数互转 (原生小端序解算核心)
typedef union
{
    float value;
    uint8_t bytes[4];
} float_union_t;

u8 TransEmpty = 1;
int ResCNT_U1, ResCNT_U2=0, ResCNT_U3=-2;
u8 ResBuf_U1[100], ResBuf_U2[100], ResBuf_U3[100];
u8 TrsBuf_U1[100], TrsBuf_U2[100], TrsBuf_U3[100];

float HCMD=0, VCMD=0;

#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef’ d in stdio.h. */ 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif 
//end
//////////////////////////////////////////////////////////////////

//串口1中断服务程序
void USART1_IRQHandler(void)
{
	u8 res;	

	if(((USART1->SR)&(0x20)) != 0)
	{
		USART1->SR &= 0xffffffdf;
		res=USART1->DR;
	}
	
	if((res == 0xa5)&&(ResCNT_U1 == -2))//握手字头
	{
		ResCNT_U1 = -1;
	}
	else if((res == 0xff)&&(ResCNT_U1 == -1))//握手字头
	{
		ResCNT_U1 = 0;
	}
	else if((ResCNT_U1 > -1)&&(ResCNT_U1 < ResLength_U1))//接收信息
	{
		ResBuf_U1[ResCNT_U1++] = res;
		if(ResCNT_U1 == ResLength_U1)
		{
			ResCNT_U1 = -2;
			
		}
	}
	else
	{
		ResCNT_U1 = -2;
	}
} 
//-------------------------end------------------------------

void USART2_IRQHandler(void)
{
  uint8_t res;
	if(((USART2->SR)&(0x20)) != 0)
	{
		USART2->SR &= 0xffffffdf;
		res=USART2->DR;
	}
	
//	ResBuf_U2[ResCNT_U2++] = res;
//	if(ResCNT_U2 == ResLength_U2)
//	{
//		ResCNT_U2 = 0;
//		
//	}
//	return;
	
	if((res == 0xa5)&&(ResCNT_U2 == -2))//握手字头
	{
		ResCNT_U2 = -1;
	}
	else if((res == 0xff)&&(ResCNT_U2 == -1))//握手字头
	{
		ResCNT_U2 = 0;
	}
	else if((ResCNT_U2 > -1)&&(ResCNT_U2 < ResLength_U2))//接收信息
	{
		ResBuf_U2[ResCNT_U2++] = res;
		if(ResCNT_U2 == ResLength_U2)
		{
			ResCNT_U2 = -2;
			HCMD = ResBuf_U2[1];
			HCMD /= 256;
			if(ResBuf_U2[0] == 0) HCMD *= -1;
			VCMD = ResBuf_U2[3];
			VCMD /= 256;
			if(ResBuf_U2[2] == 0) VCMD *= -1;
		}
	}
	else
	{
		ResCNT_U2 = -2;
	}
}
//-------------------------end------------------------------

uint16_t MB_CRC16(uint8_t * array,uint8_t index)
{
	uint16_t tmp = 0xffff;
 
	for(int n = 0; n < index; n++)/*此处的6 -- 要校验的位数为6个*/
	{
		tmp = array[n] ^ tmp;
		for(int i = 0;i < 8;i++)/*此处的8 -- 指每一个char类型又8bit，每bit都要处理*/
		{  
			if(tmp & 0x01)
			{
				tmp = tmp >> 1;
				tmp = tmp ^ 0xa001;
			}
			else
			{
				tmp = tmp >> 1;
			}
		}
	}
	return (tmp);
}
//-------------------------end------------------------------

//SBus处理函数：
void Process(uint8_t* raw,uint16_t* result)
{
  uint8_t bitsToRead=3; // bitsToRead表示需要从下一个字节中读取多少bit。规律：bitsToRead 每次总是增加 3
  uint8_t bitsToShift;
  uint8_t startByte=21;
  uint8_t channelId=15;

  do
  {
    result[channelId]=raw[startByte];

    if(bitsToRead<=8)
    {
      result[channelId]<<=bitsToRead;
      bitsToShift=8-bitsToRead;
      result[channelId]+=(raw[startByte-1]>>bitsToShift);
    }
    else
    {
      result[channelId]<<=8;
      result[channelId]+=raw[startByte-1];
      startByte--;
      bitsToRead-=8;
      result[channelId]<<=bitsToRead;
      bitsToShift=8-bitsToRead;
      result[channelId]+=(raw[startByte-1]>>bitsToShift);
    }

    result[channelId]&=0x7FF;

    channelId--;
    startByte--;
    bitsToRead+=3;

  }while(startByte>0);  
}
//-------------------------end------------------------------

void Wired_Send()
{
	TrsBuf_U2[0] = 0xa5;
	TrsBuf_U2[1] = 0xff;
	TrsBuf_U2[2] = 2;
	TrsBuf_U2[3] = 3;
	TrsBuf_U2[4] = 4;
	TrsBuf_U2[5] = 5;
	TrsBuf_U2[6] = 6;
	TrsBuf_U2[7] = 0x5a;
	MYDMA_Enable(DMA1_Stream6, (u32)TrsBuf_U2, 8);
}

void USART3_IRQHandler(void)
{
  uint8_t res;
	if(((USART3->SR)&(0x20)) != 0)
	{
		USART3->SR &= 0xffffffdf;
		res=USART3->DR;
	}
	
	if((res == 0xaa)&&(ResCNT_U3 == -2))//握手字头
	{
		ResCNT_U3 = -1;
	}
	else if((res == 0x55)&&(ResCNT_U3 == -1))//握手字头
	{
		ResCNT_U3 = 0;
	}
//	else if((ResCNT_U3 > -1)&&(ResCNT_U3 < 11))//接收信息
	else if((ResCNT_U3 > -1)&&(ResCNT_U3 < ResLength_U3))//接收信息
	{
		ResBuf_U3[ResCNT_U3++] = res;
		if(ResCNT_U3 == ResLength_U3)
		{
			ResCNT_U3 = -2;
			ResCNT_U3 = -2; // 状态机复位，准备接收下一帧帧头
			
            // ================== 开始校验与速度解算 ==================
            uint8_t checksum = 0;
            
            // 计算校验和：累加前 8 个数据字节
            for (int i = 0; i < 8; i++)
            {
                checksum += ResBuf_U3[i]; 
            }

            // 如果计算出的校验和等于收到的第 9 个字节（数组下标为 8）
            if (checksum == ResBuf_U3[8])
            {
                float_union_t v_union, w_union;

                // 提取线速度 Vx (底层小端序内存映射)
                v_union.bytes[0] = ResBuf_U3[0];
                v_union.bytes[1] = ResBuf_U3[1];
                v_union.bytes[2] = ResBuf_U3[2];
                v_union.bytes[3] = ResBuf_U3[3];

                // 提取角速度 Az (底层小端序内存映射)
                w_union.bytes[0] = ResBuf_U3[4];
                w_union.bytes[1] = ResBuf_U3[5];
                w_union.bytes[2] = ResBuf_U3[6];
                w_union.bytes[3] = ResBuf_U3[7];

                // 提取浮点数速度
                float target_vx = v_union.value;
                float target_az = w_union.value;
                
                // ==========================================================
                //  1. 运动学逆解：将 Vx 和 Az 转化为左右轮目标 RPM
                // ==========================================================
                float target_rpm_l = (target_vx - target_az * WHEEL_TRACK_M / 2.0f) * MPS_TO_RPM;
                float target_rpm_r = (target_vx + target_az * WHEEL_TRACK_M / 2.0f) * MPS_TO_RPM;

                // ==========================================================
                //  2. 转换为最大转速的百分比 (-100.0 到 +100.0)
                // ==========================================================
                float percent_l = (target_rpm_l / MAX_MOTOR_RPM) * 100.0f;
                float percent_r = (target_rpm_r / MAX_MOTOR_RPM) * 100.0f;

                // ==========================================================
                //  3. 严格的限幅保护
                // ==========================================================
                if (percent_l > 100.0f) percent_l = 100.0f;
                if (percent_l < -100.0f) percent_l = -100.0f;
                if (percent_r > 100.0f) percent_r = 100.0f;
                if (percent_r < -100.0f) percent_r = -100.0f;

                // 强转为 8 位有符号整数，并直接赋值给全局变量
                out_cmd_l = (int8_t)percent_l;
                out_cmd_r = (int8_t)percent_r;
            }	
		   }
	}
	else
	{
		ResCNT_U3 = -2;
	}
}
//-------------------------end------------------------------


//USART3清除发送完成中断标志
void DMA1_Stream3_IRQHandler(void)
{
	DMA1->LIFCR |=1<<27; 
}
//-------------------------end------------------------------

//USART2清除发送完成中断标志
void DMA1_Stream6_IRQHandler(void)
{
	DMA1->HIFCR |=1<<21; 
}
//-------------------------end------------------------------

//USART1清除发送完成中断标志
void DMA2_Stream7_IRQHandler(void)
{
	DMA2->HIFCR |=1<<27; 
	TransEmpty = 1;
}
//-------------------------end------------------------------


//初始化IO 串口1
//pclk2:PCLK2时钟频率(Mhz)
//bound:波特率 
void uart_init()
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;
	
	RCC->AHB1ENR|=1<<0;   //使能PORTA口时钟  
	RCC->AHB1ENR|=1<<1;   //使能PORTB口时钟  
	RCC->APB2ENR|=1<<4;  	//使能串口1时钟 
	RCC->APB1ENR|=1<<17; 	//UART2时钟使能
	RCC->APB1ENR|=1<<18; 	//UART3时钟使能
	
	GPIO_Set(GPIOA,PIN2|PIN3|PIN9|PIN10,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_NONE);//PA9,PA10,复用功能,上拉输出
 	GPIO_Set(GPIOB,PIN10|PIN11,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_NONE);//PA9,PA10,复用功能,上拉输出
 	GPIO_AF_Set(GPIOA,9,7);	 //AF7,UART1
	GPIO_AF_Set(GPIOA,10,7); //AF7,UART1
	GPIO_AF_Set(GPIOA,2,7);	//AF7,UART2
	GPIO_AF_Set(GPIOA,3,7);	//AF7,UART2
	GPIO_AF_Set(GPIOB,10,7);	//AF7,UART3
	GPIO_AF_Set(GPIOB,11,7);	//AF7,UART3
	
	
	//波特率设置
	temp=(float)(84*1000000)/(115200*16);//得到USARTDIV@OVER8=0
	mantissa=temp;				 //得到整数部分
	fraction=(temp-mantissa)*16; //得到小数部分@OVER8=0 
    mantissa<<=4;
	mantissa+=fraction; 
 	USART1->BRR=mantissa; 	//波特率设置
	USART1->CR1&=~(1<<15); 	//设置OVER8=0
	USART1->CR1|=1<<3;  	//串口发送使能 
	USART1->CR3=1<<7;     //DMA enable transmitter
	USART1->CR1|=1<<2;  	//串口接收使能
	USART1->CR1|=1<<5;    	//接收缓冲区非空中断使能	    	
	MYDMA_Config(DMA2_Stream7,4,M2P,(u32)&USART1->DR);
	MY_NVIC_Init(3,3,DMA2_Stream7_IRQn,2);
	MY_NVIC_Init(3,3,USART1_IRQn,2);//组2，最低优先级 
	USART1->CR1|=1<<13;  	//串口使能
	
	
	//波特率设置
	temp=(float)(42*1000000)/(9600*16);//得到USARTDIV@OVER8=0
	mantissa=temp;				 //得到整数部分
	fraction=(temp-mantissa)*16; //得到小数部分@OVER8=0 
    mantissa<<=4;
	mantissa+=fraction; 
 	USART2->BRR=mantissa; 	//波特率设置
	USART2->CR1&=~(1<<15); 	//设置OVER8=0
	USART2->CR1|=1<<3;  	  //串口发送使能
	USART2->CR3=1<<7;  //DMA enable transmitter
	USART2->CR1|=1<<2;  	  //串口接收使能
	USART2->CR1|=1<<5;    	//接收缓冲区非空中断使能
	MYDMA_Config(DMA1_Stream6,4,M2P,(u32)&USART2->DR);
	MY_NVIC_Init(3,1,USART2_IRQn,2);//抢占3，子优先级2，组2
	MY_NVIC_Init(3,3,DMA1_Stream6_IRQn,2);
	USART2->CR1|=1<<13;  	//串口使能

	
	USART3->BRR=mantissa; 	//波特率设置
	USART3->CR1&=~(1<<15); 	//设置OVER8=0
	USART3->CR1|=1<<3;  	  //串口发送使能
	USART3->CR3=1<<7;       //DMA enable transmitter
	USART3->CR1|=1<<2;  	  //串口接收使能
	USART3->CR1|=1<<5;    	//接收缓冲区非空中断使能
	MYDMA_Config(DMA1_Stream3,4,M2P,(u32)&USART3->DR);
	MY_NVIC_Init(3,2,USART3_IRQn,2);//抢占3，子优先级2，组2
	MY_NVIC_Init(3,0,DMA1_Stream3_IRQn,2);
	USART3->CR1|=1<<13;  	//串口使能
}
//-------------------------end------------------------------

