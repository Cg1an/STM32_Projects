#include "SPI.h"
#include "DMA.h"

float offsets[7][4];

/******************************************************************************
**函 数 名:MPU_WriteData
**函数功能:向MPU写入数据
**输    入:Addr--寄存器地址,Data--写入的数据
**返 回 值:无
*******************************************************************************/
u16 SPI_WriteData(uint16_t Addr)
{
	u16 Result;
	
	AS5048_CS_LOW();
	AS5048_CS_LOW();
	
	while (((SPI1->SR)&((uint16_t)0x0002)) == RESET); //等待，直到发送寄存器为空
	SPI1->DR = Addr;//写入地址
	while (((SPI1->SR)&((uint16_t)0x0001)) == RESET); //??,????????
	Result = SPI1->DR;

	AS5048_CS_HIGH();
	AS5048_CS_HIGH();

	return Result;
}
//-------------end---------------


void SPI_Init(void)
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
	MY_NVIC_Init(1,1,DMA2_Stream0_IRQn,2);
	MY_NVIC_Init(1,2,DMA2_Stream3_IRQn,2);
	SPI1->CR1&=((uint16_t)0x3040);      //清理SPI1的特定控制位
	SPI1->CR1|=((uint16_t)0x0b35);      //设置SPI1
	SPI1->I2SCFGR&=((uint16_t)0xf7ff);  //选择SPI模式
	SPI1->CRCPR|=((uint16_t)0x0007);    //校验码
	SPI1->CR1|=((uint16_t)0x0040);      //使能SPI1
	AS5048_CS_HIGH();//拉高片选
	delay_ms(100);

	SPI1->CR1&=(uint16_t)(~((uint16_t)0x0040));      //禁止SPI1
	SPI1->CR1=((uint16_t)0x0b15);       //重新设置SPI1（改写速率）
	SPI1->CR2|=((uint16_t)0x0003);      //使能SPI1的DMA请求
	SPI1->CR1|=((uint16_t)0x0040);      //使能SPI1
}
//-------------end---------------


//获得当前电角度下的编码器值与电角度间的零偏（以编码器LSB为单位）
//当前电角度值ElectricalDegCMD四个可能值：0，1，2，3*(PI_d2)；
//ECValue:当前编码器值
//value_360:每360电角度所对应的编码器值
uint32_t GetSingleOffSet()
{
	int32_t offset;
	offset = FOC.ECValue - FOC.ECValue_360 * (FOC.ElectricalDegCMD / PI_m2);
	if(offset < 0) offset += FOC.ECValue_360;
	offset %= FOC.ECValue_360;
	return offset;
}
//-------------end---------------



//获得编码器值与电角度间的零偏（以编码器LSB为单位），如果返回值为1，说明电角度与编码器值的增加方向相反
//该函数应该被周期性调用，比如每0.5s调用一次
void IdentifyOffSet()//ECValue:当前编码器值
{
	float * pValue;
	u8 Index_ElectricalDeg = (FOC.ElectricalDegCMD+0.01f) / PI_d2;  //考虑到float型的精度，避免相除计算错误

	offsets[FOC.CurrentIndex_Pole][Index_ElectricalDeg] = GetSingleOffSet();
	FOC.ElectricalDegCMD += PI_d2;
	if(FOC.ElectricalDegCMD >= (PI_m2-0.01f))
	{
		FOC.ElectricalDegCMD = 0;
		FOC.CurrentIndex_Pole ++;
		if(FOC.CurrentIndex_Pole >= FOC.Num_Pole)//一轮数据采集结束，计算统计结果
		{
			FOC.CurrentIndex_Pole = 0;
			pValue = (float *)offsets;
			FOC.Sum1=0; FOC.Sum2=0;
			for(int i=0; i<FOC.Num_Pole; i++)
			{
				FOC.Sum1 += pValue[0+i*4];
				FOC.Sum1 += pValue[2+i*4];
				FOC.Sum2 += pValue[1+i*4];
				FOC.Sum2 += pValue[3+i*4];
			}
			
			FOC.Sum1/=2*FOC.Num_Pole;
			FOC.Sum2/=2*FOC.Num_Pole;
			
			if(fabsf(FOC.Sum1 - FOC.Sum2) > FOC.ECValue_360 / 4)//如果电角度与编码器值的增加方向相反，则Sum1和Sum2相差value_360 / 2，即180°。
			{
				FOC.IdentifyResult = -1;
				return;
			}
			
			for(int i=0; i<FOC.Num_Pole; i++)
			{
				if( (fabsf(FOC.Sum1 - pValue[0+i*4]) > FOC.Tolerance)
					||(fabsf(FOC.Sum1 - pValue[2+i*4]) > FOC.Tolerance)
					||(fabsf(FOC.Sum2 - pValue[1+i*4]) > FOC.Tolerance)
					||(fabsf(FOC.Sum2 - pValue[3+i*4]) > FOC.Tolerance))
				{
					FOC.IdentifyResult = -2;//存在采样值错误
					return;
				}
			}

			FOC.IdentifyResult = 1;//辨识成功
			FOC.Offset = 0.5f * (FOC.Sum1 + FOC.Sum2);
		}
	}
}
//-------------end---------------

