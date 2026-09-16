#include "headfile.h"



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

uint8_t led_flag;
uint8_t B1_data, B2_data, B3_data, B4_data, B1_last, B2_last = 1, B3_last, B4_last;
uint32_t time_dece;

uint8_t lcd_mode;
uint16_t fre_r40;
//监控界面
uint16_t pwm_cf;
uint8_t pwm_cd;//占空比
uint8_t st_flag = 0; //0 未锁定 1锁定
uint8_t time[3];

uint32_t time_2s;
//统计界面
uint16_t pwm_cf_yc;
uint8_t pwm_cd_yc;//占空比
uint16_t pwm_xf;
uint16_t fre_40_yc;
uint8_t time_yc[3];
uint8_t recd_flag;


//参数界面
int32_t para_dfsr[4] = {1, 80, 100, 2000};//显示
int32_t para_dfsr_last[4] = {1, 80, 100, 2000};//换算
int32_t para_dfsr_true[4] = {1, 80, 100, 2000};//上一次的值
uint8_t para_index;



void key_scan(void)
{
    if (uwTick - time_dece < 10)
    {
        return;
    }

    time_dece = uwTick;
    B1_data = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
    B2_data = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1);
    B3_data = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2);
    B4_data = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);

    if (!B1_data & B1_last) //B1  检测从"高电平→低电平"的下降沿 上次为1（松开），这次为0（按下）→ 表示按键被按下
    {
        //        led_flag ^= 1;
        if (lcd_mode == 1)
        {
            //统计界面 ，即将进入参数界面 保存上一次的参数
            para_dfsr_last[0] = para_dfsr_true[0];
            para_dfsr_last[1] = para_dfsr_true[1];
            para_dfsr_last[2] = para_dfsr_true[2];
            para_dfsr_last[3] = para_dfsr_true[3];
        }

        if (++lcd_mode > 2) //参数界面2->监控界面0
        {
            lcd_mode = 0;
            para_index = 0;

            //退出参数界面
            if ((para_dfsr[0] >= (para_dfsr[1] - para_dfsr[0])) ||( para_dfsr[2] >= (para_dfsr[3] - para_dfsr[2])))
            {
                para_dfsr_true[0] = para_dfsr_last[0];
                para_dfsr_true[1] = para_dfsr_last[1];
                para_dfsr_true[2] = para_dfsr_last[2];
                para_dfsr_true[3] = para_dfsr_last[3];
							  para_dfsr[0] = para_dfsr_last[0];
                para_dfsr[1] = para_dfsr_last[1];
                para_dfsr[2] = para_dfsr_last[2];
                para_dfsr[3] = para_dfsr_last[3];
            }
            else
            {
								para_dfsr_true[0] = para_dfsr[0];
                para_dfsr_true[1] = para_dfsr[1];
                para_dfsr_true[2] = para_dfsr[2];
                para_dfsr_true[3] = para_dfsr[3];
            }
        }
    }

    if (lcd_mode == 2)//参数界面
    {
        if (!B2_data & B2_last) //B2 切换修改哪一行
        {
            if (++para_index > 3)
            {
                para_index = 0;    //索引
            }
        }

        if (!B3_data & B3_last) //B3
        {
            para_dfsr[para_index] += pow(10, para_index);
        }

        if (!B4_data & B4_last) //B4
        {
            para_dfsr[para_index] -= pow(10, para_index);
        }
    }
    else if (lcd_mode == 0) //监控界面
    {
        if (!B2_data & B2_last) //B2按下 B2_data=0,B2_last=1
        {
            time_2s = uwTick;
        }

        if (B2_data & !B2_last) //B2抬起 B2_data=1,B2_last=0
        {
            if (uwTick - time_2s > 1999) //长按
            {
                time[0] = 0;
                time[1] = 0;
                time[2] = 0;
            }
            else
            {
                st_flag ^= 1; //锁定标志位切换 异或相同为0 不同为1
            }
        }
    }

    B1_last = B1_data;
    B2_last = B2_data;
    B3_last = B3_data;
    B4_last = B4_data;
}



char text[20];
uint8_t lcd_mode;
uint32_t time_lcd;
void lcd_show(void)
{
    if (uwTick - time_lcd < 99)
    {
        return;
    }

    time_lcd = uwTick;
    uint16_t temp = GPIOC->ODR;

    if (lcd_mode == 0) //监控
    {
        sprintf(text, "       PWM          ");
        LCD_DisplayStringLine(Line1, (uint8_t *)text);
        sprintf(text, "   CF=%dHz          ", pwm_cf);
        LCD_DisplayStringLine(Line3, (uint8_t *)text);
        sprintf(text, "   CD=%d%%          ", pwm_cd);
        LCD_DisplayStringLine(Line4, (uint8_t *)text);
        sprintf(text, "   DF=%dHz          ", fre_r40);
        LCD_DisplayStringLine(Line5, (uint8_t *)text);

        if (st_flag == 0)
        {
            sprintf(text, "   ST=UNLOCK        ");
        }
        else
        {
            sprintf(text, "   ST=LOCK          ");
        }

        LCD_DisplayStringLine(Line6, (uint8_t *)text);
        sprintf(text, "   %02dH%02dM%02dS        ", time[0], time[1], time[2]);
        LCD_DisplayStringLine(Line7, (uint8_t *)text);
        //			sprintf(text, "   %d        ",fre_r40);
        //    LCD_DisplayStringLine(Line8, (uint8_t *)text);
    }
    else if (lcd_mode == 1) //统计
    {
        sprintf(text, "       RECD         ");
        LCD_DisplayStringLine(Line1, (uint8_t *)text);
        sprintf(text, "   CF=%dHz          ", pwm_cf_yc);
        LCD_DisplayStringLine(Line3, (uint8_t *)text);
        sprintf(text, "   CD=%d%%          ", pwm_cd_yc);
        LCD_DisplayStringLine(Line4, (uint8_t *)text);
        sprintf(text, "   DF=%dHz          ", fre_40_yc);
        LCD_DisplayStringLine(Line5, (uint8_t *)text);
        sprintf(text, "   XF=%dHz          ", pwm_xf);
        LCD_DisplayStringLine(Line6, (uint8_t *)text);
        sprintf(text, "   %02dH%02dM%02dS        ", time_yc[0], time_yc[1], time_yc[2]);
        LCD_DisplayStringLine(Line7, (uint8_t *)text);
    }
    else
    {
        sprintf(text, "       PARA         ");
        LCD_DisplayStringLine(Line1, (uint8_t *)text);
        sprintf(text, "   DS=%d%%          ", para_dfsr[0]);
        LCD_DisplayStringLine(Line3, (uint8_t *)text);
        sprintf(text, "   DR=%d%%          ", para_dfsr[1]);
        LCD_DisplayStringLine(Line4, (uint8_t *)text);
        sprintf(text, "   FS=%dHz          ", para_dfsr[2]);
        LCD_DisplayStringLine(Line5, (uint8_t *)text);
        sprintf(text, "   FR=%dHz          ", para_dfsr[3]);
        LCD_DisplayStringLine(Line6, (uint8_t *)text);
        LCD_ClearLine(Line7);
    }

    GPIOC->ODR = temp;
}
double adc_read(ADC_HandleTypeDef *hadc)
{
    HAL_ADC_Start(hadc);
    //uint32_t adc_value = HAL_ADC_GetValue(hadc);
    return 3.3 * HAL_ADC_GetValue(hadc) / 4096;
}
double adc_r37, adc_r38;
void data_proc(void)
{
    adc_r37 = adc_read(&hadc2);
    adc_r38 = adc_read(&hadc1);

    if (!st_flag)
    {
        //参数界面 阶梯计算 占空比
        uint16_t count = (para_dfsr_last[1] - 10) / para_dfsr_last[0];
        double va = 3.3 / (count + 1);

        for (int i = 0 ; i < count + 1; i++)
        {
            if (adc_r37 >= i * va && adc_r37 < (i + 1)*va)
            {
                TIM17->CCR1 = 10 + i * para_dfsr_last[0];
                break;
            }
        }

        //参数界面 阶梯计算 频率
        uint16_t count_f = (para_dfsr_last[3] - 1000) / para_dfsr_last[2];
        double va_f = 3.3 / (count_f + 1);

        for (int j = 0 ; j < count_f + 1; j++)
        {
            if (adc_r38 >= j * va_f && adc_r38 < (j + 1)*va_f)
            {
                //f=1000+j*para_dfsr[2]
                TIM17->PSC = 800000 / (1000 + j * para_dfsr_last[2]) -
                             1; //f = 80000000/(PSC+1)*(ARR+1)  f = 800000/(PSC+1)  psc= (800000/f) -1
                break;
            }
        }

        //监控界面
        pwm_cf = 800000 / (TIM17->PSC + 1); //f = 80000000/(PSC+1)*(ARR+1)
        pwm_cd = TIM17->CCR1;

        //统计界面 异常
        if (!recd_flag)
        {
            if (abs(fre_r40 - pwm_cf) > 1000)
            {
                recd_flag = 1;//异常
                pwm_cf_yc = pwm_cf;//异常频率
                pwm_cd_yc = pwm_cd;//异常占空比
                fre_40_yc = fre_r40;//异常捕获
                pwm_xf = abs(fre_r40 - pwm_cf);

                for (int i = 0; i < 3; i++)
                {
                    time_yc[i] = time[i];
                }
            }
        }
        else
        {
            if (abs(fre_r40 - pwm_cf) <= 1000)
            {
                recd_flag = 0;//无异常
            }
        }
    }
}
void main_pros(void)
{
		led_show(1, (lcd_mode ==0));
    led_show(2, (st_flag ==1));
	  led_show(3, (recd_flag ==1));
    data_proc();
    lcd_show();
    key_scan();
}
//HAL_TIM_IC_CaptureCallback  输入捕获中断回调函数，用于捕获外部信号的事件
//HAL_TIM_PeriodElapsedCallback 定时器溢出/更新中断回调函数，用于定时时间到的事件
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) //2534 tim.h
{
    if (htim->Instance == TIM2)
    {
        fre_r40 = 1000000 / (TIM2->CCR1 + 1);
        TIM2->CNT = 0;
    }
}
//HAL_TIM_PeriodElapsedCallback 定时器溢出/更新中断回调函数，用于定时时间到的事件
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)   //2531 tim.h
{
    if (htim->Instance == TIM3)
    {
        if (++time[2] > 59)
        {
            time[2] = 0;

            if (++time[1] > 59)
            {
                time[1] = 0;

                if (++time[0] > 99)
                {
                    time[0] = 0;
                    time[1] = 0;
                    time[2] = 0;
                }
            }
        }
    }
}
