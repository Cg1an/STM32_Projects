#ifndef __PID_H
#define __PID_H	 
#include "sys.h"	    

void PID_init(void);   

int16_t Angle_PID(float set,float actual);  //jiaodu

int16_t Speed_PID(float s_spend,float a_spend);   //sudu
	
float spend_control( float left_spend,float right_spend,int16_t set_spend );  //ËÙ¶È¿ØÖÆ



#endif




