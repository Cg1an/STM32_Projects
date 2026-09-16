#include "CH423S.h"

void CH423_Write( unsigned short cmd )// 写命令
{
    uint8_t data;
    data=( unsigned char ) cmd;
    HAL_I2C_Master_Transmit(&hi2c1,( ( unsigned char )( cmd>>7 ) & CH423_I2C_MASK ) | CH423_I2C_ADDR1,&data,1,1000);
	
//    CH423_I2c_Start();    // 启动总线
//    CH423_I2c_WrByte( ( ( unsigned char )( cmd>>7 ) & CH423_I2C_MASK ) | CH423_I2C_ADDR1 );
//    CH423_I2c_WrByte( ( unsigned char ) cmd );    // 发送数据
//    CH423_I2c_Stop();    // 结束总线 
}

void CH423_WriteByte(I2C_HandleTypeDef *hi2c, unsigned short cmd )    // 写出数据
{
//    CH423_I2c_Start();    // 启动总线
//    CH423_I2c_WrByte( ( unsigned char )( cmd>>8 ) );
//    CH423_I2c_WrByte( ( unsigned char ) cmd );    // 发送数据
//    CH423_I2c_Stop();    // 结束总线  	
	  uint8_t data;
    data=( unsigned char ) cmd;
    HAL_I2C_Master_Transmit(hi2c, (unsigned char )(cmd>>8),&data,1,1000);
}

unsigned char CH423_ReadByte(I2C_HandleTypeDef *hi2c) // 读取数据
{     
	    uint8_t data;
	    HAL_I2C_Master_Receive(hi2c, CH423_RD_IO_CMD, &data, 1,1000);
//	  HAL_I2C_Master_Recive(&hi2c1,CH423_RD_IO_CMD,&data,1,1000);
//    unsigned char din;
//    CH423_I2c_Start();    // 启动总线
//    CH423_I2c_WrByte( CH423_RD_IO_CMD );    // 此值为0x4D
//    din=CH423_I2c_RdByte();    // 读取数据
//    CH423_I2c_Stop();    // 结束总线
     return data;
}


