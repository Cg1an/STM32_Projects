#include "can.h"
#include "pid.h"

#define MOTOR_ID	0x201//  电机ID
CanRxMsg rx_message;	  //接收
CanTxMsg tx_message;    //发送

float s;
//CAN初始化
//tsjw:重新同步跳跃时间单元. 范围: ; CAN_SJW_1tq~ CAN_SJW_4tq
//tbs2:时间段2的时间单元.  范围:CAN_BS2_1tq~CAN_BS2_8tq;
//tbs1:时间段1的时间单元.     范围:   CAN_BS1_1tq ~CAN_BS1_16tq
//brp :波特率分频器.范围:1~1024;(实际要加1,也就是1~1024) tq=(brp)*tpclk1
//波特率=Fpclk1/((tsjw+tbs1+tbs2+3)*brp);
//mode: @ref CAN_operating_mode 范围：CAN_Mode_Normal,普通模式;CAN_Mode_LoopBack,回环模式;
//Fpclk1的时钟在初始化的时候设置为36M,如果设置CAN_Normal_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_LoopBack);
//则波特率为:42M/((1+6+7)*6)=500Kbps
//返回值:0,初始化OK;
//    其他,初始化失败;

struct motor       //pid结构体部分
	
{ 
int16_t	 angle_value ;//机械角度
int16_t	 speed_rpm;  //转速
int16_t	real_current;//转矩电流
int16_t	temperature ;//电机温度
int16_t	 real_angle ;	
} motor[5];

void CAN1_Mode_Init()   //CAN1初始化
{
   CAN_InitTypeDef        CAN_InitStructure;
	 CAN_FilterInitTypeDef  CAN_FilterInitStructure;
   GPIO_InitTypeDef  GPIO_InitStructure; 
	 NVIC_InitTypeDef  NVIC_InitStructure;

    //使能相关时钟
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能PORTA时钟	                   											 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//使能CAN1时钟	
	
    //初始化GPIO
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11| GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化PA11,PA12
	
	  //引脚复用映射配置
	  GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_CAN1); //GPIOA11复用为CAN1
	  GPIO_PinAFConfig(GPIOA,GPIO_PinSource12,GPIO_AF_CAN1); //GPIOA12复用为CAN1
	  
		CAN_DeInit(CAN1);
    CAN_StructInit(&CAN_InitStructure);


  	//CAN单元设置
   	CAN_InitStructure.CAN_TTCM=DISABLE;	//非时间触发通信模式   
  	CAN_InitStructure.CAN_ABOM=DISABLE;	//软件自动离线管理	  
  	CAN_InitStructure.CAN_AWUM=DISABLE;//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
  	CAN_InitStructure.CAN_NART=ENABLE;	//禁止报文自动传送 
  	CAN_InitStructure.CAN_RFLM=DISABLE;	//报文不锁定,新的覆盖旧的  
  	CAN_InitStructure.CAN_TXFP=DISABLE;	//优先级由报文标识符决定 
  	CAN_InitStructure.CAN_Mode= CAN_Mode_Normal;	 //模式设置为回环模式   (CAN_Mode_Normal;)//CAN工作模式;0,普通模式;1,环回模式
  	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;	//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位 CAN_SJW_1tq~CAN_SJW_4tq  (CAN_SJW_1tq)
  	CAN_InitStructure.CAN_BS1=CAN_BS1_9tq; //Tbs1范围CAN_BS1_1tq ~CAN_BS1_16tq   (CAN_BS1_9tq)
  	CAN_InitStructure.CAN_BS2=CAN_BS2_4tq;//Tbs2范围CAN_BS2_1tq ~	CAN_BS2_8tq   (CAN_BS2_4tq)
  	CAN_InitStructure.CAN_Prescaler=3;  //分频系数(Fdiv)为brp+1	    (3)
  	CAN_Init(CAN1, &CAN_InitStructure);   // 初始化CAN1 ***///42/(1+9+4)/3=1Mbps
    
		//配置过滤器
 	  CAN_FilterInitStructure.CAN_FilterNumber=0;	  //过滤器0
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32位 
  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32位ID
  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32位MASK
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//过滤器0关联到FIFO0
  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //激活过滤器0
  	CAN_FilterInit(&CAN_FilterInitStructure);//滤波器初始化
		
	  CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//FIFO0消息挂号中断允许.		    
    CAN_ITConfig(CAN1,CAN_IT_TME,ENABLE);
	
  	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;     // 主优先级为1
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;            // 次优先级为0
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
 
    NVIC_InitStructure.NVIC_IRQChannel = CAN1_TX_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // 主优先级为1
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;            // 次优先级为0
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);

}   
 

void CAN1_TX_IRQHandler(void)
{
    if (CAN_GetITStatus(CAN1,CAN_IT_TME)!= RESET) 
	{
	   CAN_ClearITPendingBit(CAN1,CAN_IT_TME);
      
  }
}

//中断服务函数	
void CAN1_RX0_IRQHandler(void)   
{
  s=read_spend();

}


		    
//void CAN1_RX0_IRQHandler(void)   
//{
//  	if (CAN_GetITStatus(CAN1,CAN_IT_FMP0)!= RESET)
//	{
//  
//		CAN_Receive(CAN1, CAN_FIFO0, &rx_message);
//		CAN_ClearITPendingBit(CAN1, CAN_IT_FF0);
//		CAN_ClearFlag(CAN1, CAN_FLAG_FF0); 
//		s=read_spend(1);
//		Set_motor_urrent( 1000,0,0,0);
//  }
//	//Set_motor_urrent( 1000,0,0,0);

//}

void Motor_ReadData(void)     //接收数据  num为电机ID
{
	
  if (CAN_GetITStatus(CAN1,CAN_IT_FMP0)!= RESET)
	{
		CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
		
		CAN_Receive(CAN1, CAN_FIFO0, &rx_message);
		
		if(rx_message.StdId == MOTOR_ID )  //区别不同电机反馈数据
		{
						
			motor[1]. angle_value   = rx_message.Data[0] << 8 | rx_message.Data[1];  //0~8191
			motor[1].speed_rpm   = rx_message.Data[2] << 8 | rx_message.Data[3];     //速度
			motor[1].real_current   = rx_message.Data[4] << 8 | rx_message.Data[5];     //电流
			motor[1].temperature   = rx_message.Data[4] << 8 | rx_message.Data[5];     //温度
		
			motor[1]. real_angle  = motor[1]. angle_value/8192.0f*360.0f;    //角度
		}
	}
}


//发送数据  设置电调的电流输出  -16384到16384  对应-20A到20A
void Set_motor_urrent(signed short int i1, signed short int i2, signed short int i3, signed short int i4)
{
    tx_message.StdId = 0x200;  //ID  
    tx_message.IDE = CAN_Id_Standard;
    tx_message.RTR = CAN_RTR_Data;
    tx_message.DLC = 0x08;   //0x08可控制8个电机，0x02可控制1个电机
    
    tx_message.Data[0] = i1 >> 8;
    tx_message.Data[1] = i1;
    tx_message.Data[2] = i2 >> 8;
    tx_message.Data[3] = i2;
    tx_message.Data[4] = i3 >> 8;
    tx_message.Data[5] = i3;
    tx_message.Data[6] = i4 >> 8;
    tx_message.Data[7] = i4;
	
    CAN_Transmit(CAN1,&tx_message);
}

//读取速度
float  read_spend(void)   //num用来区别不同的电机ID
	  
{
		if (CAN_GetITStatus(CAN1,CAN_IT_FMP0)!= RESET)
	{
		CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
		
		CAN_Receive(CAN1, CAN_FIFO0, &rx_message);
		
  if(rx_message.StdId ==MOTOR_ID)
		{
						
			motor[1].speed_rpm = rx_message.Data[2] << 8 | rx_message.Data[3];
		
		}
	}
return 	motor[1].speed_rpm;
}

//角度
signed short int  read_angle(void)   //num用来区别不同的电机ID
{

 if(rx_message.StdId ==MOTOR_ID )  //区别不同电机反馈数据
{
						
			motor[1]. angle_value  = rx_message.Data[0] << 8 | rx_message.Data[1];  //0~8191
			
			motor[1]. real_angle  = motor[1]. angle_value/8192.0f*360.0f;
		
}
		return 	motor[1]. real_angle  ;
}

//signed short int total_angle()
//{
//  float res1 ,res2;







//}
