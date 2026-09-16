#include "can.h"
#include "canuser.h"
#include "delay.h"
#include "usart.h"
//#include "UART.h"
//#include "MPU.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//CAN驱动 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/7
//版本：V1.0 
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 

	    
//CAN初始化
//tsjw:重新同步跳跃时间单元.范围:1~3;
//tbs2:时间段2的时间单元.范围:1~8;
//tbs1:时间段1的时间单元.范围:1~16;
//brp :波特率分频器.范围:1~1024;(实际要加1,也就是1~1024) tq=(brp)*tpclk1
//注意以上参数任何一个都不能设为0,否则会乱.
//波特率=Fpclk1/((tbs1+tbs2+1)*brp);
//mode:0,普通模式;1,回环模式;
//Fpclk1的时钟在初始化的时候设置为42M,如果设置CAN1_Mode_Init(1,6,7,6,1);
//则波特率为:42M/((6+7+1)*6)=500Kbps
//返回值:0,初始化OK;
//    其他,初始化失败;

CAN_DATA CAN_DATA1;
CAN_DATA CAN_AngleFL, CAN_AngleFR, CAN_AngleBL, CAN_AngleBR;
CAN_DATA CAN_VelFL, CAN_VelFR, CAN_VelBL, CAN_VelBR;
CAN_DATA CAN_Motor1, CAN_Motor2;

uint8_t EnMode[8]  = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfc};
uint8_t DisMode[8] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfd};

u8 CAN_Data[8] = {0, 0, 0, 0, 0, 0, 0, 0};
//u32 ID;

int rawAngle[4];

u8 DriverInfoReceived = 0;
u16 Motor_Enabled = 0;

u8 CAN1_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{
 	if(tsjw==0||tbs2==0||tbs1==0||brp==0)return 1;
	tsjw-=1;//先减去1.再用于设置
	tbs2-=1;
	tbs1-=1;
	brp-=1;

	RCC->AHB1ENR|=1<<0;  	//使能PORTA口时钟 
	GPIO_Set(GPIOA,PIN11|PIN12,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);//PA11,PA12,复用功能,上拉输出
 	GPIO_AF_Set(GPIOA,11,9);//AF9
	GPIO_AF_Set(GPIOA,12,9);//AF9
 
	RCC->APB1ENR|=1<<25;//使能CAN1时钟 CAN1使用的是APB1的时钟(max:42M)
	
	CAN1->MCR=0x0000;	//退出睡眠模式(同时设置所有位为0)
	CAN1->MCR|=1<<0;		//请求CAN进入初始化模式
//	while((CAN1->MSR&1<<0)==0)
//	{
//		i++;
//		if(i>100)return 2;//进入初始化模式失败
//	}

	CAN1->MCR|=0<<7;		//非时间触发通信模式
	CAN1->MCR|=0<<6;		//软件自动离线管理
	CAN1->MCR|=0<<5;		//睡眠模式通过软件唤醒(清除CAN1->MCR的SLEEP位)
	CAN1->MCR|=1<<4;		//禁止报文自动传送
	CAN1->MCR|=0<<3;		//报文不锁定,新的覆盖旧的
	CAN1->MCR|=0<<2;		//优先级由报文标识符决定
	CAN1->BTR=0x00000000;	//清除原来的设置.
	CAN1->BTR|=mode<<30;	//模式设置 0,普通模式;1,回环模式;
	CAN1->BTR|=tsjw<<24; 	//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位
	CAN1->BTR|=tbs2<<20; 	//Tbs2=tbs2+1个时间单位
	CAN1->BTR|=tbs1<<16;	//Tbs1=tbs1+1个时间单位
	CAN1->BTR|=brp<<0;  	//分频系数(Fdiv)为brp+1，波特率:Fpclk1/((Tbs1+Tbs2+1)*Fdiv)

	
	//过滤器初始化
	//CAN1->FMR =0x2A1C0E01;		//过滤器组工作在初始化模式
	CAN1->FA1R|=1<<0;		//激活过滤器0
	CAN1->FA1R|=1<<15;		//激活过滤器15
	
	CAN1->FS1R|=1<<0; 		//过滤器位宽为32位.
	CAN1->FM1R|=0<<0;		  //过滤器0工作在标识符屏蔽位模式
	CAN1->FFA1R|=0<<0;		//过滤器0关联到FIFO0
	CAN1->sFilterRegister[0].FR1=(0X00000208)<<5;//32位ID
	CAN1->sFilterRegister[0].FR2=0X00000000;//32位MASK

	CAN1->FS1R|=1<<15; 		//过滤器位宽为32位. 0: Dual 16-bit scale configuration; 1: Single 32-bit scale configuration
	CAN1->FM1R|=0<<15;		//过滤器0工作在标识符屏蔽位模式. 0: Two 32-bit registers of filter bank x are in Identifier Mask mode; 1: Two 32-bit registers of filter bank x are in Identifier List mode.
	CAN1->FFA1R|=0<<15;		//过滤器15关联到FIFO0
	CAN1->sFilterRegister[15].FR1=0X00000480;//32位ID
	CAN1->sFilterRegister[15].FR2=0X00000000;//32位MASK
	
	CAN1->FMR&=0x2A1C0E00;		//过滤器组进入正常模式

	CAN1->IER|=1<<1;		////使用中断接收, FIFO0消息挂号中断允许.	  
	CAN1->IER|=1<<4;		////使用中断接收, FIFO1消息挂号中断允许.	  
	MY_NVIC_Init(0,2,CAN1_RX0_IRQn,2);//组2
	MY_NVIC_Init(0,2,CAN1_RX1_IRQn,2);//组2

	CAN1->MCR&=~(1<<0);		//请求CAN退出初始化模式
//	while((CAN1->MSR&1<<0)==1)
//	{
//		i++;
//		if(i>0XFFF0)return 3;//退出初始化模式失败
//	}

	return 0;
}
//-----------------end----------------------


u8 CAN2_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{
	u16 i=0;
 	if(tsjw==0||tbs2==0||tbs1==0||brp==0)return 1;
	tsjw-=1;//先减去1.再用于设置
	tbs2-=1;
	tbs1-=1;
	brp-=1;

	RCC->AHB1ENR|=1<<1;  	//使能PORTB口时钟 
	GPIO_Set(GPIOB,PIN12|PIN13,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);//复用功能,上拉输出
 	GPIO_AF_Set(GPIOB,12,9);//AF9
	GPIO_AF_Set(GPIOB,13,9);//AF9
 
	RCC->APB1ENR|=1<<26;//使能CAN2时钟 CAN2使用的是APB1的时钟(max:42M)
	CAN2->MCR=0x0000;	//退出睡眠模式(同时设置所有位为0)
	CAN2->MCR|=1<<0;		//请求CAN进入初始化模式
	while((CAN2->MSR&1<<0)==0)
	{
		i++;
		if(i>100)return 2;//进入初始化模式失败
	}
	
	CAN2->MCR|=0<<7;		//非时间触发通信模式
	CAN2->MCR|=0<<6;		//软件自动离线管理
	CAN2->MCR|=0<<5;		//睡眠模式通过软件唤醒(清除CAN1->MCR的SLEEP位)
	CAN2->MCR|=1<<4;		//禁止报文自动传送
	CAN2->MCR|=0<<3;		//报文不锁定,新的覆盖旧的
	CAN2->MCR|=0<<2;		//优先级由报文标识符决定
	CAN2->BTR=0x00000000;	//清除原来的设置.
	CAN2->BTR|=mode<<30;	//模式设置 0,普通模式;1,回环模式;
	CAN2->BTR|=tsjw<<24; 	//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位
	CAN2->BTR|=tbs2<<20; 	//Tbs2=tbs2+1个时间单位
	CAN2->BTR|=tbs1<<16;	//Tbs1=tbs1+1个时间单位
	CAN2->BTR|=brp<<0;  	//分频系数(Fdiv)为brp+1
							//波特率:Fpclk1/((Tbs1+Tbs2+1)*Fdiv)
	CAN2->MCR&=~(1<<0);		//请求CAN退出初始化模式
	while((CAN2->MSR&1<<0)==1)
	{
		i++;
		if(i>0XFFF0)return 3;//退出初始化模式失败
	}
	CAN2->IER|=1<<1;		////使用中断接收, FIFO0消息挂号中断允许.	   
	CAN2->IER|=1<<4;		////使用中断接收, FIFO1消息挂号中断允许.	 	
	MY_NVIC_Init(0,1,CAN2_RX0_IRQn,2);//组2
	MY_NVIC_Init(0,1,CAN2_RX1_IRQn,2);//组2
	return 0;
}
//-----------------end----------------------

void InitControl()
{
	//使能模式
	CAN_SetHead(&CAN_Motor1, 0x001, 0, 0, 8);
	CAN_SetHead(&CAN_Motor2, 0x001, 0, 0, 8);
	CAN_SetDATA(&CAN_Motor1, EnMode); CAN_Trans(CAN1, &CAN_Motor1);
	CAN_SetDATA(&CAN_Motor2, EnMode); CAN_Trans(CAN2, &CAN_Motor2);
}

void CAN_SetHead(CAN_DATA * data, u32 id,u8 ide,u8 rtr,u8 len)
{
	if(ide==0)	//标准帧
	{
		id&=0x7ff;//取低11位stdid
		id<<=21;		  
	}else		//扩展帧
	{
		id&=0X1FFFFFFF;//取低29位extid
		id<<=3;
	}
	data->IR = 0;
	data->IR|=id;		 
	data->IR|=ide<<2;	  
	data->IR|=rtr<<1;
	len&=0X0F;//得到低四位
	data->DTR&=~(0X0000000F);
	data->DTR|=len;//设置DLC.
}
//-----------------end----------------------



void CAN_SetDATA(CAN_DATA * CAN, u8 * data)
{
	u8 i;
	for(i=0; i<8; i++)
	{
		CAN->buf.buf[i] = data[i];
	}
}



u8 CAN_Trans(CAN_TypeDef * CAN, CAN_DATA * data)
{
//	u8 status=0;
//	while((CAN->TSR&(1<<26))==0);
	u32 timeout = 0; // 【修改 1】：增加超时变量 (改用 u32 防止太快溢出)
    
    // 【修改 2】：增加超时判断
    while((CAN->TSR&(1<<26))==0)
    {
        timeout++;
        // 50万次大约需要几毫秒，如果还没发出去，就直接退出
        if(timeout > 500000) 
        {
            return 0xFF; // 发送失败，返回错误码，不再死等！
        }
    }
	if(CAN->TSR&(1<<26)) //邮箱0为空
		data->mbox=0;
//	else if(CAN->TSR&(1<<27)) data->mbox=1;	//邮箱1为空
//	else if(CAN->TSR&(1<<28)) data->mbox=2;	//邮箱2为空
//	else 
//	{
//		data->mbox = 0XFF;					//无空邮箱,无法发送
//		return 0xFF;
//	}
//	while(status != 7)
//	{
//		
//	}
	CAN->sTxMailBox[data->mbox].TIR   = data->IR;
	CAN->sTxMailBox[data->mbox].TDTR &= 0X0000FEF0;
	CAN->sTxMailBox[data->mbox].TDTR |= data->DTR;
	CAN->sTxMailBox[data->mbox].TDHR  = data->buf.DR[1];
	CAN->sTxMailBox[data->mbox].TDLR  = data->buf.DR[0];
	
	CAN->sTxMailBox[data->mbox].TIR|=1<<0; //请求发送邮箱数据
	return data->mbox;
}


//获得发送状态.
//mbox:邮箱编号;
//返回值:发送状态. 0,挂起;0X05,发送失败;0X07,发送成功.
u8 CAN_Tx_Staus(CAN_TypeDef * CAN, u8 mbox)
{
	u8 sta=0;					    
	switch (mbox)
	{
		case 0: 
			sta |= CAN->TSR&(1<<0);			//RQCP0
			sta |= CAN->TSR&(1<<1);			//TXOK0
			sta |=((CAN->TSR&(1<<26))>>24);	//TME0
			break;
		case 1: 
			sta |= CAN->TSR&(1<<8)>>8;		//RQCP1
			sta |= CAN->TSR&(1<<9)>>8;		//TXOK1
			sta |=((CAN->TSR&(1<<27))>>25);	//TME1	   
			break;
		case 2: 
			sta |= CAN->TSR&(1<<16)>>16;	//RQCP2
			sta |= CAN->TSR&(1<<17)>>16;	//TXOK2
			sta |=((CAN->TSR&(1<<28))>>26);	//TME2
			break;
		default:
			sta=0X05;//邮箱号不对,肯定失败.
		break;
	}
	return sta;
}


void CAN_Res(u8 fifox, CAN_TypeDef * CAN, CAN_DATA * data)
{
	data->IR  = CAN->sFIFOMailBox[fifox].RIR;
	data->DTR = CAN->sFIFOMailBox[fifox].RDTR;
	data->buf.DR[0] = CAN->sFIFOMailBox[fifox].RDLR;
	data->buf.DR[1] = CAN->sFIFOMailBox[fifox].RDHR;
	
	if(fifox==0)CAN->RF0R|=0X20;//释放FIFO0邮箱
	else if(fifox==1)CAN->RF1R|=0X20;//释放FIFO1邮箱
}

//中断服务函数
void CAN1_RX0_IRQHandler(void)
{
	u16 type, index;//消息类型，电机编号

	CAN_Res(0, CAN1, &CAN_DATA1);
//	ID = (CAN_DATA1.IR>>21)&0x7ff;
//	
//	type = ID&0x7f0;
//	index = ID&0x0f;
	switch(type)
	{
		case 0x0380:
			if((index>4)&&(index<9))
			{
				DriverInfoReceived |= 0x01<<(index-5);
				rawAngle[index-5] = CAN_DATA1.buf.buf[4] | (CAN_DATA1.buf.buf[3]<<8) | (CAN_DATA1.buf.buf[2]<<16) | (CAN_DATA1.buf.buf[1]<<24);
			}
			break;
			
		case 0x0280:
//			PLC_Trans1.Message_Struct.Driver_Info[index-1].Velocity = CAN_DATA1.buf.buf[2] | (CAN_DATA1.buf.buf[1]<<8);
//			PLC_Trans1.Message_Struct.Driver_Info[index-1].Alarm &= 0x00FF;//清除高8位
//			PLC_Trans1.Message_Struct.Driver_Info[index-1].Alarm |= CAN_DATA1.buf.buf[7]<<8;//设置高8位
			break;
		
		case 0x0300:
//			PLC_Trans1.Message_Struct.Driver_Info[index-1].Current = CAN_DATA1.buf.buf[2] | (CAN_DATA1.buf.buf[1]<<8);
//			PLC_Trans1.Message_Struct.Driver_Info[index-1].Alarm &= 0xFF00;//清除低8位
//			PLC_Trans1.Message_Struct.Driver_Info[index-1].Alarm |= CAN_DATA1.buf.buf[7];//设置低8位
			break;
		
		case 0x0080:
			if(CAN_DATA1.buf.buf[2] == 0x0a)
				Motor_Enabled |= 0x01<< index;
	}
	
	
	
//	if(ID == 0x0385)
//	{
//		rawAngle[0] = CAN_DATA1.buf.buf[4] | (CAN_DATA1.buf.buf[3]<<8) | (CAN_DATA1.buf.buf[2]<<16) | (CAN_DATA1.buf.buf[1]<<24);
//		return;
//	}
//	if(ID == 0x0386)
//	{
//		rawAngle[1] = CAN_DATA1.buf.buf[4] | (CAN_DATA1.buf.buf[3]<<8) | (CAN_DATA1.buf.buf[2]<<16) | (CAN_DATA1.buf.buf[1]<<24);
//		return;
//	}
//	if(ID == 0x0387)
//	{
//		rawAngle[2] = CAN_DATA1.buf.buf[4] | (CAN_DATA1.buf.buf[3]<<8) | (CAN_DATA1.buf.buf[2]<<16) | (CAN_DATA1.buf.buf[1]<<24);
//		return;
//	}
//	if(ID == 0x0388)
//	{
//		rawAngle[3] = CAN_DATA1.buf.buf[4] | (CAN_DATA1.buf.buf[3]<<8) | (CAN_DATA1.buf.buf[2]<<16) | (CAN_DATA1.buf.buf[1]<<24);
//		return;
//	}
		
}

//中断服务函数
void CAN1_RX1_IRQHandler(void)
{
//		CAN_Res(1, CAN1, &CAN_DATA1);
//		ID = (CAN_DATA1.IR>>21)&0x7ff;
//		if(ID == 0x0208)
//		{
//			rawAngle[0] = CAN_DATA1.buf.buf[4] | (CAN_DATA1.buf.buf[3]<<8) | (CAN_DATA1.buf.buf[2]<<16) | (CAN_DATA1.buf.buf[1]<<24);
//		}
	//CAN_Trans(CAN2, &CAN_DATA1);
	//CAN1_DataHandler();
}

void CAN2_RX0_IRQHandler(void)
{
	u32 ID;	
	CAN_Res(0, CAN2, &CAN_DATA1);
}

/**
 * @brief  [兼容旧版本] 发送标准帧数据 (Standard ID Data Frame)
 * @param  CANx:   CAN外设指针 (填 CAN1 或 CAN2)
 * @param  std_id: 目标ID (例如 0x600)
 * @param  pData:  要发送的数据数组指针 (uint8_t data[8])
 * @retval 返回使用的邮箱号，0xFF表示发送失败
 */
u8 CANx_SendStdData(CAN_TypeDef * CANx, uint32_t std_id, uint8_t *pData)
{
    CAN_DATA Tx_Msg; // 定义临时报文结构体
    
    // 1. 配置报文头：标准帧ide=0, 数据帧rtr=0, 数据长度len=8
    CAN_SetHead(&Tx_Msg, std_id, 0, 0, 8);
    
    // 2. 拷贝数据
    CAN_SetDATA(&Tx_Msg, pData);
    
    // 3. 调用底层发送并返回状态
    return CAN_Trans(CANx, &Tx_Msg);
}