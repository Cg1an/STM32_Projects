#ifndef	CH453_H
#define	CH453_H

#include "main.h"

#define  CH453_SET 0X4801   //SLEEP INTENS 0 X_INT 0 KEB DISP 00000001


void CH453_Write( unsigned short cmd );
void CH453_WriteByte( unsigned short cmd );

extern uint8_t CH453_12NUM_3LED[13];  //前面12个是数字，后面1个是LED
extern uint16_t CH453_3_10_10KEY;

//电源管理设备服务函数
void CH453_12NUM_3LED_10KEY_init(void);             //初始化
void CH453_WRITE12NUM(uint8_t * num,uint8_t length);//写数码管
void CH453_WRITE3LED(uint8_t LED);                  //写LED
uint16_t CH453_READ10KEY(void);                     //读取按键
#endif





