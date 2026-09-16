#include "fun.h"


uint8_t b1_data,b1_last;
uint8_t time;
uint32_t time_1s;
uint8_t led_flag;
uint8_t key_flag_1s;
void key_scan(void)
{
	if(uwTick - time <10)
	{
		return;
	}
	time = uwTick;
	b1_data = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
	if(!b1_data & !b1_last)
	{
		key_flag_1s=1;
		if(time_1s > 3999)
		{
			time_1s=0;
			led_flag^=1;	
			led_show(led_flag);
		}
	}
	else if(b1_data & !b1_last)
	{
		key_flag_1s=0;
		time_1s=0;
	}
	
	
	b1_last = b1_data;
}
void led_show(uint8_t led)
{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
	if(!led)
	{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET);
	}
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance ==TIM4)
	{
		if(key_flag_1s)
		{time_1s++;}
	}
}
