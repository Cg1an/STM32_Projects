#include "sys.h"
#include "delay.h"
#include "can.h"



int main()
{
	delay_init(168);
NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
//delay_init(168);
CAN_Configure();

while (1)

{
    Set_ChassisMotor_Current(1000,800,800,1000);

}


}

