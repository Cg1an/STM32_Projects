#ifndef _WS2812_H
#define _WS2812_H

#include "main.h"

#define PIXEL_NUM_FRAME  59
#define NUM_FRAME (24*PIXEL_NUM_FRAME+ 224)        // Reset 280us / 1.25us = 224

#define PIXEL_NUM_REC 320                        //加上R标为320个
#define NUM_REC (24*PIXEL_NUM_REC + 224)        // Reset 280us / 1.25us = 224

#define PIXEL_NUM_REC_ 256                       //加上R标为320个
#define NUM_REC_ (24*PIXEL_NUM_REC_ + 224)        // Reset 280us / 1.25us = 224



#define PIXEL_NUM_FRAME_UP  88
#define NUM_FRAME_UP (24*PIXEL_NUM_FRAME_UP + 224)        // Reset 280us / 1.25us = 224

#define PIXEL_NUM_FRAME_DOWN  92
#define NUM_FRAME_DOWN (24*PIXEL_NUM_FRAME_DOWN + 224)        // Reset 280us / 1.25us = 224

#define PIXEL_NUM_CIRCLE  275
#define NUM_CIRCLE (24*PIXEL_NUM_CIRCLE + 224)        // Reset 280us / 1.25us = 224

#define PIXEL_TEN_RING_NUM  4
#define TEN_RING_NUM (24*PIXEL_TEN_RING_NUM + 224) 

#define PIXEL_NINE_RING_NUM  8
#define NINE_RING_NUM (24*PIXEL_NINE_RING_NUM + 224)

#define PIXEL_EIGHT_RING_NUM  15
#define EIGHT_RING_NUM (24*PIXEL_EIGHT_RING_NUM + 224)

#define PIXEL_SEVEN_RING_NUM  20
#define SEVEN_RING_NUM (24*PIXEL_SEVEN_RING_NUM + 224)

#define PIXEL_SIX_RING_NUM  25
#define SIX_RING_NUM (24*PIXEL_SIX_RING_NUM + 224)

#define PIXEL_FIVE_RING_NUM  30
#define FIVE_RING_NUM (24*PIXEL_FIVE_RING_NUM + 224)

#define PIXEL_FOUR_RING_NUM  35
#define FOUR_RING_NUM (24*PIXEL_FOUR_RING_NUM + 224)

#define PIXEL_THREE_RING_NUM  40
#define THREE_RING_NUM (24*PIXEL_THREE_RING_NUM + 224)

#define PIXEL_TWO_RING_NUM  45
#define TWO_RING_NUM (24*PIXEL_TWO_RING_NUM + 224)

#define PIXEL_ONE_RING_NUM  50
#define ONE_RING_NUM (24*PIXEL_ONE_RING_NUM + 224)

#define WS1 30  //随着自动重装载值而变化 2:1
#define WS0 15

void WS_Load_rectangle(void);
void WS_Load_frame_up(void);
void WS_Load_frame_down(void);
void WS_Load_Cross(void);
void WS_Load_Frame(void);

void WS_WriteAll_RGB_FRAME_UP(uint8_t n_R, uint8_t n_G, uint8_t n_B);
void WS_WriteAll_RGB_FRAME_DOWN(uint8_t n_R, uint8_t n_G, uint8_t n_B );
void WS_WriteAll_RGB_REC(uint8_t n_R, uint8_t n_G, uint8_t n_B );
void WS_WriteAll_RGB_RECT(uint8_t n_R, uint8_t n_G, uint8_t n_B );
void WS_Write_RGB_ALL_RING(uint8_t n_R, uint8_t n_G, uint8_t n_B );
void WS_Write_Cross(uint8_t n_R, uint8_t n_G, uint8_t n_B );
void WS_WriteAll_RGB_FRAME(uint8_t n_R, uint8_t n_G, uint8_t n_B );

void WS_CloseAll_FRAME_UP(void);
void WS_CloseAll_RECTANGLE(void);
void WS_CloseAll_FRAME_DOWN(void);
void WS_CloseAll_Circle(void);
void WS_CloseAll_Frame(void);

uint32_t WS281x_Color(uint8_t red, uint8_t green, uint8_t blue);
void WS281x_SetPixelColor(uint16_t n, uint32_t GRBColor);
void WS281x_SetPixelRGB_CIR(uint16_t n ,uint8_t red, uint8_t green, uint8_t blue);
void WS281x_SetPixelRGB_FRAME_UP(uint16_t n, uint8_t red, uint8_t green, uint8_t blue);
void WS281x_SetPixelRGB_FRAME_DOWN(uint16_t n, uint8_t red, uint8_t green, uint8_t blue);
void WS281x_SetPixelRGB_FRAME(uint16_t n, uint8_t red, uint8_t green, uint8_t blue);
void WS281x_SetPixelColor_RECT(uint16_t n, uint8_t red, uint8_t green, uint8_t blue);

uint32_t Wheel(uint8_t WheelPos);
void LED_Blue_rectangle_current(uint16_t wait_time ,uint8_t n_R, uint8_t n_G, uint8_t n_B );
void LED_Red_rectangle_current(uint16_t wait_time ,uint8_t n_R, uint8_t n_G, uint8_t n_B );
void LED_Blue_rectangle_current_ALL_IN(uint8_t wait_time);
void LED_Red_rectangle_current_ALL_IN(uint8_t wait_time);
	
void LED_Police_rectangle_current(uint8_t wait_time);
void LED_Blue_frame_up_current(uint8_t wait_time);
void LED_Red_frame_up_current(uint8_t wait_time);

/*********************靶环点亮部分***************************/
void WS_Write_RGB_TEN_RING(uint8_t n_R, uint8_t n_G, uint8_t n_B );
void WS_Write_RGB_NINE_RING(uint8_t n_R, uint8_t n_G, uint8_t n_B );
void WS_Write_RGB_EIGHT_RING(uint8_t n_R, uint8_t n_G, uint8_t n_B );
void WS_Write_RGB_SEVEN_RING(uint8_t n_R, uint8_t n_G, uint8_t n_B );
void WS_Write_RGB_SIX_RING(uint8_t n_R, uint8_t n_G, uint8_t n_B );
void WS_Write_RGB_FIVE_RING(uint8_t n_R, uint8_t n_G, uint8_t n_B );
void WS_Write_RGB_FOUR_RING(uint8_t n_R, uint8_t n_G, uint8_t n_B );
void WS_Write_RGB_THREE_RING(uint8_t n_R, uint8_t n_G, uint8_t n_B );
void WS_Write_RGB_TWO_RING(uint8_t n_R, uint8_t n_G, uint8_t n_B );
void WS_Write_RGB_ONE_RING(uint8_t n_R, uint8_t n_G, uint8_t n_B );
/*********************靶环点亮部分***************************/

void WS_WriteAll_RGB_FRAME_Delay(uint8_t n_R, uint8_t n_G, uint8_t n_B );
void WS_WriteAll_RGB_FRAME_UP_Delay(uint8_t n_R, uint8_t n_G, uint8_t n_B );
void WS_WriteAll_RGB_RECT_Delay(uint8_t n_R, uint8_t n_G, uint8_t n_B );
void WS_WriteAll_RGB_FRAME_DOWN_Delay(uint8_t n_R, uint8_t n_G, uint8_t n_B );
void WS_Write_RGB_ALL_Delay(uint8_t n_R, uint8_t n_G, uint8_t n_B );
void Rainbow_Current(uint8_t n_R, uint8_t n_G, uint8_t n_B );
void rainbowCycle(uint8_t wait);


void LED_rectangle_current_REC(uint16_t wait_time ,uint8_t n_R, uint8_t n_G, uint8_t n_B );

#endif

