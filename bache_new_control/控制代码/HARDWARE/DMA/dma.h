#ifndef __DMA_H
#define	__DMA_H	   
#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//DMA 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/7
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 
#define P2M 0x0 //外设到存储器模式
#define M2P 0x1 //存储器到外设模式
#define M2M 0x2 //存储器到存储器模式

extern u8 Uart2DMAGoing;
extern u8 Loaded;
extern u8 DMAfifo[512];

void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u8 chx, u8 dir, u32 par);//配置DMAx_CHx
void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx,u32 mar,u16 ndtr);	//使能一次DMA传输		   
#endif






























