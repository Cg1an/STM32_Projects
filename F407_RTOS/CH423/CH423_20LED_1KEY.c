#include "CH423S.h"
#include "RS485_work.h"

//接在iic1上
//数话同传引脚使用
//OC0-OC7  8个LED
//0C8-OC15 8个LED
//IO0-IO3  4个LED  
//PE1      1个按键

//task_information CH423_20LED_1KEY_information;

void CH423_20led_1key_init(void)
{ 
     
     
}

void CH423_WRITE20LED(uint32_t LED)
{
		  CH423_WriteByte(&hi2c1,0x4400+(LED&0X000000FF));
			CH423_WriteByte(&hi2c1,0x4600+(uint8_t)(LED>>8));
		  CH423_WriteByte(&hi2c1,0x4801);  //设置io引脚为输出
	    CH423_WriteByte(&hi2c1,0x6000+(uint8_t)(LED>>16));
}
uint8_t CH423_READ1KEY()
{
		  return HAL_GPIO_ReadPin(PE1_KEY_GPIO_Port,PE1_KEY_Pin);
}

