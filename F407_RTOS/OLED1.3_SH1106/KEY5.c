#include "key.h"

//E
#define OLED_KEY1 GPIO_PIN_0
#define OLED_KEY2 GPIO_PIN_1
//B
#define OLED_KEY3 GPIO_PIN_3
#define OLED_KEY4 GPIO_PIN_4
#define OLED_KEY5 GPIO_PIN_5
#define OLED_KEY6 GPIO_PIN_6
#define OLED_KEY7 GPIO_PIN_7
//G
#define OLED_KEY8 GPIO_PIN_14
#define OLED_KEY9 GPIO_PIN_15


void OLED_5KEY_init(void)
{
   MX_GPIO_OLED_9KEY_INIT();
}


uint16_t OLED_9KEYREAD(void)
{
   uint16_t key;
	   key+=HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_0);
   key=key<<1;
		 key+=HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_1);
   key=key<<1;
		 key+=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_3);
   key=key<<1;
		 key+=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_4);
   key=key<<1;
		 key+=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_5);
   key=key<<1;
		 key+=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_6);
   key=key<<1;
		 key+=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7);
   key=key<<1;
		 key+=HAL_GPIO_ReadPin(GPIOG,GPIO_PIN_14);
	 key=key<<1;
		 key+=HAL_GPIO_ReadPin(GPIOG,GPIO_PIN_15);	
   return key;	
}


