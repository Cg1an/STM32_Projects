#include "DShot.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 

#include "Math.h"
#define Motor_PIN                  	GPIO_Pin_11                 
#define Motor_GPIO_PORT            	GPIOA                      
#define Motor_GPIO_CLK             	RCC_AHB1Periph_GPIOA
#define Motor_TIM                  	TIM1  
//#define	Low_vol										 	7
//#define	High_vol											14
//#define	Low_vol										 	1750
//#define	High_vol											3500
#define	Low_vol										 	150
#define	High_vol										900
//H:6/8
//L:3/8


/******************************************************************************************************************
*                         void TIM1_Motor_PWM_Init(u16 arr,u16 psc)	
*Description : TIM1电机PWM_dma初始化 
*Arguments   : arr：自动重装值 psc：时钟预分
*Returns     : none
*Notes       : none
*******************************************************************************************************************
*/
uint16_t data[18];
uint16_t Send_data[16];
u16 tick = 0, tickCNT = 0, Veltick = 0;
u8 EnableTelemetry = 1, Telemetry = 0;
DSCMD DShot_CMD;
Deflection deflectionCMD;


/******************************************************************************************************************
*                         SendSignal(uint16_t *motor)	
*Description : DMA发送函数
*Arguments   : motor发送的数据
*Returns     : none
*Notes       : none
*******************************************************************************************************************
*/
void SendSignal(uint16_t *Signal)
{
	MYDMA_Enable(DMA2_Stream1, (u32)Signal, 66);
	TIM1->CR1|=1<<0;    	//使能定时器 	
}


void DMA2_Stream1_IRQHandler(void)
{
	DMA2->LIFCR |=1<<11; //TIM1清除发送完成中断标志
	TIM1->CR1&=~((uint32_t)(1<<0));    	//去使能定时器 	
}
//-------------------------end------------------------------


/******************************************************************************************************************
*                        u16 add_checksum_and_telemetry(u16 packet, u8 telem)
*Description : 校验位解算
*Arguments   : packet油门值 			telem回传位
*Returns     : none
*Notes       : none
*******************************************************************************************************************
*/
u16 add_checksum_and_telemetry(u16 packet, u8 telem) {
    u16 packet_telemetry = (packet << 1) | (telem & 1);
    u8 i;
    u16 csum = 0;
    u16 csum_data = packet_telemetry;

    for (i = 0; i < 3; i++) {
        csum ^=  csum_data;   																// xor data by nibbles
        csum_data >>= 4;
    }
		//csum = ~csum;
    csum &= 0xf;
    return (packet_telemetry << 4) | csum;    								//append checksum
}
/******************************************************************************************************************
*                       pwmWriteDigital(uint16_t *esc_cmd, int value)
*Description : 生成16位数据
*Arguments   : esc_cmd生成的位			value油门值
*Returns     : none
*Notes       : none
*******************************************************************************************************************
*/
void pwmWriteDigital(int value)
{
	u8 i;
	for(i=0; i<4; i++)
	{
		LaserSig[i*16 + 0]  = High_vol;
		LaserSig[i*16 + 1]  = High_vol;
		LaserSig[i*16 + 2]  = High_vol;
		LaserSig[i*16 + 3]  = High_vol;
		LaserSig[i*16 + 4]  = High_vol;
		LaserSig[i*16 + 5]  = High_vol;
		LaserSig[i*16 + 6]  = High_vol;
		LaserSig[i*16 + 7]  = Low_vol;
		LaserSig[i*16 + 8]  = (value & 0x0080) ? High_vol :  Low_vol ;
		LaserSig[i*16 + 9]  = (value & 0x0040) ? High_vol :  Low_vol ;
		LaserSig[i*16 + 10] = (value & 0x0020) ? High_vol :  Low_vol ;
		LaserSig[i*16 + 11] = (value & 0x0010) ? High_vol :  Low_vol ;   
		LaserSig[i*16 + 12] = (value & 0x8) ? High_vol : Low_vol;
		LaserSig[i*16 + 13] = (value & 0x4) ? High_vol : Low_vol;
		LaserSig[i*16 + 14] = (value & 0x2) ? High_vol : Low_vol;
		LaserSig[i*16 + 15] = (value & 0x1) ? High_vol : Low_vol;
	}
	
	LaserSig[64] = 0;
	LaserSig[65] = 0;
}









