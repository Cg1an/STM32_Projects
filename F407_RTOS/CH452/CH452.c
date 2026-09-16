#include"CH452.H"


void CH452_Write(I2C_HandleTypeDef *hi2c,unsigned short cmd)	//写命令
{
	  uint8_t data=(unsigned char)cmd;
	  uint8_t addr=(unsigned char)(cmd>>8);
   //HAL_I2C_Master_Transmit(hi2c, ((unsigned char)(cmd>>7)&CH452_I2C_MASK)|CH452_I2C_ADDR0,&data,1,1000);
	HAL_I2C_Master_Transmit(hi2c, (0x20|addr)<<1,&data,1,1000);
}

unsigned char CH452_Read(I2C_HandleTypeDef *hi2c)		//读取按键
{
//	unsigned char keycode;
//   	CH452_I2c_Start();                /*启动总线*/
//   	CH452_I2c_WrByte((unsigned char)(CH452_GET_KEY>>7)&CH452_I2C_MASK|0x01|CH452_I2C_ADDR1);    // 若有两个CH452并连,当ADDR=0时,需修改为CH452_I2C_ADDR0
//   	keycode=CH452_I2c_RdByte();      /*读取数据*/
//	CH452_I2c_Stop(); 
//	/*结束总线*/ 
	
	    uint8_t data=0;
//	    HAL_I2C_Master_Receive(hi2c, (unsigned char)((CH452_GET_KEY>>7)&CH452_I2C_MASK)|CH452_I2C_ADDR0, &data, 1,1000);
	    HAL_I2C_Master_Receive(hi2c,(0x20|0x07)<<1, &data, 1,1000);
	    return data;
}


