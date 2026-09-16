#include "CH351.h"

uint32_t CH351_key_7;
uint16_t CH351_10KEY;
uint8_t CH351_8LED;
uint8_t CH351_7_8KEY;




//车长仓7个按键读取函数
//void MX_GPIO_Init_INTPUT(void);
//void MX_GPIO_Init_OUTPUT(void);
//void MX_GPIO_Init_keych351(void);
//void MX_GPIO_CH452_INIT(void);
//void MX_GPIO_OLED_9KEY_INIT(void); 
//void MX_GPIO_Init(void);

void CH351_7KEY_ONLY_init(void)
{  
	 MX_GPIO_DEInit_keych351();
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
	 //MX_GPIO_Init_keych351();

}//暂时没有

void CH351_7KEY_ONLY_init_CN1(void)
{

   
	 MX_GPIO_DEInit_keych351();
	 CH351_RST_RESET;  
	 CH351_RST_SET;
	
	 CH351_WRITE_SET(CH351_MODE_GPIO1,0XFF);//1为输出模式
	 CH351_WRITE_SET(CH351_MODE_GPIO2,0X00);//1为输出模式
	 CH351_WRITE_SET(CH351_MODE_GPIO3,0X00);//0为输入模式
	 CH351_WRITE_SET(CH351_MODE_GPIO4,0X00);//1为输出模式
	
	 CH351_WRITE_SET(CH351_GPIO1,0X01);//0为下拉
	 CH351_WRITE_SET(CH351_GPIO2,0XFF);//0为下拉
	 CH351_WRITE_SET(CH351_GPIO3,0XFF);//1为上拉
	 CH351_WRITE_SET(CH351_GPIO4,0XFF);//0为下拉
	 //MX_GPIO_Init_keych351();

}//暂时没有

void CH351_24KEY_GET7(void)
{   
	  MX_GPIO_DEInit_keych351();
    uint32_t key=0;
    key+=CH351_READ3_SET(CH351_GPIO2);
		CH351_7_8KEY=key;//这个给上位机
	  key=key<<8;
	  key+=CH351_READ3_SET(CH351_GPIO3);
	  key=key<<8;
	  key+=CH351_READ3_SET(CH351_GPIO4);	
	  CH351_key_7=key;
	  //MX_GPIO_Init_keych351();
		return ;
		
}

void CH351_24KEY_GET7_CN1(void)
{   
	  MX_GPIO_DEInit_keych351();
    uint32_t key=0;
    key+=CH351_READ_SET(CH351_GPIO2);
	  key=key<<8;
	  key+=CH351_READ_SET(CH351_GPIO3);
	  key=key<<8;
	  key+=CH351_READ_SET(CH351_GPIO4);	
	  CH351_10KEY=key>>8;
	  //MX_GPIO_Init_keych351();
		return ;	
}

void CH351_8LED_WRITE_CN1(uint8_t LED)
{   
	 	 CH351_WRITE_SET(CH351_MODE_GPIO1,0XFF);//1为输出模式
	   CH351_WRITE_SET(CH351_GPIO1,LED);//1为输出模式
		 return ;
}


void CH351_8LED_WRITE(uint8_t LED)
{   
	//GPIO_PinState CSMOD = GPIO_PIN_RESET;
	
	   //MX_GPIO_DEInit_keych351();
		
			
	
//		CSMOD=HAL_GPIO_ReadPin(CH351_CS3_GPIO_Port,CH351_CS3_Pin);
//		CH351_CS3_RESET;
	
	 	 CH351_WRITE3_SET(CH351_MODE_GPIO1,0XFF);//1为输出模式
	   CH351_WRITE3_SET(CH351_GPIO1,LED);//1为输出模式
	
	 // HAL_GPIO_WritePin(CH351_CS3_GPIO_Port,CH351_CS3_Pin,CSMOD);
	
	   //MX_GPIO_Init_keych351();
		 return ;
		
}

