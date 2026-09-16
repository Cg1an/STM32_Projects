#include "CH453.h"


void CH453_Write( unsigned short cmd )// 写命令
{
    uint8_t data;
    data=( unsigned char ) cmd;
    HAL_I2C_Master_Transmit(&hi2c2,( ( unsigned char )( cmd>>7 ) & CH423_I2C_MASK ) | CH423_I2C_ADDR1,&data,1,1000);
	
//  	CH423_I2c_Start();    // 启动总线
//    CH423_I2c_WrByte( ( ( unsigned char )( cmd>>7 ) & CH423_I2C_MASK ) | CH423_I2C_ADDR1 );
//    CH423_I2c_WrByte( ( unsigned char ) cmd );    // 发送数据
//    CH423_I2c_Stop();    // 结束总线 
}

void CH453_WriteByte( unsigned short cmd )    // 写出数据
{
//    CH423_I2c_Start();    // 启动总线
//    CH423_I2c_WrByte( ( unsigned char )( cmd>>8 ) );
//    CH423_I2c_WrByte( ( unsigned char ) cmd );    // 发送数据
//    CH423_I2c_Stop();    // 结束总线  	
	  uint8_t data;
    data=( unsigned char ) cmd;
    HAL_I2C_Master_Transmit(&hi2c2, (unsigned char )(cmd>>8),&data,1,1000);
}




