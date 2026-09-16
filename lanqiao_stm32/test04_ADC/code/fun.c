#include "headfile.h"

uint32_t capture_value1,fre1,capture_value2,fre2;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	 if(htim->Instance == TIM2)
	 {
		 capture_value1 = HAL_TIM_ReadCapturedValue(&htim2,TIM_CHANNEL_1);
		 TIM2->CNT = 0;
		 fre1 = (80000000)/(80*capture_value1);
	 }
	 if(htim->Instance == TIM16)
	 {
		 capture_value2 = HAL_TIM_ReadCapturedValue(&htim16,TIM_CHANNEL_1);
		 TIM16->CNT = 0;
		 fre2 = (80000000)/(80*capture_value2);
	 }
}
char text[20];
void lcd_show()
{	
	sprintf(text,"        test       ");//test字符串拷贝到 text数组里
	LCD_DisplayStringLine(Line0,(uint8_t *)text); //也会改变PC8-PC15的引进输出
	sprintf(text,"        fre1: %d   ",fre1);//test字符串拷贝到 text数组里
	LCD_DisplayStringLine(Line2,(uint8_t *)text);
	sprintf(text,"        fre2: %d   ",fre2);//test字符串拷贝到 text数组里
	LCD_DisplayStringLine(Line3,(uint8_t *)text);
	sprintf(text,"        R37_v: %.2f   ",get_vol(&hadc2));//test字符串拷贝到 text数组里
	LCD_DisplayStringLine(Line5,(uint8_t *)text);
	sprintf(text,"        R38_v: %.2f   ",get_vol(&hadc1));//test字符串拷贝到 text数组里
	LCD_DisplayStringLine(Line7,(uint8_t *)text);
	
	//led_show(1,led_mode);//这个不放在定时器中断回调函数里，原因是，若在LCD_DisplayStringLine这个函数里 执行到中间部分 跳到中断 然后没有把PC8-PC15 ODR数据重新赋值  当在中断回调函数里执行led_show() PD2制高 使能 则灯全亮。led_show()放在lcd_show() 会在LCD ODR寄存器赋值完再执行，解决灯全亮的问题。
}

double get_vol(ADC_HandleTypeDef *hadc)
{
	HAL_ADC_Start(hadc);
	uint32_t adc_value = HAL_ADC_GetValue(hadc);
	return 3.3*adc_value/4096;
}


