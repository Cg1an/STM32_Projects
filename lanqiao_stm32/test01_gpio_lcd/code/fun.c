#include "headfile.h"

int count = 0;
uint8_t led_mode;
void led_show(uint8_t led, uint8_t mode)
{
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);

    if (mode)
    {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8 << (led - 1), GPIO_PIN_RESET);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8 << (led - 1), GPIO_PIN_SET);
    }

    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
}

uint8_t B1_state; 
uint8_t B1_last_state;
uint8_t B2_state;
uint8_t B2_last_state;
uint8_t B3_state;
uint8_t B3_last_state;
uint8_t B4_state;
uint8_t B4_last_state;


void key_scan()
{
    B1_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
		B2_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1);
		B3_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2);
		B4_state = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);

    if (B1_state == 0 && B1_last_state == 1) //按键B1按下
    {
        //led_show(1, 1);
				count++;
    }
		if (B2_state == 0 && B2_last_state == 1) //按键B1按下
    {
        //led_show(1, 0);
			  count--;
    }
		if (B3_state == 0 && B3_last_state == 1) //按键B1按下
    {
        led_show(2, 1);
    }
		if (B4_state == 0 && B4_last_state == 1) //按键B1按下
    {
        led_show(2, 0);
    }

    B1_last_state = B1_state;
		B2_last_state = B2_state;
		B3_last_state = B3_state;
		B4_last_state = B4_state;

}

char text[20];
void lcd_show()
{	
	sprintf(text,"        test       ");//test字符串拷贝到 text数组里
	LCD_DisplayStringLine(Line0,(uint8_t *)text); //也会改变PC8-PC15的引进输出
	sprintf(text,"        count: %d   ",count);//test字符串拷贝到 text数组里
	LCD_DisplayStringLine(Line2,(uint8_t *)text);
	
	led_show(1,led_mode);//这个不放在定时器中断回调函数里，原因是，若在LCD_DisplayStringLine这个函数里 执行到中间部分 跳到中断 然后没有把PC8-PC15 ODR数据重新赋值  当在中断回调函数里执行led_show() PD2制高 使能 则灯全亮。led_show()放在lcd_show() 会在LCD ODR寄存器赋值完再执行，解决灯全亮的问题。
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM2) //TIM2中断函数
	{
		count++;
		led_mode++;
		led_mode = led_mode % 2; //让led_mode在 0和1这两个状态循环往复。
	}
}
