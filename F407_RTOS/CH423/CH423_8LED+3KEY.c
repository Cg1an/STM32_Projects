#include "CH423S.h"
//接在iic2上
//安全防护设备引脚使用
//OC0-OC7  8个LED
//IO0-IO2  3个按键 

void CH423_WRITE8LED(uint8_t LED)
{
		  CH423_WriteByte(&hi2c2,0x4400+LED);

}
uint8_t CH423_READ3KEY(void)
{
			CH423_WriteByte(&hi2c2,0x4800);
		 	return  CH423_ReadByte(&hi2c2);
	 
}




