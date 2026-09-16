#ifndef __PID_H
#define __PID_H	 
#include "sys.h"	    

void PID_init(void);   


int16_t Inremental_PI2(float s_spend,float a_spend);
	
float spend_control( float left_spend,float right_spend,int16_t set_spend );  //ËÙ¶È¿ØÖÆ



#endif




