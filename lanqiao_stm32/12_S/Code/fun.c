#include "headfile.h"
//串口

//接收数据
char uart_str[19];//返回字符串
uint8_t tran_data, rec_data;
uint8_t rec_flag;
uint8_t rec_cat_data[23];
uint8_t rec_cat_data_index;
uint8_t cat_s_index;
uint8_t cat_data_index;
char cat_lx[9][5]; //存储车辆类型
char cat_shijian[9][8]; //存储车辆停车时间
uint8_t cat_lx_index;//临时存储
char cat_lx_flag[5];//临时存储
uint8_t flag_for=0;
uint8_t flag_for_last;
uint8_t stop_flag;
uint8_t out_flag;
uint8_t stop_time;//停车时间
uint8_t cat_yuan;//停车费用

char uart_error[5] = {'E', 'R', 'R', 'O', 'R'};
//PWM
uint8_t pwm_flag;
//lcd
char text[20];
uint8_t lcd_mode;
//key
uint8_t b1_data, b2_data, b3_data, b4_data, b1_last, b2_last, b3_last, b4_last;
uint32_t time_key;
//车位显示界面
uint8_t data_cw[3] = {0, 0, 8};
//费率界面
float para[2] = {3.50, 2.00};
//接收数据
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
void key_scan(void)
{
    if ((uwTick - time_key) < 10)
    {
        return;
    }

    time_key = uwTick;
    b1_data = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
    b2_data = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1);
    b3_data = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2);
    b4_data = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);

    if (!b1_data & b1_last)
    {
        if (++lcd_mode > 1)
        {
            lcd_mode = 0;
        }
    }

    if (lcd_mode == 1)
    {
        if (!b2_data & b2_last)
        {
            para[0] += 0.5f;
            para[1] += 0.5f;
        }

        if (!b3_data & b3_last)
        {
            para[0] -= 0.5f;
            para[1] -= 0.5f;
        }
    }

    if (!b4_data & b4_last)
    {
        pwm_flag ^= 1;

        if (pwm_flag)
        {
            TIM17->CCR1 = 20;
            //            led_show(2, 0);
            //            led_show(1, 1);
        }
        else
        {
            TIM17->CCR1 = 0;
            //            led_show(2, 1);
            //            led_show(1, 0);
        }
    }

    b1_last = b1_data;
    b2_last = b2_data;
    b3_last = b3_data;
    b4_last = b4_data;
}
void lcd_show(void)
{
    uint16_t temp = GPIOC->ODR;

    if (lcd_mode == 0)
    {
        sprintf(text, "       Data         ");
        LCD_DisplayStringLine(Line1, (uint8_t *)text);
        sprintf(text, "   CNBR:%d          ", data_cw[0]);
        LCD_DisplayStringLine(Line3, (uint8_t *)text);
        sprintf(text, "   VNBR:%d          ", data_cw[1]);
        LCD_DisplayStringLine(Line5, (uint8_t *)text);
        sprintf(text, "   IDLE:%d          ", data_cw[2]);
        LCD_DisplayStringLine(Line7, (uint8_t *)text);
    }
    else
    {
        sprintf(text, "       Para         ");
        LCD_DisplayStringLine(Line1, (uint8_t *)text);
        sprintf(text, "   CNBR:%.2f        ", para[0]);
        LCD_DisplayStringLine(Line3, (uint8_t *)text);
        sprintf(text, "   VNBR:%.2f        ", para[1]);
        LCD_DisplayStringLine(Line5, (uint8_t *)text);
        LCD_ClearLine(Line7);
    }

    GPIOC->ODR = temp;
}
void data_pro(void)
{
}
void rec_data_pro()
{
    if (rec_flag)
    {
        if (TIM4->CNT > 15)
        {
            if (rec_cat_data[0] == 'C' && rec_cat_data[1] == 'N' && rec_cat_data[2] == 'B' && rec_cat_data[3] == 'R'
                    && rec_cat_data[4] == ':')
            {
                for (int i = 0; i < 4; i++)
                {
                    cat_lx_flag[i] = rec_cat_data[i + 5];
                }
								cat_lx_flag[4]='\0';
                for (flag_for = 0; flag_for < 8; flag_for++)
                {
                    if (strcmp(cat_lx_flag, cat_lx[flag_for]) == 0)
                    {
                        stop_flag = 1;
                        break;
                    }
                }

                if (stop_flag) //output
                {
                    //费用
										if(rec_cat_data[19]=='0'&&rec_cat_data[20]=='0'&&rec_cat_data[21]=='0'&&rec_cat_data[22]=='0')
										{
											stop_time = ((cat_shijian[flag_for][0]-'0')-(rec_cat_data[15]-'0'))*24+(rec_cat_data[16]-'0')*10+(rec_cat_data[17]-'0')-
											(cat_shijian[flag_for][1]-'0')*10-(cat_shijian[flag_for][2]-'0');
										}
										else
										{
											//停车小时数加1
											stop_time = 1+((cat_shijian[flag_for][0]-'0')-(rec_cat_data[15]-'0'))*24+(rec_cat_data[16]-'0')*10+(rec_cat_data[17]-'0')-
											(cat_shijian[flag_for][1]-'0')*10-(cat_shijian[flag_for][2]-'0');
										}
                    //
                    data_cw[0]--;
                    data_cw[2]++;
                    stop_flag = 0;
                    out_flag = 1;
                    flag_for_last = flag_for;//保存上一次出去车的索引

                    for (int i = 0; i < 4; i++)
                    {
                        cat_lx[flag_for][i] = 0;
                    }
										for (int i = 0; i < 8; i++)//时间
                    {
                            cat_shijian[cat_lx_index][i] = rec_cat_data[i + 15];
                    }

                    sprintf(uart_str, "shijian:%d",stop_time);
                    HAL_UART_Transmit(&huart1, (uint8_t *)uart_str, sizeof(uart_str), 50);
                }
                else//input
                {
                    data_cw[0]++;
                    data_cw[2]--;

                    if (!out_flag) //没车出去
                    {
                        //进入车辆存储
                        for (int i = 0; i < 4; i++)//类型
                        {
                            cat_lx[flag_for][i] = rec_cat_data[i + 5];
                        }
												cat_lx[cat_lx_index][4] = '\0';
												for (int i = 0; i < 8; i++)//时间
                        {
                            cat_shijian[cat_lx_index][i] = rec_cat_data[i + 15];
                        }

                        if (++cat_lx_index > 7)
                        {
                            cat_lx_index = 0 ;
                        }
                    }
                    else //有车出去 填坑
                    {
                        for (int i = 0; i < 4; i++)
                        {
                            cat_lx[flag_for_last][i] = rec_cat_data[i + 5];
                        }
												cat_lx[cat_lx_index][4] = '\0';

                        if (++cat_lx_index > 7)
                        {
                            cat_lx_index = 0 ;
                        }

                        out_flag = 0;
                    }

                    sprintf(uart_str, "input");
                    HAL_UART_Transmit(&huart1, (uint8_t *)uart_str, sizeof(uart_str), 50);
                }
            }
            else if
            (rec_cat_data[0] == 'V' && rec_cat_data[1] == 'N' && rec_cat_data[2] == 'B' && rec_cat_data[3] == 'R'
                    && rec_cat_data[4] == ':')
            {
                sprintf(uart_str, "ok");
                HAL_UART_Transmit(&huart1, (uint8_t *)uart_str, sizeof(uart_str), 50);
            }
            else
            {
                HAL_UART_Transmit(&huart1, (uint8_t *)uart_error, sizeof(uart_error), 50);
            }

            rec_flag = 0;
            rec_cat_data_index = 0;
        }
    }
}
void main_pro(void)
{
    // led_show(1, 1);
    key_scan();
    lcd_show();
    data_pro();
    rec_data_pro();
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        TIM4->CNT = 0;
        rec_flag = 1;
        rec_cat_data[rec_cat_data_index++] = rec_data;
        HAL_UART_Receive_IT(huart, &rec_data, 1);
    }
}
