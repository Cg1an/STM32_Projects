#include "CH452.H"
#include "gpio.h"

#define CH452_RST_PIN PE1_KEY_Pin
#define CH452_RST_GPIO_Port PE1_KEY_GPIO_Port

void CH452_12KEY_init(void)
{
  
	  MX_GPIO_CH452_INIT();
	  HAL_GPIO_WritePin(CH452_RST_GPIO_Port,CH452_RST_PIN,GPIO_PIN_SET);
	
		HAL_GPIO_WritePin(CH452_RST_GPIO_Port,CH452_RST_PIN,GPIO_PIN_RESET);
	//¸´Î»Ò»ÏÂ
    CH452_Write(&hi2c1,0x07ff );
		//CH452_Write(&hi2c1,CH452_RESET );
	  CH452_Write(&hi2c1,0x0403);

}

uint8_t CH452_READ12KEY(void)
{
    return  CH452_Read(&hi2c1);
}
