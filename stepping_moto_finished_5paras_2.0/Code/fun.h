#ifndef _FUN_H
#define _FUN_H

#include "headfile.h"

#define up 1
#define down 0
void OLED_Fill(uint8_t data);
void OLED_Init(void);
void OLED_WriteCommand(uint8_t cmd);
void OLED_ShowString(uint8_t x, uint8_t y, char *str) ;
void main_pro(void);
void motor_start(uint8_t fangxiang, uint16_t val_line1_sd,uint16_t val_line2_t);
void Load_Parameters(void);
void Save_Parameters(void);
extern uint16_t val_line1;

#endif
