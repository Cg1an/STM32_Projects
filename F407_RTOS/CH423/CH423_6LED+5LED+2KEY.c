#include "CH423S.h"
//接在iic1上
//车载电源引脚使用
//OC0-OC5  六个LED
//0C8-OC12 五个LED
//IO3-IO4  两个按键
//

void CH423_WRITE6LED_5LED(uint16_t KEY)
{
		  CH423_WriteByte(&hi2c1,0x4400+(KEY&0X00FF));
			CH423_WriteByte(&hi2c1,0x4600+(KEY>>8));
}

uint8_t CH423_READ_2KEY()
{
	 uint8_t key;
	 key=CH423_ReadByte(&hi2c1);
	 return key;
}

