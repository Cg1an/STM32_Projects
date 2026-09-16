#include "CH351.h"

uint8_t  lunshi_LED_8;
uint32_t lunshi_ALL_KEY;

void CH351_lunshi_init(void)
{
	 CH351_RST_RESET;  
	 CH351_RST_SET;
	
	 CH351_WRITE3_SET(CH351_MODE_GPIO1,0XFF);//1为输出模式
	 CH351_WRITE3_SET(CH351_MODE_GPIO2,0X00);//1为输出模式
	 CH351_WRITE3_SET(CH351_MODE_GPIO3,0X00);//0为输入模式
	 CH351_WRITE3_SET(CH351_MODE_GPIO4,0X00);//1为输出模式
	
	 CH351_WRITE3_SET(CH351_GPIO1,0X01);//0为下拉
	 CH351_WRITE3_SET(CH351_GPIO2,0XFF);//0为下拉
	 CH351_WRITE3_SET(CH351_GPIO3,0XFF);//1为上拉
	 CH351_WRITE3_SET(CH351_GPIO4,0XFF);//0为下拉 
}//暂时没有


void CH351_24KEY_ALL(void)
{   
	  MX_GPIO_DEInit_keych351();
    uint32_t key=0;
    key+=CH351_READ3_SET(CH351_GPIO2);
	  key=key<<8;
	  key+=CH351_READ3_SET(CH351_GPIO3);
	  key=key<<8;
	  key+=CH351_READ3_SET(CH351_GPIO4);	
	  lunshi_ALL_KEY=key;
		return ;		
}

void CH351_8LED_ALL(uint8_t LED)
{   
	 	 CH351_WRITE3_SET(CH351_MODE_GPIO1,0XFF);//1为输出模式
	   CH351_WRITE3_SET(CH351_GPIO1,LED);//1为输出模式
		 return;
}





