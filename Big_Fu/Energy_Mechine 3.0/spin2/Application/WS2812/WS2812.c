#include "WS2812.h"
extern DMA_HandleTypeDef hdma_tim1_ch1;
extern DMA_HandleTypeDef hdma_tim1_ch2;
extern DMA_HandleTypeDef hdma_tim2_ch1;
extern DMA_HandleTypeDef hdma_tim2_ch2_ch4;
extern DMA_HandleTypeDef hdma_tim3_ch1_trig;
extern DMA_HandleTypeDef hdma_tim4_ch1;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern int16_t flag_color;

int16_t send_Buf_rectangle[NUM_REC];
int16_t send_Buf_move_rectangle = 0;
int16_t cnt_time_rectangle = 0;
int16_t Led_bite_cnt_rectangle=0;

int16_t send_Buf_frame_up[NUM_FRAME_UP] ;
int16_t send_Buf_move_frame_up = 0;
int16_t cnt_time_frame_up = 0;
int16_t Led_bite_cnt_frame_up=0;

int16_t send_Buf_frame_down[NUM_FRAME_DOWN] ;
int16_t send_Buf_move_frame_down = 0;
int16_t cnt_time_frame_down = 0;
int16_t Led_bite_cnt_frame_down=0;

int16_t send_Buf_Circle[NUM_CIRCLE] ;
int16_t send_Buf_move_Circle = 0;
int16_t cnt_time_Circle = 0;
int16_t Led_bite_cnt_Circle=0;

int16_t send_Buf_frame[NUM_FRAME] ;
int16_t send_Buf_move_frame = 0;
int16_t cnt_time_frame = 0;
int16_t Led_bite_cnt_frame=0;

int16_t test1,test2;

/*
   DMA PWM 
   开启函数
*/

void WS_Load_rectangle(void)  //PA8/////////灯带
{

	HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_2, (uint32_t *)send_Buf_rectangle, NUM_REC);
  
}
void WS_Load_frame_up(void)//PC6
{
	
	HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_1, (uint32_t *)send_Buf_frame_up, NUM_FRAME_UP);
	
}
	
void WS_Load_frame_down(void)//PA1
{
	
	HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_2, (uint32_t *)send_Buf_frame_down, NUM_FRAME_DOWN);
}

void WS_Load_Circle(void)//PA0
{
	HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_1, (uint32_t *)send_Buf_Circle, NUM_CIRCLE);
}

void WS_Load_Frame(void)//PA9           //点亮灯条
{
	HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_1, (uint32_t *)send_Buf_frame, NUM_FRAME);
}

/*DMA接收中断回调函数*/
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{

}

/********************	 LED关闭函数   ***********************/


/*
	 关闭PCB上边框函数
*/
void WS_CloseAll_FRAME_UP(void)
{
	uint16_t i;
	for (i = 0; i < PIXEL_NUM_FRAME_UP * 24; i++)
		send_Buf_frame_up[i] = WS0; // 写入逻辑0的占空比
	for (i = PIXEL_NUM_FRAME_UP * 24; i < NUM_FRAME_UP; i++)
		send_Buf_frame_up[i] = 0; // 占空比比为0，全为低电平
	WS_Load_frame_up();
}
/*
	 关闭PCB下边框函数
*/
void WS_CloseAll_FRAME_DOWN(void)
{
	uint16_t i;
	for (i = 0; i < (PIXEL_NUM_FRAME_DOWN) * 24; i++)
		send_Buf_frame_down[i] = WS0; // 写入逻辑0的占空比
	for (i = PIXEL_NUM_FRAME_DOWN * 24; i < NUM_FRAME_DOWN; i++)
		send_Buf_frame_down[i] = 0; // 占空比比为0，全为低电平
		
	WS_Load_frame_down();
}
/*
	 关闭箭头灯条函数
*/
void WS_CloseAll_RECTANGLE(void)
{
	uint16_t i;
	for (i = 0; i < (PIXEL_NUM_REC) * 24; i++)
		send_Buf_rectangle[i] = WS0; // 写入逻辑0的占空比
	for (i = PIXEL_NUM_REC * 24; i < NUM_REC; i++)
		send_Buf_rectangle[i] = 0; // 占空比比为0，全为低电平
	
	WS_Load_rectangle();
	
}

/*
	 关闭PCB靶环函数
*/
void WS_CloseAll_Circle(void)
{
	uint16_t i;
	for (i = 0; i < (PIXEL_NUM_CIRCLE) * 24; i++)
		send_Buf_Circle[i] = WS0; // 写入逻辑0的占空比
	for (i = PIXEL_NUM_CIRCLE * 24; i < NUM_CIRCLE; i++)
		send_Buf_Circle[i] = 0; // 占空比比为0，全为低电平
	
	WS_Load_Circle();
}

/*
	 关闭PCB靶环函数
*/
void WS_CloseAll_Frame(void)
{
	uint16_t i;
	for (i = 0; i < (PIXEL_NUM_FRAME) * 24; i++)
		send_Buf_frame[i] = WS0; // 写入逻辑0的占空比
	for (i = PIXEL_NUM_FRAME * 24; i < NUM_FRAME; i++)
		send_Buf_frame[i] = 0; // 占空比比为0，全为低电平
	
	WS_Load_Frame();
}


/*
	 箭头流水灯设计函数
*/


void arrow_LED_cut_down_rectangle(int16_t add,int16_t send_Buf_move_rectangle,uint8_t red, uint8_t green, uint8_t blue)
{
	for ( Led_bite_cnt_rectangle = (3+add); Led_bite_cnt_rectangle < (5+add);Led_bite_cnt_rectangle++)
		{
			uint8_t i;
			if (Led_bite_cnt_rectangle < (5+add))
			{for (i = 0; i < 24; ++i)
		   {
			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
			 }
			}
		 }
			
			for ( Led_bite_cnt_rectangle = (10+add); Led_bite_cnt_rectangle < (14+add);Led_bite_cnt_rectangle++)
		{
			uint8_t i;
			if (Led_bite_cnt_rectangle < (14+add))
			{for (i = 0; i < 24; ++i)
		   {
			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
			 }
			}
		 }
		
			for ( Led_bite_cnt_rectangle = (17+add); Led_bite_cnt_rectangle < (23+add);Led_bite_cnt_rectangle++)
		{
			uint8_t i;
			if (Led_bite_cnt_rectangle < (23+add))
			{for (i = 0; i < 24; ++i)
		   {
			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
			 }
			}
		 }
		
					for ( Led_bite_cnt_rectangle = (24+add); Led_bite_cnt_rectangle < (27+add);Led_bite_cnt_rectangle++)
		{
			uint8_t i;
			if (Led_bite_cnt_rectangle < (27+add))
			{for (i = 0; i < 24; ++i)
		   {
			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
			 }
			}
		 }
	
			for ( Led_bite_cnt_rectangle = (29+add); Led_bite_cnt_rectangle < (34+add);Led_bite_cnt_rectangle++)
		{
			uint8_t i;
			if (Led_bite_cnt_rectangle < (34+add))
			{for (i = 0; i < 24; ++i)
		   {
			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
			 }
			}
		 }
		
		for ( Led_bite_cnt_rectangle = (38+add); Led_bite_cnt_rectangle < (40+add);Led_bite_cnt_rectangle++)
		{
			uint8_t i;
			if (Led_bite_cnt_rectangle < (40+add))
			{for (i = 0; i < 24; ++i)
		   {
			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
			 }
			}
		 }
}
/*
	 点亮边框灯条函数
*/
void WS_WriteAll_RGB_FRAME(uint8_t n_R, uint8_t n_G, uint8_t n_B )
{
	uint16_t n = 0;
	
	//PIXEL_NUM_FRAME
	//flag_color
	for(n =0 ; n <PIXEL_NUM_FRAME; n++)
	{
		 WS281x_SetPixelRGB_FRAME(n,n_R, n_G, n_B);
	}
	 WS_Load_Frame();
}

/*
	 点亮PCB上边框函数
*/
void WS_WriteAll_RGB_FRAME_UP(uint8_t n_R, uint8_t n_G, uint8_t n_B )
{
	uint16_t n = 0;
	
	for(n =  0 ; n <PIXEL_NUM_FRAME_UP; n++)
	{
		 WS281x_SetPixelRGB_FRAME_UP(n,n_R, n_G, n_B);
	}
	
	WS_Load_frame_up();
}

/*
	 点亮中间灯带
*/
void WS_WriteAll_RGB_RECT(uint8_t n_R, uint8_t n_G, uint8_t n_B )
{
	uint16_t n = 0;
	
	for(n =0 ; n <PIXEL_NUM_REC; n++)
	{
		 WS281x_SetPixelColor_RECT(n,n_R, n_G, n_B);
	}
	
	WS_Load_rectangle();
}
/*
	 点亮PCB下边框函数
*/
void WS_WriteAll_RGB_FRAME_DOWN(uint8_t n_R, uint8_t n_G, uint8_t n_B )
{
   uint16_t n = 0;
	
	for(n =  0 ; n <PIXEL_NUM_FRAME_DOWN; n++)
	{
		 WS281x_SetPixelRGB_FRAME_DOWN(n,n_R, n_G, n_B);//移位操作
	}
	
	WS_Load_frame_down();//DMA发送///im2, TIM_CHANNEL_2/PA1
}
/*
	 点亮箭头灯带函数
*/

void WS_WriteAll_RGB_REC(uint8_t n_R, uint8_t n_G, uint8_t n_B )
{
	uint16_t bit_24_low,bit_24_mid,bit_24_hig,i,j;
	uint8_t dat[24];
	// 将RGB数据进行转换
	for (bit_24_low = 0; bit_24_low < 8; bit_24_low++)
	{
		dat[bit_24_low] = ((n_G & 0x80) ? WS1 : WS0);
		n_G <<= 1;
	}
	for (bit_24_mid = 0; bit_24_mid < 8; bit_24_mid++)
	{
		dat[bit_24_mid + 8] = ((n_R & 0x80) ? WS1 : WS0);
		n_R <<= 1;
	}
	for (bit_24_hig = 0; bit_24_hig < 8; bit_24_hig++)
	{
		dat[bit_24_hig + 16] = ((n_B & 0x80) ? WS1 : WS0);
		n_B <<= 1;
	}
	for (i = 0; i < PIXEL_NUM_REC; i++)
	{
		for (j = 0; j < 24; j++)
		{
			send_Buf_rectangle[i * 24 + j] = dat[j];
		}
	}
	for(i = PIXEL_NUM_REC* 24; i < NUM_REC; i++)
		send_Buf_rectangle[i] = 0; // 占空比比为0，全为低电平
	WS_Load_rectangle();
}
//void WS_WriteAll_RGB_REC(uint8_t n_R, uint8_t n_G, uint8_t n_B )
//{
//  uint8_t red=n_R;
//  uint8_t green=n_G;
//  uint8_t blue=n_B;
//	for ( Led_bite_cnt_rectangle = (0); Led_bite_cnt_rectangle < (256);Led_bite_cnt_rectangle++)
//		{
//			uint8_t i;
//			if (Led_bite_cnt_rectangle < 256)
//			{for (i = 0; i < 24; ++i)
//		   {
//			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue)<< i) & 0X800000) ? WS1 : WS0);
//			 }
//			}
//		 }
	
//////////////////////////////////////////////R镖
//			 for ( Led_bite_cnt_rectangle = (256); Led_bite_cnt_rectangle < (263);Led_bite_cnt_rectangle++)
//		{
//			uint8_t i;
//			if (Led_bite_cnt_rectangle < 263)
//			{for (i = 0; i < 24; ++i)
//		   {
//			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue)<< i) & 0X800000) ? WS1 : WS0);
//			 }
//			}
//		 }
//		
//			 for ( Led_bite_cnt_rectangle = (264); Led_bite_cnt_rectangle < (266);Led_bite_cnt_rectangle++)
//		{
//			uint8_t i;
//			if (Led_bite_cnt_rectangle < 266)
//			{for (i = 0; i < 24; ++i)
//		   {
//			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue)<< i) & 0X800000) ? WS1 : WS0);
//			 }
//			}
//		 }
//		 
//		//
//		 	 for ( Led_bite_cnt_rectangle = (271); Led_bite_cnt_rectangle < (274);Led_bite_cnt_rectangle++)
//		{
//			uint8_t i;
//			if (Led_bite_cnt_rectangle < 274)
//			{for (i = 0; i < 24; ++i)
//		   {
//			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
//			 }
//			}
//		 }
//		
//		  for ( Led_bite_cnt_rectangle = (279); Led_bite_cnt_rectangle < (287);Led_bite_cnt_rectangle++)
//		{
//			uint8_t i;
//			if (Led_bite_cnt_rectangle < 287)
//			{for (i = 0; i < 24; ++i)
//		   {
//			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
//			 }
//			}
//		 }
//		
// for ( Led_bite_cnt_rectangle = (288); Led_bite_cnt_rectangle < (293);Led_bite_cnt_rectangle++)
//		{
//			uint8_t i;
//			if (Led_bite_cnt_rectangle < 293)
//			{for (i = 0; i < 24; ++i)
//		   {
//			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
//			 }
//			}
//		 }
//		
//	 for ( Led_bite_cnt_rectangle = (296); Led_bite_cnt_rectangle < (298);Led_bite_cnt_rectangle++)
//		{
//			uint8_t i;
//			if (Led_bite_cnt_rectangle < 298)
//			{for (i = 0; i < 24; ++i)
//		   {
//			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
//			 }
//			}
//		 }	 
//		
// for ( Led_bite_cnt_rectangle = (299); Led_bite_cnt_rectangle < (301);Led_bite_cnt_rectangle++)
//		{
//			uint8_t i;
//			if (Led_bite_cnt_rectangle < 301)
//			{for (i = 0; i < 24; ++i)
//		   {
//			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
//			 }
//			}
//		 }			
//for ( Led_bite_cnt_rectangle = (304); Led_bite_cnt_rectangle < (306);Led_bite_cnt_rectangle++)
//		{
//			uint8_t i;
//			if (Led_bite_cnt_rectangle < 306)
//			{for (i = 0; i < 24; ++i)
//		   {
//			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
//			 }
//			}
//		 }		
//for ( Led_bite_cnt_rectangle = (308); Led_bite_cnt_rectangle < (310);Led_bite_cnt_rectangle++)
//		{
//			uint8_t i;
//			if (Led_bite_cnt_rectangle < 310)
//			{for (i = 0; i < 24; ++i)
//		   {
//			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
//			 }
//			}
//		 }	
//	for ( Led_bite_cnt_rectangle = (312); Led_bite_cnt_rectangle < (314);Led_bite_cnt_rectangle++)
//		{
//			uint8_t i;
//			if (Led_bite_cnt_rectangle < 314)
//			{for (i = 0; i < 24; ++i)
//		   {
//			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
//			 }
//			}
//		 }	
//		for ( Led_bite_cnt_rectangle = (317); Led_bite_cnt_rectangle < (319);Led_bite_cnt_rectangle++)
//		{
//			uint8_t i;
//			if (Led_bite_cnt_rectangle < 319)
//			{for (i = 0; i < 24; ++i)
//		   {
//			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
//			 }
//			}
//		 }	
//	WS_Load_rectangle();
//}
/*
	 点亮PCB靶环十字函数
*/
void WS_Write_Cross(uint8_t n_R, uint8_t n_G, uint8_t n_B )//十字效果
{
	for(int i = 0; i < 1; i++)
		WS281x_SetPixelRGB_CIR(i,n_R, n_G, n_B);
  for(int i = 12; i < 13; i++)
		WS281x_SetPixelRGB_CIR(i,n_R, n_G, n_B);
	for(int i = 25; i < 26; i++)
		WS281x_SetPixelRGB_CIR(i,n_R, n_G, n_B);
	for(int i = 38; i < 39; i++)
		WS281x_SetPixelRGB_CIR(i,n_R, n_G, n_B);
	for(int i = 50; i < 51; i++)
		WS281x_SetPixelRGB_CIR(i,n_R, n_G, n_B);
	for(int i = 61; i < 62; i++)
		WS281x_SetPixelRGB_CIR(i,n_R, n_G, n_B);
	for(int i = 72; i < 73; i++)
		WS281x_SetPixelRGB_CIR(i,n_R, n_G, n_B);
	for(int i = 84; i < 85; i++)
		WS281x_SetPixelRGB_CIR(i,n_R, n_G, n_B);
	for(int i = 95; i < 96; i++)
		WS281x_SetPixelRGB_CIR(i,n_R, n_G, n_B);
	for(int i = 105; i < 106; i++)
		WS281x_SetPixelRGB_CIR(i,n_R, n_G, n_B);
	for(int i = 115; i < 116; i++)
		WS281x_SetPixelRGB_CIR(i,n_R, n_G, n_B);
	for(int i = 125; i < 126; i++)
		WS281x_SetPixelRGB_CIR(i,n_R, n_G, n_B);
	for(int i = 135; i < 136; i++)
		WS281x_SetPixelRGB_CIR(i,n_R, n_G, n_B);
	for(int i = 144; i < 145; i++)
		WS281x_SetPixelRGB_CIR(i,n_R, n_G, n_B);
	for(int i = 152; i < 153; i++)
		WS281x_SetPixelRGB_CIR(i,n_R, n_G, n_B);
	for(int i = 161; i < 162; i++)
		WS281x_SetPixelRGB_CIR(i,n_R, n_G, n_B);
	for(int i = 170; i < 171; i++)
		WS281x_SetPixelRGB_CIR(i,n_R, n_G, n_B);
	for(int i = 178; i < 179; i++)
		WS281x_SetPixelRGB_CIR(i,n_R, n_G, n_B);
	for(int i = 185; i < 186; i++)
		WS281x_SetPixelRGB_CIR(i,n_R, n_G, n_B);
	for(int i = 230; i < 231; i++)
		WS281x_SetPixelRGB_CIR(i,n_R, n_G, n_B);
	for(int i = 225; i < 226; i++)
		WS281x_SetPixelRGB_CIR(i,n_R, n_G, n_B);
  for(int i = 240; i < 241; i++)
		WS281x_SetPixelRGB_CIR(i,n_R, n_G, n_B);
	for(int i = 192; i < 193; i++)
		WS281x_SetPixelRGB_CIR(i,n_R, n_G, n_B);
	for(int i = 235; i < 236; i++)
		WS281x_SetPixelRGB_CIR(i,n_R, n_G, n_B);
	WS_Load_Circle();
}

/*********************靶环点亮部分***************************/

/*
	 全部点亮
*/
void WS_Write_RGB_ALL_RING(uint8_t n_R, uint8_t n_G, uint8_t n_B )
{
  uint16_t n = 0;	
	for(n = 0 ; n < 272; n++)
	{
		 WS281x_SetPixelRGB_CIR(n,n_R, n_G, n_B);
	}
	
	WS_Load_Circle();
}
/*
	 十环
*/
void WS_Write_RGB_TEN_RING(uint8_t n_R, uint8_t n_G, uint8_t n_B )
{
  uint16_t n = 0;
	
	for(n = 268 ; n < 272; n++)
	{
		 WS281x_SetPixelRGB_CIR(n,n_R, n_G, n_B);
	}
	
	WS_Load_Circle();
}
/*
	 九环
*/
void WS_Write_RGB_NINE_RING(uint8_t n_R, uint8_t n_G, uint8_t n_B )
{
  uint16_t n = 0;
	
	for(n = 260 ; n < 268; n++)
	{
		 WS281x_SetPixelRGB_CIR(n,n_R, n_G, n_B);
	}
	
	WS_Load_Circle();
}
/*
	 八环
*/
void WS_Write_RGB_EIGHT_RING(uint8_t n_R, uint8_t n_G, uint8_t n_B )
{
  uint16_t n = 0;
	
	for(n = 245 ; n < 260; n++)
	{
		 WS281x_SetPixelRGB_CIR(n,n_R, n_G, n_B);
	}
	
	WS_Load_Circle();
}
/*
	 七环
*/
void WS_Write_RGB_SEVEN_RING(uint8_t n_R, uint8_t n_G, uint8_t n_B )
{
  uint16_t n = 0;
	
	for(n = 225 ; n < 245; n++)
	{
		 WS281x_SetPixelRGB_CIR(n,n_R, n_G, n_B);
	}
	
	WS_Load_Circle();
}
/*
	 六环
*/
void WS_Write_RGB_SIX_RING(uint8_t n_R, uint8_t n_G, uint8_t n_B )
{
  uint16_t n = 0;
	
	for(n =  200 ; n < 225; n++)
	{
		 WS281x_SetPixelRGB_CIR(n,n_R, n_G, n_B);
	}
	
	WS_Load_Circle();
}
/*
	 五环
*/
void WS_Write_RGB_FIVE_RING(uint8_t n_R, uint8_t n_G, uint8_t n_B )
{
  uint16_t n = 0;
	
	for(n = 170 ; n < 200; n++)
	{
		 WS281x_SetPixelRGB_CIR(n,n_R, n_G, n_B);
	}
	
	WS_Load_Circle();
}
/*
	 四环
*/
void WS_Write_RGB_FOUR_RING(uint8_t n_R, uint8_t n_G, uint8_t n_B )
{
  uint16_t n = 0;
	
	for(n = 135 ; n < 170; n++)
	{
		 WS281x_SetPixelRGB_CIR(n,n_R, n_G, n_B);
	}
	
	WS_Load_Circle();
}
/*
	 三环
*/
void WS_Write_RGB_THREE_RING(uint8_t n_R, uint8_t n_G, uint8_t n_B )
{
  uint16_t n = 0;
	
	for(n = 95 ; n < 135; n++)
	{
		 WS281x_SetPixelRGB_CIR(n,n_R, n_G, n_B);
	}
	
	WS_Load_Circle();
}
/*
	 二环
*/
void WS_Write_RGB_TWO_RING(uint8_t n_R, uint8_t n_G, uint8_t n_B )
{
  uint16_t n = 0;
	
	for(n = 50 ; n < 95; n++)
	{
		 WS281x_SetPixelRGB_CIR(n,n_R, n_G, n_B);
	}
	
	WS_Load_Circle();
}
/*
	 一环
*/
void WS_Write_RGB_ONE_RING(uint8_t n_R, uint8_t n_G, uint8_t n_B )
{
  uint16_t n = 0;
	
	for(n =  0 ; n < 50; n++)
	{
		 WS281x_SetPixelRGB_CIR(n,n_R, n_G, n_B);		
	}
	
	WS_Load_Circle();
}
/*********************靶环点亮部分***************************/
/* 
	<< 移位运算符：返回绿红蓝顺序的32bit数据,或运算符
	绿红蓝的数据顺序是WS2812通信协议规定的，不是乱设的！
*/
uint32_t WS281x_Color(uint8_t red, uint8_t green, uint8_t blue)
{
	return green << 16 | red << 8 | blue;
}

void WS281x_SetPixelColor(uint16_t n, uint32_t GRBColor)
{

	uint8_t i;
	if (n < PIXEL_NUM_CIRCLE)
	{
		for (i = 0; i < 24; ++i)
			send_Buf_Circle[24 * n + i] = (((GRBColor << i) & 0X800000) ? WS1 : WS0);
	}
}


void WS281x_SetPixelColor_RECT(uint16_t n, uint8_t red, uint8_t green, uint8_t blue)
{
	uint8_t i;
	if (n < PIXEL_NUM_REC)
	{
    for (i = 0; i < 24; ++i)
		{
			send_Buf_rectangle[24 * n + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
		}
	}
}

/*
	 点亮PCB上边框base函数
*/
void WS281x_SetPixelRGB_FRAME_UP(uint16_t n, uint8_t red, uint8_t green, uint8_t blue)
{
	uint8_t i;

	if (n < PIXEL_NUM_FRAME_UP) 
	{
		for (i = 0; i < 24; ++i)
		{
			send_Buf_frame_up[24 * n + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
		}
	}
}
/*
	 点亮灯条边框base函数
*/
void WS281x_SetPixelRGB_FRAME(uint16_t n, uint8_t red, uint8_t green, uint8_t blue)
{
	uint8_t i;

	if (n < PIXEL_NUM_FRAME) 
	{
		for (i = 0; i < 24; ++i)
		{
			send_Buf_frame[24 * n + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
		}
	}
}
/*
	 点亮PCB靶环base函数
*/
void WS281x_SetPixelRGB_CIR(uint16_t n, uint8_t red, uint8_t green, uint8_t blue)
{
	uint8_t i;

	if (n < PIXEL_NUM_CIRCLE)
	{
		for (i = 0; i < 24; ++i)
		{
				send_Buf_Circle[24 * n + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
		}
	}
	
}
/*
	 点亮PCB下边框base函数
*/
void WS281x_SetPixelRGB_FRAME_DOWN(uint16_t n, uint8_t red, uint8_t green, uint8_t blue)
{
	uint8_t i;

	if (n < PIXEL_NUM_FRAME_DOWN)
	{
		for (i = 0; i < 24; ++i)
		{
			send_Buf_frame_down[24 * n + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
		}
	}
}
			//中间灯带全亮
void LED_rectangle_current_REC_2(uint16_t wait_time ,uint8_t n_R, uint8_t n_G, uint8_t n_B )
{
  uint8_t red=n_R;
  uint8_t green=n_G;
  uint8_t blue=n_B;
	
				uint16_t light_num = 256;
        
				// 2. 遍历这 256 个灯珠 后到前
        for (Led_bite_cnt_rectangle = 0; Led_bite_cnt_rectangle < 256; Led_bite_cnt_rectangle++)
        {
            uint8_t i;
            
            // 【核心修改】：当序号 >= (256 - 需要点亮的数量) 时，代表到了尾部，点亮它们
            if (Led_bite_cnt_rectangle >= (256 - light_num))
            {
                // 【点亮区域】：尾部的灯珠，赋给指定的 RGB 颜色
                for (i = 0; i < 24; ++i)
                {
                    send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
                }
            }
            else
            {
                // 【熄灭区域】：头部前面的灯珠，赋给黑色 (0, 0, 0)
                for (i = 0; i < 24; ++i)
                {
                    send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(0, 0, 0) << i) & 0X800000) ? WS1 : WS0);
                }
            }
        }
			}
//熄灭
void LED_rectangle_current_REC_XIMIE(uint16_t wait_time ,uint8_t n_R, uint8_t n_G, uint8_t n_B )
{
  uint8_t red=n_R;
  uint8_t green=n_G;
  uint8_t blue=n_B;
	
				
				uint16_t light_num = 0;
        uint8_t current_stage = Board_Order_info[BOARD_NUMBER].count_t; // 读取主控发来的进度
        
        if (current_stage == 5 || current_stage == 0 ) 
        {
            light_num = 256; // 满进度全亮
            //WS_WriteAll_RGB_FRAME(RED_NUM,0,0);
        }
        else if (current_stage == 1)
        {
            light_num = 56; 
            //WS_WriteAll_RGB_FRAME_JINDU(RED_NUM,0,0,17,39);
        }
        else if (current_stage == 2)
        {
            light_num = 104; 
            //WS_WriteAll_RGB_FRAME_JINDU(RED_NUM,0,0,12,44);
        }
        else if (current_stage == 3)
        {
            light_num = 152; 
            //WS_WriteAll_RGB_FRAME_JINDU(RED_NUM,0,0,9,49);
        }
        else if (current_stage == 4)
        {
            light_num = 200; 
            //WS_WriteAll_RGB_FRAME_JINDU(RED_NUM,0,0,5,51);
        }	
				// 2. 遍历这 256 个灯珠 后到前
        for (Led_bite_cnt_rectangle = 0; Led_bite_cnt_rectangle < 256; Led_bite_cnt_rectangle++)
        {
            uint8_t i;
            
            // 【核心修改】：当序号 >= (256 - 需要点亮的数量) 时，代表到了尾部，点亮它们
            if (Led_bite_cnt_rectangle >= (256 - light_num))
            {
                // 【点亮区域】：尾部的灯珠，赋给指定的 RGB 颜色
                for (i = 0; i < 24; ++i)
                {
                    send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(0, 0, 0) << i) & 0X800000) ? WS1 : WS0);
                }
            }
            else
            {
                // 【熄灭区域】：头部前面的灯珠，赋给黑色 (0, 0, 0)
                for (i = 0; i < 24; ++i)
                {
                    send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(0, 0, 0) << i) & 0X800000) ? WS1 : WS0);
                }
            }
        }
			}
/*
	 点亮
*/
void LED_rectangle_current_REC(uint16_t wait_time ,uint8_t n_R, uint8_t n_G, uint8_t n_B )
{
  uint8_t red=n_R;
  uint8_t green=n_G;
  uint8_t blue=n_B;
	
//		for ( Led_bite_cnt_rectangle = (0); Led_bite_cnt_rectangle < (256);Led_bite_cnt_rectangle++)
//		{
//			uint8_t i;
//			if (Led_bite_cnt_rectangle < 256)
//			{for (i = 0; i < 24; ++i)
//		   {
//			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
//			 }
//			}
//		 }
		
		 // 1. 根据 count_t 计算需要点亮的灯珠数量 (256等分5份，每份约51个)
//        uint16_t light_num = 0;
//				uint8_t current_stage = Board_Order_info[BOARD_NUMBER].count_t; // 读取主控发来的进度
//        if (current_stage == 5 || current_stage==0) 
//        {
//            light_num = 256; // 满进度全亮
//					  WS_WriteAll_RGB_FRAME(RED_NUM,0,0);
//        }
//        else if (current_stage ==1)
//        {
//            light_num = 56; // 自动计算 1/5, 2/5... 的数量
//					  WS_WriteAll_RGB_FRAME_JINDU(RED_NUM,0,0,17,39);
//        }
//				else if (current_stage ==2)
//				{
//            light_num = 104; // 自动计算 1/5, 2/5... 的数量
//					  WS_WriteAll_RGB_FRAME_JINDU(RED_NUM,0,0,12,44);
//        }
//				else if (current_stage ==3)
//				{
//            light_num = 152; // 自动计算 1/5, 2/5... 的数量
//						WS_WriteAll_RGB_FRAME_JINDU(RED_NUM,0,0,9,49);
//        }
//				else if (current_stage ==4)
//				{
//            light_num = 200; // 自动计算 1/5, 2/5... 的数量
//						WS_WriteAll_RGB_FRAME_JINDU(RED_NUM,0,0,5,51);
//        }
//        else if(current_stage ==6)
//        {
//            light_num = 0;   // count_t 为 0 时，灯带全灭
//						WS_WriteAll_RGB_FRAME_JINDU(RED_NUM,0,0,5,51);
//        }
				
				uint16_t light_num = 0;
        uint8_t current_stage = Board_Order_info[BOARD_NUMBER].count_t; // 读取主控发来的进度
        
        if (current_stage == 5 || current_stage == 0 ) 
        {
            light_num = 256; // 满进度全亮
            WS_WriteAll_RGB_FRAME(RED_NUM,0,0);
        }
        else if (current_stage == 1)
        {
            light_num = 56; 
            WS_WriteAll_RGB_FRAME_JINDU(RED_NUM,0,0,17,39);
        }
        else if (current_stage == 2)
        {
            light_num = 104; 
            WS_WriteAll_RGB_FRAME_JINDU(RED_NUM,0,0,12,44);
        }
        else if (current_stage == 3)
        {
            light_num = 152; 
            WS_WriteAll_RGB_FRAME_JINDU(RED_NUM,0,0,9,49);
        }
        else if (current_stage == 4)
        {
            light_num = 200; 
            WS_WriteAll_RGB_FRAME_JINDU(RED_NUM,0,0,5,51);
        }
//        else if (current_stage == 0) // <=== 修复点：0进度时应该全灭！
//        {
//            light_num = 0;   
//            WS_WriteAll_RGB_FRAME(0,0,0); // 同步把边框也熄灭
//        }

        // 2. 遍历这 256 个灯珠 前-后
//        for (Led_bite_cnt_rectangle = 0; Led_bite_cnt_rectangle < 256; Led_bite_cnt_rectangle++)
//        {
//            uint8_t i;
//            if (Led_bite_cnt_rectangle < light_num)
//            {
//                // 【点亮区域】：在进度比例以内的灯珠，赋给指定的 RGB 颜色
//                for (i = 0; i < 24; ++i)
//                {
//                    send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
//                }
//            }
//            else
//            {
//                // 【熄灭区域】：超出进度比例的剩余灯珠，赋给黑色 (0, 0, 0)
//                for (i = 0; i < 24; ++i)
//                {
//                    send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(0, 0, 0) << i) & 0X800000) ? WS1 : WS0);
//                }
//            }
//        }
				
				// 2. 遍历这 256 个灯珠 后到前
        for (Led_bite_cnt_rectangle = 0; Led_bite_cnt_rectangle < 256; Led_bite_cnt_rectangle++)
        {
            uint8_t i;
            
            // 【核心修改】：当序号 >= (256 - 需要点亮的数量) 时，代表到了尾部，点亮它们
            if (Led_bite_cnt_rectangle >= (256 - light_num))
            {
                // 【点亮区域】：尾部的灯珠，赋给指定的 RGB 颜色
                for (i = 0; i < 24; ++i)
                {
                    send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
                }
            }
            else
            {
                // 【熄灭区域】：头部前面的灯珠，赋给黑色 (0, 0, 0)
                for (i = 0; i < 24; ++i)
                {
                    send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(0, 0, 0) << i) & 0X800000) ? WS1 : WS0);
                }
            }
        }
//				if(Board_Order_info[BOARD_NUMBER].count_t == 5)
//				{
//					for (Led_bite_cnt_rectangle = 0; Led_bite_cnt_rectangle < 256; Led_bite_cnt_rectangle++)
//        {
//            uint8_t i;
//            
//            // 【核心修改】：当序号 >= (256 - 需要点亮的数量) 时，代表到了尾部，点亮它们
//            if (Led_bite_cnt_rectangle >= (256 - light_num))
//            {
//                // 【点亮区域】：尾部的灯珠，赋给指定的 RGB 颜色
//                for (i = 0; i < 24; ++i)
//                {
//                    send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(0, 0, 0) << i) & 0X800000) ? WS1 : WS0);
//                }
//            }
//            else
//            {
//                // 【熄灭区域】：头部前面的灯珠，赋给黑色 (0, 0, 0)
//                for (i = 0; i < 24; ++i)
//                {
//                    send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(0, 0, 0) << i) & 0X800000) ? WS1 : WS0);
//                }
//            }
//        }
//				}
				
		
		 
		 
			 for ( Led_bite_cnt_rectangle = (256); Led_bite_cnt_rectangle < (263);Led_bite_cnt_rectangle++)
		{
			uint8_t i;
			if (Led_bite_cnt_rectangle < 263)
			{for (i = 0; i < 24; ++i)
		   {
			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue)<< i) & 0X800000) ? WS1 : WS0);
			 }
			}
		 }
		
			 for ( Led_bite_cnt_rectangle = (264); Led_bite_cnt_rectangle < (266);Led_bite_cnt_rectangle++)
		{
			uint8_t i;
			if (Led_bite_cnt_rectangle < 266)
			{for (i = 0; i < 24; ++i)
		   {
			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue)<< i) & 0X800000) ? WS1 : WS0);
			 }
			}
		 }
		 	 for ( Led_bite_cnt_rectangle = (271); Led_bite_cnt_rectangle < (274);Led_bite_cnt_rectangle++)
		{
			uint8_t i;
			if (Led_bite_cnt_rectangle < 274)
			{for (i = 0; i < 24; ++i)
		   {
			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
			 }
			}
		 }
		
		  for ( Led_bite_cnt_rectangle = (279); Led_bite_cnt_rectangle < (287);Led_bite_cnt_rectangle++)
		{
			uint8_t i;
			if (Led_bite_cnt_rectangle < 287)
			{for (i = 0; i < 24; ++i)
		   {
			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
			 }
			}
		 }
		
 for ( Led_bite_cnt_rectangle = (288); Led_bite_cnt_rectangle < (293);Led_bite_cnt_rectangle++)
		{
			uint8_t i;
			if (Led_bite_cnt_rectangle < 293)
			{for (i = 0; i < 24; ++i)
		   {
			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
			 }
			}
		 }
		
	 for ( Led_bite_cnt_rectangle = (296); Led_bite_cnt_rectangle < (298);Led_bite_cnt_rectangle++)
		{
			uint8_t i;
			if (Led_bite_cnt_rectangle < 298)
			{for (i = 0; i < 24; ++i)
		   {
			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
			 }
			}
		 }	 
		
 for ( Led_bite_cnt_rectangle = (299); Led_bite_cnt_rectangle < (301);Led_bite_cnt_rectangle++)
		{
			uint8_t i;
			if (Led_bite_cnt_rectangle < 301)
			{for (i = 0; i < 24; ++i)
		   {
			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
			 }
			}
		 }			
for ( Led_bite_cnt_rectangle = (304); Led_bite_cnt_rectangle < (306);Led_bite_cnt_rectangle++)
		{
			uint8_t i;
			if (Led_bite_cnt_rectangle < 306)
			{for (i = 0; i < 24; ++i)
		   {
			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
			 }
			}
		 }		
for ( Led_bite_cnt_rectangle = (308); Led_bite_cnt_rectangle < (310);Led_bite_cnt_rectangle++)
		{
			uint8_t i;
			if (Led_bite_cnt_rectangle < 310)
			{for (i = 0; i < 24; ++i)
		   {
			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
			 }
			}
		 }	
	for ( Led_bite_cnt_rectangle = (312); Led_bite_cnt_rectangle < (314);Led_bite_cnt_rectangle++)
		{
			uint8_t i;
			if (Led_bite_cnt_rectangle < 314)
			{for (i = 0; i < 24; ++i)
		   {
			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
			 }
			}
		 }	
		for ( Led_bite_cnt_rectangle = (317); Led_bite_cnt_rectangle < (319);Led_bite_cnt_rectangle++)
		{
			uint8_t i;
			if (Led_bite_cnt_rectangle < 319)
			{for (i = 0; i < 24; ++i)
		   {
			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
			 }
			}
		 }		
	
	
}
//点亮边框灯条进度函数
void WS_WriteAll_RGB_FRAME_JINDU(uint8_t n_R, uint8_t n_G, uint8_t n_B ,uint8_t num_q,uint8_t num_h)
{
	uint16_t n = 0;
	
	//PIXEL_NUM_FRAME
	//flag_color
	//1--17-39
	//2--12-44
	//3--9-49
	//4--5-51
	for(n =num_q; n <num_h; n++) //PIXEL_NUM_FRAME
	{
		 WS281x_SetPixelRGB_FRAME(n,n_R, n_G, n_B);
	}
	 WS_Load_Frame();
}


void LED_Blue_rectangle_current(uint16_t wait_time ,uint8_t n_R, uint8_t n_G, uint8_t n_B )
{
  uint8_t red=n_R;
  uint8_t green=n_G;
  uint8_t blue=n_B;
	
	arrow_LED_cut_down_rectangle(0,send_Buf_move_rectangle, red, green, blue);
	arrow_LED_cut_down_rectangle(40,send_Buf_move_rectangle, red, green, blue);
	arrow_LED_cut_down_rectangle(80,send_Buf_move_rectangle, red, green, blue);
	arrow_LED_cut_down_rectangle(120,send_Buf_move_rectangle, red, green, blue);
  arrow_LED_cut_down_rectangle(160,send_Buf_move_rectangle, red, green, blue);
	arrow_LED_cut_down_rectangle(200,send_Buf_move_rectangle, red, green, blue);
//	arrow_LED_cut_down_rectangle(240,send_Buf_move_rectangle, red, green, blue);
	
	for ( Led_bite_cnt_rectangle = (243); Led_bite_cnt_rectangle < (245);Led_bite_cnt_rectangle++)
		{
			uint8_t i;
			if (Led_bite_cnt_rectangle < 245)
			{for (i = 0; i < 24; ++i)
		   {
			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
			 }
			}
		 }
		
		 for ( Led_bite_cnt_rectangle = (250); Led_bite_cnt_rectangle < (254);Led_bite_cnt_rectangle++)
		{
			uint8_t i;
			if (Led_bite_cnt_rectangle < 254)
			{for (i = 0; i < 24; ++i)
		   {
			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
			 }
			}
		 }
		 
//		中心R标
///////////////////////////////////////////////////////////////////////		 
			 for ( Led_bite_cnt_rectangle = (256); Led_bite_cnt_rectangle < (263);Led_bite_cnt_rectangle++)
		{
			uint8_t i;
			if (Led_bite_cnt_rectangle < 263)
			{for (i = 0; i < 24; ++i)
		   {
			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue)<< i) & 0X800000) ? WS1 : WS0);
			 }
			}
		 }
		
			 for ( Led_bite_cnt_rectangle = (264); Led_bite_cnt_rectangle < (266);Led_bite_cnt_rectangle++)
		{
			uint8_t i;
			if (Led_bite_cnt_rectangle < 266)
			{for (i = 0; i < 24; ++i)
		   {
			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue)<< i) & 0X800000) ? WS1 : WS0);
			 }
			}
		 }
		 	 for ( Led_bite_cnt_rectangle = (271); Led_bite_cnt_rectangle < (274);Led_bite_cnt_rectangle++)
		{
			uint8_t i;
			if (Led_bite_cnt_rectangle < 274)
			{for (i = 0; i < 24; ++i)
		   {
			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
			 }
			}
		 }
		
		  for ( Led_bite_cnt_rectangle = (279); Led_bite_cnt_rectangle < (287);Led_bite_cnt_rectangle++)
		{
			uint8_t i;
			if (Led_bite_cnt_rectangle < 287)
			{for (i = 0; i < 24; ++i)
		   {
			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
			 }
			}
		 }
		
 for ( Led_bite_cnt_rectangle = (288); Led_bite_cnt_rectangle < (293);Led_bite_cnt_rectangle++)
		{
			uint8_t i;
			if (Led_bite_cnt_rectangle < 293)
			{for (i = 0; i < 24; ++i)
		   {
			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
			 }
			}
		 }
		
	 for ( Led_bite_cnt_rectangle = (296); Led_bite_cnt_rectangle < (298);Led_bite_cnt_rectangle++)
		{
			uint8_t i;
			if (Led_bite_cnt_rectangle < 298)
			{for (i = 0; i < 24; ++i)
		   {
			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
			 }
			}
		 }	 
		
 for ( Led_bite_cnt_rectangle = (299); Led_bite_cnt_rectangle < (301);Led_bite_cnt_rectangle++)
		{
			uint8_t i;
			if (Led_bite_cnt_rectangle < 301)
			{for (i = 0; i < 24; ++i)
		   {
			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
			 }
			}
		 }			
for ( Led_bite_cnt_rectangle = (304); Led_bite_cnt_rectangle < (306);Led_bite_cnt_rectangle++)
		{
			uint8_t i;
			if (Led_bite_cnt_rectangle < 306)
			{for (i = 0; i < 24; ++i)
		   {
			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
			 }
			}
		 }		
for ( Led_bite_cnt_rectangle = (308); Led_bite_cnt_rectangle < (310);Led_bite_cnt_rectangle++)
		{
			uint8_t i;
			if (Led_bite_cnt_rectangle < 310)
			{for (i = 0; i < 24; ++i)
		   {
			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
			 }
			}
		 }	
	for ( Led_bite_cnt_rectangle = (312); Led_bite_cnt_rectangle < (314);Led_bite_cnt_rectangle++)
		{
			uint8_t i;
			if (Led_bite_cnt_rectangle < 314)
			{for (i = 0; i < 24; ++i)
		   {
			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
			 }
			}
		 }	
		for ( Led_bite_cnt_rectangle = (317); Led_bite_cnt_rectangle < (319);Led_bite_cnt_rectangle++)
		{
			uint8_t i;
			if (Led_bite_cnt_rectangle < 319)
			{for (i = 0; i < 24; ++i)
		   {
			  send_Buf_rectangle[24 * Led_bite_cnt_rectangle + i] = (((WS281x_Color(red, green, blue) << i) & 0X800000) ? WS1 : WS0);
			 }
			}
		 }	
////////////////////////////////////////////////////		
		WS_Load_rectangle();
}
//	void WS_Write_R(uint8_t n_R, uint8_t n_G, uint8_t n_B )
//{
//	uint16_t bit_24_low,bit_24_mid,bit_24_hig;
//	uint8_t dat[24];
//	// 将RGB数据进行转换
//	for (bit_24_low = 0; bit_24_low < 8; bit_24_low++)
//	{
//		dat[bit_24_low] = ((n_G & 0x80) ? WS1 : WS0);
//		n_G <<= 1;
//	}
//	for (bit_24_mid = 0; bit_24_mid < 8; bit_24_mid++)
//	{
//		dat[bit_24_mid + 8] = ((n_R & 0x80) ? WS1 : WS0);
//		n_R <<= 1;
//	}
//	for (bit_24_hig = 0; bit_24_hig < 8; bit_24_hig++)
//	{
//		dat[bit_24_hig + 16] = ((n_B & 0x80) ? WS1 : WS0);
//		n_B <<= 1;
//	}
//		for (int i = 0; i < PIXEL_NUM_R; i++)
//	{
//		for (int j = 0; j < 24; j++)
//		{
//			send_Buf_R[i * 24 + j] = dat[j];
//		}
//	}
//	for(int i = PIXEL_NUM_R * 24; i < NUM_R; i++)
//		send_Buf_R[i] = 0; // 占空比比为0，全为低电平
//	
//	WS_Load_R();
//}
/*
	 点亮红色箭头函数
*/
//void LED_Red_rectangle_current(uint16_t wait_time ,uint8_t n_R, uint8_t n_G, uint8_t n_B )
//{

// uint16_t bit_24_low,bit_24_mid,bit_24_hig,i,j;
//	uint8_t dat[24];
//	// 将RGB数据进行转换
//	for (bit_24_low = 0; bit_24_low < 8; bit_24_low++)
//	{
//		dat[bit_24_low] = ((n_G & 0x80) ? WS1 : WS0);
//		n_G <<= 1;
//	}
//	for (bit_24_mid = 0; bit_24_mid < 8; bit_24_mid++)
//	{
//		dat[bit_24_mid + 8] = ((n_R & 0x80) ? WS1 : WS0);
//		n_R <<= 1;
//	}
//	for (bit_24_hig = 0; bit_24_hig < 8; bit_24_hig++)
//	{
//		dat[bit_24_hig + 16] = ((n_B & 0x80) ? WS1 : WS0);
//		n_B <<= 1;
//	}
//	
//	
//	for (i = 0; i < PIXEL_NUM_REC; i++)
//	{

//		for (j = 0; j < 24; j++)
//		{
//			send_Buf_rectangle[i * 24 + j] = dat[j];
//		}
//	}
//	for(i = PIXEL_NUM_REC * 24; i < NUM_REC; i++)
//		send_Buf_rectangle[i] = 0; // 占空比比为0，全为低电平
//	

//	arrow_LED_cut_down_rectangle(0,send_Buf_move_rectangle);
//	arrow_LED_cut_down_rectangle(40,send_Buf_move_rectangle);
//	arrow_LED_cut_down_rectangle(80,send_Buf_move_rectangle);
//	arrow_LED_cut_down_rectangle(120,send_Buf_move_rectangle);
//  arrow_LED_cut_down_rectangle(160,send_Buf_move_rectangle);
//	arrow_LED_cut_down_rectangle(200,send_Buf_move_rectangle);
//	arrow_LED_cut_down_rectangle(240,send_Buf_move_rectangle);


//	WS_Load_rectangle();
//}





void rainbowCycle(uint8_t wait)
{
	uint32_t timestamp = HAL_GetTick();
	uint16_t i;
	static uint8_t j;
	static uint32_t next_time = 0;

	static uint8_t loop = 0;
	if (loop == 0)
		next_time = timestamp;
	loop = 1; //首次调用初始化

	if ((timestamp > next_time)) // && (timestamp - next_time < wait*5))
	{
		j++;
		next_time = timestamp + wait;
		for (i = 0; i < PIXEL_NUM_REC; i++)
		{
			WS281x_SetPixelColor(i, Wheel(((i * 256 / (PIXEL_NUM_REC)) + j) & 255));
		}
	}
	WS_Load_rectangle();
}


void LED_Police_rectangle_current(uint8_t wait_time)
{
	uint32_t timestamp = HAL_GetTick();
	uint16_t pixel_num;
	static uint32_t next_time = 0;
  uint32_t light_open_flag = 0;
	uint16_t light_change_cut = 0;
	
	light_change_cut++;
	
	
	
	if (next_time < wait_time)
	{
		if ((uint64_t)timestamp + wait_time - next_time > 0)
			light_open_flag = 1;
	}
	else if (timestamp > next_time)
	{
		light_open_flag = 1;
	}
	if (light_open_flag) 
	{
		next_time = timestamp + wait_time;
		for (pixel_num = 0; pixel_num < PIXEL_NUM_REC; pixel_num++)
		{

			
			if(light_change_cut <= 300)
			{
				WS281x_SetPixelRGB_CIR(pixel_num,0,0,255);
			}
			if(light_change_cut > 300)
			{
				WS281x_SetPixelRGB_CIR(pixel_num,255,0,0);
				if(light_change_cut >= 600)
				{
					light_change_cut = 0;
				}
			}
			
		}
			
	}
	WS_Load_rectangle();
}

uint32_t Wheel(uint8_t WheelPos)
{
	WheelPos = 255 - WheelPos;
	if (WheelPos < 85)
	{
		return WS281x_Color(255 - WheelPos * 3, 0, WheelPos * 3);
	}
	if (WheelPos < 170)
	{
		WheelPos -= 85;
		return WS281x_Color(0, WheelPos * 3, 255 - WheelPos * 3);
	}
	WheelPos -= 170;
	return WS281x_Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}


/*
 整活系列之彩色流水大转盘
*/

/*
	 点亮边框灯条函数(延时)
*/
void WS_WriteAll_RGB_FRAME_Delay(uint8_t n_R, uint8_t n_G, uint8_t n_B )
{
	uint16_t n = 0;
	
	for(n =  0 ; n <PIXEL_NUM_FRAME; n++)
	{
		 WS281x_SetPixelRGB_FRAME(n,n_R, n_G, n_B);
		 HAL_Delay(3);
	}
	
	WS_Load_Frame();
}

/*
	 点亮PCB上边框函数
*/
void WS_WriteAll_RGB_FRAME_UP_Delay(uint8_t n_R, uint8_t n_G, uint8_t n_B )
{
	uint16_t n = 0;
	
	for(n =  0 ; n <PIXEL_NUM_FRAME_UP; n++)
	{
		 WS281x_SetPixelRGB_FRAME_UP(n,n_R, n_G, n_B);
		 HAL_Delay(2);
	}
	
	WS_Load_frame_up();
}

/*
	 点亮中间灯带
*/
void WS_WriteAll_RGB_RECT_Delay(uint8_t n_R, uint8_t n_G, uint8_t n_B )
{
	uint16_t n = 0;
	
	for(n =  0 ; n <PIXEL_NUM_REC; n++)
	{
		 WS281x_SetPixelColor_RECT(n,n_R, n_G, n_B);
		 HAL_Delay(2);
	}
	
	WS_Load_rectangle();
}
/*
	 点亮PCB下边框函数
*/
void WS_WriteAll_RGB_FRAME_DOWN_Delay(uint8_t n_R, uint8_t n_G, uint8_t n_B )
{
   uint16_t n = 0;
	
	for(n =  0 ; n <PIXEL_NUM_FRAME_DOWN; n++)
	{
		 WS281x_SetPixelRGB_FRAME_DOWN(n,n_R, n_G, n_B);
		 HAL_Delay(2);
	}
	
	WS_Load_frame_down();
}

/*
	 全部点亮
*/
void WS_Write_RGB_ALL_Delay(uint8_t n_R, uint8_t n_G, uint8_t n_B )
{
  uint16_t n = 0;	
	for(n = 0 ; n < 224; n++)//272 224
	{
		 WS281x_SetPixelRGB_CIR(n,n_R, n_G, n_B);
		 HAL_Delay(3);
	}
	
	WS_Load_Circle();
}
void Rainbow_Current(uint8_t n_R, uint8_t n_G, uint8_t n_B )
{
	 WS_WriteAll_RGB_FRAME_Delay( n_R,  n_G,  n_B );
	 WS_WriteAll_RGB_FRAME_UP_Delay( n_R,  n_G,  n_B );
	 WS_WriteAll_RGB_FRAME_DOWN_Delay( n_R,  n_G,  n_B );
	 WS_WriteAll_RGB_RECT_Delay( n_R,  n_G,  n_B );
}












