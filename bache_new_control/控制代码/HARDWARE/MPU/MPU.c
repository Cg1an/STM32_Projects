#include "MPU.h"
#include "delay.h"
#include "dma.h"	
#include "led.h" 

/******************************************************************************
**函 数 名:MPU_WriteData
**函数功能:向MPU写入数据
**输    入:Addr--寄存器地址,Data--写入的数据
**返 回 值:无
*******************************************************************************/

u8 MPU_t[16] = {0xBB, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
u8 MPU_r[16];
u8 ID;

Sensor_Data		MPU_Gyro;
Sensor_Data		MPU_Gyro_p;
Sensor_Data		MPU_Accel = {0, 0, 0};
Sensor_Data		MPU_Accel_p;

static void MPU_WriteData(uint8_t Addr, uint8_t Data)
{
	u8 Byte;
	
	MPU_CS_LOW();
	MPU_CS_LOW();
	
	while (((SPI1->SR)&((uint16_t)0x0002)) == RESET); //等待，直到发送寄存器为空
	SPI1->DR = MPU_WRITE + Addr;//写入地址
	while (((SPI1->SR)&((uint16_t)0x0001)) == RESET); //??,????????
	Byte = SPI1->DR;
	while (((SPI1->SR)&((uint16_t)0x0002)) == RESET); //等待，直到发送寄存器为空
	SPI1->DR = Data;//写入数据
	while (((SPI1->SR)&((uint16_t)0x0001)) == RESET); //??,????????
	Byte = SPI1->DR;	
	Byte = Byte;
	MPU_CS_HIGH();
	MPU_CS_HIGH();

}
//-----------------end---------------------

static u8 MPU_ReadData(uint8_t Addr, uint8_t Data)
{
	u8 Byte;
	
	MPU_CS_LOW();
	MPU_CS_LOW();
	
	while (((SPI1->SR)&((uint16_t)0x0002)) == RESET); //等待，直到发送寄存器为空
	SPI1->DR = MPU_READ + Addr;//写入地址
	while (((SPI1->SR)&((uint16_t)0x0001)) == RESET); //??,????????
	Byte = SPI1->DR;
	while (((SPI1->SR)&((uint16_t)0x0002)) == RESET); //等待，直到发送寄存器为空
	SPI1->DR = Data;//写入数据
	while (((SPI1->SR)&((uint16_t)0x0001)) == RESET); //??,????????
	Byte = SPI1->DR;
	
	MPU_CS_HIGH();
	MPU_CS_HIGH();
	
	return Byte;
}

void MPU_Init(void)
{
	RCC->AHB1ENR|=1<<1;   //使能PORTB时钟
	RCC->AHB1ENR|=1<<3;   //使能PORTD时钟
	RCC->APB2ENR|=1<<12; 	//SPI1时钟使能
	GPIO_Set(GPIOD,PIN2,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);//输出功能
	GPIO_Set(GPIOB,PIN3|PIN4|PIN5,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);//复用功能
	GPIO_AF_Set(GPIOB,3,5);	//PB3,AF5,SPI1
	GPIO_AF_Set(GPIOB,4,5);	//PB4,AF5,SPI1
	GPIO_AF_Set(GPIOB,5,5);	//PB5,AF5,SPI1
	MYDMA_Config(DMA2_Stream0,3,P2M,(u32)&SPI1->DR);
	MYDMA_Config(DMA2_Stream3,3,M2P,(u32)&SPI1->DR);
	MY_NVIC_Init(0,1,DMA2_Stream0_IRQn,2);
	MY_NVIC_Init(0,2,DMA2_Stream3_IRQn,2);
	SPI1->CR1&=((uint16_t)0x3040);      //清理SPI1的特定控制位
	SPI1->CR1|=((uint16_t)0x0337);      //设置SPI1
	SPI1->I2SCFGR&=((uint16_t)0xf7ff);  //选择SPI模式
	SPI1->CRCPR|=((uint16_t)0x0007);    //校验码
	SPI1->CR1|=((uint16_t)0x0040);      //使能SPI1
	MPU_CS_HIGH();//拉高片选
	delay_ms(100);
	MPU_CS_LOW();//选中MPU
	delay_ms(100);
	MPU_WriteData(PWR_MGMT_1, 0x80);		//解除休眠状态
	delay_ms(1000);
	MPU_WriteData(PWR_MGMT_1, 0x03);		//选择陀螺Z轴时钟源
	delay_ms(10);
	MPU_WriteData(USER_CTRL, 0x10);			//禁止I2C接口
	delay_ms(10);
	MPU_WriteData(SMPRT_DIV, 0x00);			//设置采样速率为8KHz
	delay_ms(10);
	MPU_WriteData(CONFIG, 0x00);			  //设置低通滤波频率256Hz, 陀螺仪输出频率8KHz
	delay_ms(10);
	MPU_WriteData(GYRO_CONFIG, (GFS_SEL_250 << 3));	//陀螺仪测量范围±250°/s
	delay_ms(10);
	MPU_WriteData(ACCEL_CONFIG, (AFS_SEL_4 << 3));	//加速度测量范围±4g
	delay_ms(10);
	MPU_WriteData(INT_PIN_CFG, 0x10);			//中断设置
	delay_ms(10);
	MPU_WriteData(INT_ENABLE, 0x01);			//使能数据ready中断
	delay_ms(10);
	ID = MPU_ReadData(WHO_AM_I, 0x01);			//使能数据ready中断
	delay_ms(10);
//	while(1)
//	{
//		ID = MPU_ReadData(WHO_AM_I, 0x01);			//使能数据ready中断
//	}
	
	SPI1->CR1&=(uint16_t)(~((uint16_t)0x0040));      //禁止SPI1
	SPI1->CR1=((uint16_t)0x0317);       //重新设置SPI1（改写速率）
	SPI1->CR2|=((uint16_t)0x0003);      //使能SPI1的DMA请求
	SPI1->CR1|=((uint16_t)0x0040);      //使能SPI1
}
//-----------------end---------------------

void MPU_StartRead(void)
{
	MPU_CS_LOW();		  //使能器件
	MPU_CS_LOW();		  //使能器件
	MYDMA_Enable(DMA2_Stream3, (u32)MPU_t, 15); //发送读取数据命令，然后发送14个dummy
	MYDMA_Enable(DMA2_Stream0, (u32)MPU_r, 15); //接受数据
}


//DMA发送完成中断服务程序
void DMA2_Stream3_IRQHandler(void)
{
	DMA2->LIFCR |=1<<27; //清除发送完成中断标志
}
//-----------------end---------------------


//DMA接收完成中断服务程序
void DMA2_Stream0_IRQHandler(void)
{
	u8 * SensorData;
	DMA2->LIFCR |=1<<5; //清除接收完成中断标志
	MPU_CS_HIGH();  	//取消片选
	MPU_CS_HIGH();  	//取消片选
	SensorData = MPU_r + 1;
	MPU_Accel.Sensor.X = (int16_t)((((uint16_t)SensorData[0]) << 8) | SensorData[1]);
	MPU_Accel.Sensor.Y = (int16_t)((((uint16_t)SensorData[2]) << 8) | SensorData[3]);
	MPU_Accel.Sensor.Z = (int16_t)((((uint16_t)SensorData[4]) << 8) | SensorData[5]);

	MPU_Gyro.Sensor.Y = (-1) * ((int16_t)((((uint16_t)SensorData[8]) << 8) | SensorData[9]));
	MPU_Gyro.Sensor.X = (int16_t)((((uint16_t)SensorData[10]) << 8) | SensorData[11]);
	MPU_Gyro.Sensor.Z = (int16_t)((((uint16_t)SensorData[12]) << 8) | SensorData[13]);
}
//-----------------end---------------------

