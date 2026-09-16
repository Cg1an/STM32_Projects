#include "headfile.h"

uint8_t lcd_mode;
//数据界面
uint8_t ft_flag;
//频率
int16_t fre_a, fre_b;
double value_f_a, value_f_b;
uint16_t fre_a_z, fre_a_x, fre_b_z, fre_b_x;
//周期
double t_a_s, t_b_s, t_a_ms, t_b_ms;
uint16_t t_a_us, t_b_us;
double value_t_a, value_t_b;
uint16_t t_a_z, t_a_x, t_b_z, t_b_x;
//参数界面
int16_t para[3] = {1000, 5000, 0};//PD PH PX
uint8_t para_index;
//统计界面
uint8_t recd[4];//NDA NDB NHA NHB
uint8_t NHA_flag = 1; //标志位 为了确保频率是由小变大 超过PH时 超限记录，而不是频率由大变小 超过PH时  超限记录
uint8_t NHB_flag = 1;
//按键
uint8_t b1_data, b2_data, b3_data, b4_data, b1_last, b2_last, b3_last, b4_last;
uint8_t time;
uint8_t time_lcd;

uint16_t count_1s;
uint16_t count_3s;
int16_t frea_max, freb_max;
int16_t frea_min = 20000;
int16_t freb_min = 20000;
uint8_t led_flag;
uint8_t pd_flag_a;
uint8_t pd_flag_b;
uint8_t long_flag;

void key_scan(void)
{
    if (uwTick - time < 10)
    {
        return;
    }

    time = uwTick;
    b1_data = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
    b2_data = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1);
    b3_data = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2);
    b4_data = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);

    //数据界面
    if (lcd_mode == 0)
    {
        if (!b3_data & b3_last)
        {
            ft_flag ^= 1;
        }
    }

    //参数界面
    if (lcd_mode == 1)
    {
        if (!b1_data & b1_last)
        {
            para[para_index] += 100;

            if (para_index == 0)
            {
                if (para[para_index] >= 1000)
                {
                    para[para_index] = 1000;
                }
            }

            if (para_index == 1)
            {
                if (para[para_index] >= 10000)
                {
                    para[para_index] = 10000;
                }
            }

            if (para_index == 2)
            {
                if (para[para_index] >= 1000)
                {
                    para[para_index] = 1000;
                }
            }
        }

        if (!b2_data & b2_last)
        {
            para[para_index] -= 100;

            if (para_index == 0)
            {
                if (para[para_index] <= 100)
                {
                    para[para_index] = 100;
                }
            }

            if (para_index == 1)
            {
                if (para[para_index] <= 1000)
                {
                    para[para_index] = 1000;
                }
            }

            if (para_index == 2)
            {
                if (para[para_index] <= -1000)
                {
                    para[para_index] = -1000;
                }
            }
        }

        if (!b3_data & b3_last)
        {
            if (++para_index > 2)
            {
                para_index = 0;
            }
        }
    }

    if (!b4_data & b4_last)
    {
        if (++lcd_mode > 2)
        {
            lcd_mode = 0;
            ft_flag = 0;
        }

        if (lcd_mode == 1)
        {
            para_index = 0;
        }
    }

    if (lcd_mode == 2)
    {
        if (!b3_data && !b3_last)
        {
            long_flag = 1;

            if (count_1s > 1000)
            {
      
								count_1s=0;
                recd[0]=0;
								recd[1]=0;
								recd[2]=0;
								recd[3]=0;
            }
        }
//				else if (b3_data & !b3_last)
				else if (b3_data)
				{
					long_flag=0;
					count_1s=0;
				}
    }

    b1_last = b1_data;
    b2_last = b2_data;
    b3_last = b3_data;
    b4_last = b4_data;
}
void led_show(uint8_t led, uint8_t led_mode)
{
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);

    if (led_mode)
    {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8 << (led - 1), GPIO_PIN_RESET);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8 << (led - 1), GPIO_PIN_SET);
    }

    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
}
char text[20];
void lcd_show(void)
{
		if (uwTick - time_lcd < 99)
    {
        return;
    }

    time_lcd = uwTick;
    uint16_t temp = GPIOC->ODR;

    if (lcd_mode == 0)
    {
        if (!ft_flag)//数据-频率界面
        {
            sprintf(text, "        DATA        ");
            LCD_DisplayStringLine(Line1, (uint8_t *)text);

            if (fre_a > 1000)
            {
                //                value_f_a = fre_a / 1000.0f;
                //                fre_a_z = (uint16_t)(value_f_a);
                //                fre_a_x = (uint16_t)((value_f_a - fre_a_z) * 10000.0f + 0.5f);
                //sprintf(text, "     A=%d.%02dKHZ         ", fre_a_z, fre_a_x);
                sprintf(text, "     A=%.2fKHz         ", (float)fre_a / 1000);
                LCD_DisplayStringLine(Line3, (uint8_t *)text);
            }
            else if (fre_a >= 0)
            {
                sprintf(text, "     A=%dHz         ", fre_a);
                LCD_DisplayStringLine(Line3, (uint8_t *)text);
            }
            else
            {
                sprintf(text, "     A=NULL         ");
                LCD_DisplayStringLine(Line3, (uint8_t *)text);
            }

            if (fre_b > 1000)
            {
                //                value_f_b = fre_b / 1000.0f;
                //                fre_b_z = (uint16_t)(value_f_b);
                //                fre_b_x = (uint16_t)((value_f_b - fre_b_z) * 10000.0f + 0.5f);
                sprintf(text, "     B=%.2fKHz      ", (float)fre_b / 1000);
                LCD_DisplayStringLine(Line4, (uint8_t *)text);
            }
            else if (fre_b >= 0)
            {
                sprintf(text, "     B=%dHz        ", fre_b);
                LCD_DisplayStringLine(Line4, (uint8_t *)text);
            }
            else
            {
                sprintf(text, "     B=NULL        ");
                LCD_DisplayStringLine(Line4, (uint8_t *)text);
            }

            LCD_ClearLine(Line5);
            LCD_ClearLine(Line6);
        }
        else //数据-周期界面
        {
            sprintf(text, "        DATA        ");
            LCD_DisplayStringLine(Line1, (uint8_t *)text);
            t_a_ms = (1.0f / fre_a) * 1000.0f;
            t_b_ms = (1.0f / fre_b) * 1000.0f;
            t_a_us = (uint16_t)((1.0f / fre_a) * 1000000.0f);
            t_b_us = (uint16_t)((1.0f / fre_b) * 1000000.0f);

            if (fre_a >= 0)
            {
                if (t_a_us > 1000)
                {
                    sprintf(text, "     A=%.3fmS       ", t_a_ms);
                    LCD_DisplayStringLine(Line3, (uint8_t *)text);
                }
                else
                {
                    sprintf(text, "     A=%duS         ", t_a_us);
                    LCD_DisplayStringLine(Line3, (uint8_t *)text);
                }
            }
            else
            {
                sprintf(text, "     A=NULL         ");
                LCD_DisplayStringLine(Line3, (uint8_t *)text);
            }

            if (fre_b >= 0)
            {
                if (t_b_us > 1000)
                {
                    sprintf(text, "     B=%.3fmS       ", t_b_ms);
                    LCD_DisplayStringLine(Line4, (uint8_t *)text);
                }
                else
                {
                    sprintf(text, "     B=%duS         ", t_b_us);
                    LCD_DisplayStringLine(Line4, (uint8_t *)text);
                }
            }
            else
            {
                sprintf(text, "     B=NULL         ");
                LCD_DisplayStringLine(Line4, (uint8_t *)text);
            }

            LCD_ClearLine(Line5);
            LCD_ClearLine(Line6);
        }
    }
    else if (lcd_mode == 1)//参数界面
    {
        sprintf(text, "        PARA        ");
        LCD_DisplayStringLine(Line1, (uint8_t *)text);
        sprintf(text, "     PD=%dHz        ", para[0]);
        LCD_DisplayStringLine(Line3, (uint8_t *)text);
        sprintf(text, "     PH=%dHz        ", para[1]);
        LCD_DisplayStringLine(Line4, (uint8_t *)text);
        sprintf(text, "     PX=%dHz        ", para[2]);
        LCD_DisplayStringLine(Line5, (uint8_t *)text);
        LCD_ClearLine(Line6);
    }
    else if (lcd_mode == 2)//统计界面
    {
        sprintf(text, "        RECD        ");
        LCD_DisplayStringLine(Line1, (uint8_t *)text);
        sprintf(text, "     NDA=%d         ", recd[0]);
        LCD_DisplayStringLine(Line3, (uint8_t *)text);
        sprintf(text, "     NDB=%d         ", recd[1]);
        LCD_DisplayStringLine(Line4, (uint8_t *)text);
        sprintf(text, "     NHA=%d         ", recd[2]);
        LCD_DisplayStringLine(Line5, (uint8_t *)text);
        sprintf(text, "     NHB=%d         ", recd[3]);
        LCD_DisplayStringLine(Line6, (uint8_t *)text);
    }

    GPIOC->ODR = temp;
}
void data_pros(void)
{
    if (NHA_flag == 0)
    {
        if (fre_a > para[1])
        {
            recd[2]++;
            NHA_flag = 1;
        }
    }
    else
    {
        if (fre_a < para[1])
        {
            NHA_flag = 0;
        }
    }

    if (NHB_flag == 0)
    {
        if (fre_b > para[1])
        {
            recd[3]++;
            NHB_flag = 1;
        }
    }
    else
    {
        if (fre_b < para[1])
        {
            NHB_flag = 0;
        }
    }

    //PD
    if (count_3s > 3000)
    {
        count_3s = 0;
        frea_max = 0;
        freb_max = 0;
        frea_min = 20000;
        freb_min = 20000;
        pd_flag_a = 0;
        pd_flag_b = 0;
    }
    else
    {
        if (fre_a > frea_max)
        {
            frea_max = fre_a;
        }

        if (fre_a < frea_min)
        {
            frea_min = fre_a;
        }

        if (fre_b > freb_max)
        {
            freb_max = fre_b;
        }

        if (fre_b < freb_min)
        {
            freb_min = fre_b;
        }

        if (!pd_flag_a)
        {
            if ((frea_max - frea_min) > para[0])
            {
                recd[0]++;
                pd_flag_a = 1;
            }
        }

        if (!pd_flag_b)
        {
            if ((freb_max - freb_min) > para[0])
            {
                recd[1]++;
                pd_flag_b = 1;
            }
        }
    }
}
void main_pros(void)
{
    led_show(1, (lcd_mode==0));
		led_show(2, (fre_a>para[1]));
		led_show(3, (fre_b>para[1]));
		led_show(8, (recd[0]>2 || recd[1]>2));
    lcd_show();
    key_scan();
    data_pros();
}
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2)
    {
        fre_a = 1000000 / (TIM2->CCR1 + 1);//(TIM2->CCR1 + 1) =周期（us）
        TIM2->CNT = 0; //***** 重点

        if (fre_a <= 400)
        {
            fre_a = 400;
        }
        else if (fre_a >= 20000)
        {
            fre_a = 20000;
        }

        fre_a += para[2];
    }

    if (htim->Instance == TIM3)
    {
        fre_b = 1000000 / (TIM3->CCR1 + 1);
        TIM3->CNT = 0;//****** 重点

        if (fre_b <= 400)
        {
            fre_b = 400;
        }
        else if (fre_b >= 20000)
        {
            fre_b = 20000;
        }

        fre_b += para[2];
    }
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM4)
    {
        if (long_flag)
        {
            count_1s++;
        }

        count_3s++;
    }
}
