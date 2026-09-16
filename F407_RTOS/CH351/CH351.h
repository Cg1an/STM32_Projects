#ifndef	CH351_H
#define	CH351_H

#include "main.h"

#define  CH351_RD_SET       HAL_GPIO_WritePin(CH351_RD_GPIO_Port,CH351_RD_Pin,GPIO_PIN_SET);
#define  CH351_RD_RESET     HAL_GPIO_WritePin(CH351_RD_GPIO_Port,CH351_RD_Pin,GPIO_PIN_RESET);

#define  CH351_WR_SET       HAL_GPIO_WritePin(CH351_WR_GPIO_Port,CH351_WR_Pin,GPIO_PIN_SET);
#define  CH351_WR_RESET     HAL_GPIO_WritePin(CH351_WR_GPIO_Port,CH351_WR_Pin,GPIO_PIN_RESET);
 
#define  CH351_CS_SET       HAL_GPIO_WritePin(CH351_CS_GPIO_Port,CH351_CS_Pin,GPIO_PIN_SET);
#define  CH351_CS_RESET     HAL_GPIO_WritePin(CH351_CS_GPIO_Port,CH351_CS_Pin,GPIO_PIN_RESET);

#define  CH351_CS2_SET       HAL_GPIO_WritePin(CH351_CS2_GPIO_Port,CH351_CS2_Pin,GPIO_PIN_SET);
#define  CH351_CS2_RESET     HAL_GPIO_WritePin(CH351_CS2_GPIO_Port,CH351_CS2_Pin,GPIO_PIN_RESET);

#define  CH351_CS3_SET       HAL_GPIO_WritePin(CH351_CS3_GPIO_Port,CH351_CS3_Pin,GPIO_PIN_SET);
#define  CH351_CS3_RESET     HAL_GPIO_WritePin(CH351_CS3_GPIO_Port,CH351_CS3_Pin,GPIO_PIN_RESET);

#define  CH351_RST_SET      HAL_GPIO_WritePin(CH351_RST_GPIO_Port,CH351_RST_Pin,GPIO_PIN_SET);
#define  CH351_RST_RESET    HAL_GPIO_WritePin(CH351_RST_GPIO_Port,CH351_RST_Pin,GPIO_PIN_RESET);

#define  CH351_A0_SET       HAL_GPIO_WritePin(CH351_A0_GPIO_Port,CH351_A0_Pin,GPIO_PIN_SET);
#define  CH351_A0_RESET     HAL_GPIO_WritePin(CH351_A0_GPIO_Port,CH351_A0_Pin,GPIO_PIN_RESET);

#define  CH351_A1_SET       HAL_GPIO_WritePin(CH351_A1_GPIO_Port,CH351_A1_Pin,GPIO_PIN_SET);
#define  CH351_A1_RESET     HAL_GPIO_WritePin(CH351_A1_GPIO_Port,CH351_A1_Pin,GPIO_PIN_RESET);
 
#define  CH351_A2_SET       HAL_GPIO_WritePin(CH351_A2_GPIO_Port,CH351_A2_Pin,GPIO_PIN_SET);
#define  CH351_A2_RESET     HAL_GPIO_WritePin(CH351_A2_GPIO_Port,CH351_A2_Pin,GPIO_PIN_RESET);


//WR=0 RD=1 向351写入数据
//WR=1 RD=0 从351读出数据
//CS片选引脚 0为选中1为挂起
//A2 A1 A0 读写地址片选引脚
//1  0  0  写入 CH351 的第一组 GPIO 的方向控制寄存器     ,默认为 1，对应输出高电平，如果设置为 0 则对应输出低电平
//0  0  0  写入 CH351 的第一组 GPIO 的内部输出数据寄存器 ,默认为 0，表示该 GPIO 引脚为输入引脚，如果被设置为 1 则表示该 GPIO 引脚为输出引脚。
//1  0  1  写入 CH351 的第二组 GPIO 的方向控制寄存器
//0  0  1  写入 CH351 的第二组 GPIO 的内部输出数据寄存器
//1  1  0  写入 CH351 的第三组 GPIO 的方向控制寄存器
//0  1  0  写入 CH351 的第三组 GPIO 的内部输出数据寄存器
//1  1  1  写入 CH351 的第四组 GPIO 的方向控制寄存器
//0  1  1  写入 CH351 的第四组 GPIO 的内部输出数据寄存器

//当引脚被配置成输入模式时，引脚输入低电平会触发中断 int引脚会变成低电平

#define CH351_MODE_GPIO1     0X04
#define CH351_MODE_GPIO2     0X05
#define CH351_MODE_GPIO3     0X06
#define CH351_MODE_GPIO4     0X07

#define CH351_GPIO1    0X00
#define CH351_GPIO2    0X01
#define CH351_GPIO3    0X02
#define CH351_GPIO4    0X03




//A0_A2设置函数 低三位有效
void A0_A2_SET(uint8_t A0_2);    //初始化A0-A2
void CH351_WRITE_SET(uint8_t A0_2,uint8_t data);  //写数据
uint8_t CH351_READ_SET(uint8_t A0_2);             //读数据

void CH351_WRITE2_SET(uint8_t A0_2,uint8_t data);
uint8_t CH351_READ2_SET(uint8_t A0_2);             //读数据

void CH351_WRITE3_SET(uint8_t A0_2,uint8_t data);
uint8_t CH351_READ3_SET(uint8_t A0_2);             //读数据


//网络通信控制和设备服务函数
void CH351_20LED_2KEY_init(void);          //初始化输入输出
void CH351_WRITE_20LED(uint32_t LED_SET);  //配置24个led的亮灭
uint8_t CH351_READ_2KEY(void);             //读取两个按键的状态


//BD时钟接线盒设备服务函数
void CH351_LCD_4LED_21KEYinit(void);
uint32_t CH351_READ_21KEY(void);
void CH351_WRITE_4LED(uint8_t LED_SET);


//综合接口设备服务函数
void CH351_24LED_2KEY_init(void);
uint8_t CH351_READ_24_2KEY(void);
void CH351_WRITE_24LED(uint32_t LED_SET);

//司机右面板接口函数
void CH351_8LED_24KEY_init(void);
uint32_t CH351_READ_8_24KEY(void);
void CH351_WRITE_8LED(uint8_t LED_SET);

//车长仓使用司机右边面板驱动7key
void CH351_24KEY_GET7(void);
void CH351_7KEY_ONLY_init(void);
void CH351_8LED_WRITE(uint8_t LED);

//电源管理器
void CH351_7KEY_ONLY_init_CN1(void);
void CH351_24KEY_GET7_CN1(void);
void CH351_8LED_WRITE_CN1(uint8_t LED);

//lunshi_drive
void CH351_lunshi_init(void);
void CH351_24KEY_ALL(void);
void CH351_8LED_ALL(uint8_t LED);




extern uint8_t CH351_8LED;    // 车长厂的1个LED
extern uint8_t CH351_7_8KEY;  //车长厂的7或者8key
extern uint16_t CH351_10KEY;

extern uint8_t  lunshi_LED_8;
extern uint32_t lunshi_ALL_KEY;







#endif



