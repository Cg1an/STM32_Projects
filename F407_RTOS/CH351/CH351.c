#include "CH351.h"
#include "gpio.h"


void CH351_init(void)
{
	 CH351_WRITE_SET(CH351_MODE_GPIO1,0XFF);//1为输出模式
	 CH351_WRITE_SET(CH351_MODE_GPIO2,0XFF);//1为输出模式
	 CH351_WRITE_SET(CH351_MODE_GPIO3,0XFF);//1为输出模式
	 CH351_WRITE_SET(CH351_MODE_GPIO4,0XFF);//1为输出模式
	
	 CH351_WRITE_SET(CH351_GPIO1,0X00);//0为下拉
	 CH351_WRITE_SET(CH351_GPIO2,0X00);//0为下拉
	 CH351_WRITE_SET(CH351_GPIO3,0X00);//0为下拉
	 CH351_WRITE_SET(CH351_GPIO4,0X00);//0为下拉  
}


void A0_A2_SET(uint8_t A0_2)
{
	
   uint16_t  PIN=CH351_A0_Pin;
	 uint8_t j;
	 for(j=0;j<3;j++)
	 {
	   if(A0_2&0X01) //最后一位不为0引脚就是高电平
		 {
		    HAL_GPIO_WritePin(CH351_A0_GPIO_Port,PIN,GPIO_PIN_SET);
		 }
		 else
		 {
		    HAL_GPIO_WritePin(CH351_A0_GPIO_Port,PIN,GPIO_PIN_RESET);
		 }
		 A0_2=A0_2>>1;
		 PIN=PIN*2;
	 }
	 return;
}

void D0_D7_WRITE(uint8_t D0_8)
{
   uint16_t  PIN=CH351_D0_Pin;
	 uint8_t j;
	 for(j=0;j<8;j++)
	 {
	   if(D0_8&0X01) //最后一位不为0引脚就是高电平
		 {
		    HAL_GPIO_WritePin(CH351_D0_GPIO_Port,PIN,GPIO_PIN_SET);
		 }
		 else
		 {
		    HAL_GPIO_WritePin(CH351_D0_GPIO_Port,PIN,GPIO_PIN_RESET);
		 }
		 D0_8=D0_8>>1;
		 PIN=PIN*2;
	 }
	 //读写使能

	 return;
}

uint8_t D0_D7_READ(uint8_t A0_2)
{
	 MX_GPIO_Init_INTPUT();
   uint16_t  PIN=CH351_D7_Pin;
	 uint8_t output=0;
	 uint8_t j;  
//   HAL_GPIO_WritePin(CH351_D0_GPIO_Port,CH351_D0_Pin|CH351_D1_Pin|CH351_D2_Pin|CH351_D3_Pin|CH351_D4_Pin|CH351_D5_Pin|CH351_D6_Pin|CH351_D7_Pin,GPIO_PIN_RESET);
	//摆好读写标志位 	//读写使能 	
	 for(j=0;j<8;j++)
	 {
		 output=output<<1;
	   if(HAL_GPIO_ReadPin(CH351_D0_GPIO_Port,PIN)==1)
		 {
		    output+=1;
		 }
		 else
		 {
		    
		 }
		 PIN=PIN/2;
	 }
	 //读写使能
	 MX_GPIO_Init_OUTPUT();
	 return output;
}

//读写CH351设置;
void CH351_WRITE_SET(uint8_t A0_2,uint8_t data)
{
	   CH351_CS_SET;
    A0_A2_SET(A0_2);
    D0_D7_WRITE(data);
		CH351_WR_RESET;
	  CH351_RD_SET;
    CH351_CS_RESET;
	  CH351_CS_SET;
}
uint8_t CH351_READ_SET(uint8_t A0_2)
{
	  uint8_t output;
	  CH351_CS_SET;
    A0_A2_SET(A0_2);
	  CH351_WR_SET;
	  CH351_RD_RESET;
		CH351_CS_RESET;
    output=D0_D7_READ(A0_2);
		CH351_CS_SET;
	  return output;
}

//读写CH351设置;
void CH351_WRITE2_SET(uint8_t A0_2,uint8_t data)
{
	   CH351_CS2_SET;
    A0_A2_SET(A0_2);
    D0_D7_WRITE(data);
		CH351_WR_RESET;
	  CH351_RD_SET;
    CH351_CS2_RESET;
	  CH351_CS2_SET;
}
uint8_t CH351_READ2_SET(uint8_t A0_2)
{
    uint8_t output;
	  CH351_CS2_SET;
    A0_A2_SET(A0_2);
	  CH351_WR_SET;
	  CH351_RD_RESET;
		CH351_CS2_RESET;
    output=D0_D7_READ(A0_2);
		CH351_CS2_SET;
	  return output;
}

//读写CH351设置;
void CH351_WRITE3_SET(uint8_t A0_2,uint8_t data)
{
	  CH351_CS3_SET;
    A0_A2_SET(A0_2);
    D0_D7_WRITE(data);
		CH351_WR_RESET;
	  CH351_RD_SET;
    CH351_CS3_RESET;
	  CH351_CS3_SET;
}
uint8_t CH351_READ3_SET(uint8_t A0_2)
{
	  uint8_t output;
	  CH351_CS3_SET;
    A0_A2_SET(A0_2);
	  CH351_WR_SET;
	  CH351_RD_RESET;
		CH351_CS3_RESET;
    output=D0_D7_READ(A0_2);
		CH351_CS3_SET;
	  return output;
}




