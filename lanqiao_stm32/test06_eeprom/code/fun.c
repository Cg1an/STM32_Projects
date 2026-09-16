#include "headfile.h"

uint32_t capture_value1, fre1, capture_value2, fre2;
char send_buff[20];
char text[20];
uint8_t rc_flag;
uint8_t count = 0;
uint8_t re_buff[20];

void lcd_show()
{
    sprintf(text, "        test       "); //test字符串拷贝到 text数组里
    LCD_DisplayStringLine(Line0, (uint8_t *)text); //也会改变PC8-PC15的引进输出
//    sprintf(text, "        fre1: %d   ", rec_data); //test字符串拷贝到 text数组里
//    LCD_DisplayStringLine(Line2, (uint8_t *)text);
//    sprintf(text, "        fre2: %d   ", rec_data); //test字符串拷贝到 text数组里
//    LCD_DisplayStringLine(Line3, (uint8_t *)text);
    //LCD_DisplayStringLine(Line7,(uint8_t *)text);
    //led_show(1,led_mode);//这个不放在定时器中断回调函数里，原因是，若在LCD_DisplayStringLine这个函数里 执行到中间部分 跳到中断 然后没有把PC8-PC15 ODR数据重新赋值  当在中断回调函数里执行led_show() PD2制高 使能 则灯全亮。led_show()放在lcd_show() 会在LCD ODR寄存器赋值完再执行，解决灯全亮的问题。
}

