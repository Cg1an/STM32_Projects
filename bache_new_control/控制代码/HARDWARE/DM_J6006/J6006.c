#include "J6006.h"

J6006 J6006_yaw;
J6006 J6006_pitch;

void J6006_MOTOR_DATA_INIT()
{
	 J6006_yaw.P_MAX=1;
	 J6006_yaw.P_MIN=-1;
	 J6006_yaw.T_MAX=12;
	 J6006_yaw.T_MIN=-12;
	 J6006_yaw.V_MAX=45;
	 J6006_yaw.V_MIN=-45;
	 
	 J6006_pitch.P_MAX=1;
	 J6006_pitch.P_MIN=-1;
	 J6006_pitch.T_MAX=12;
	 J6006_pitch.T_MIN=-12;
	 J6006_pitch.V_MAX=45;
	 J6006_pitch.V_MIN=-45;
}
//-------------end--------------


float uint_to_float(int x_int, float x_min, float x_max, int bits)
{
  /// converts unsigned int to float, given range and number of bits ///
  float span = x_max - x_min;
  float offset = x_min;
  return ((float)x_int)*span/((float)((1<<bits))) + offset;
}
//-------------end--------------


 
int float_to_uint(float x, float x_min, float x_max, int bits)
{
  /// Converts a float to an unsigned int, given range and number of bits///
  float span = x_max - x_min; float offset = x_min;
  return (int) ((x-offset)*((float)((1<<bits)-1))/span);
}
//-------------end--------------



void V_mode_send(CAN_TypeDef CAN, uint16_t ID, J6006 M) //速度模式下电机发送函数
{
//	uint8_t   txdata[8];
	f_u8     motor_data;

	motor_data.f_data[0]=M.V_v_des;

	//fdcanx_send_data(hfdcan, ID,  motor_data.u8_data, 8);	
	CAN_Data[0] = 3; CAN_Data[1] = 0x00; CAN_Data[2] = 0x00;
	CAN_SetDATA(&CAN_Motor1, CAN_Data); CAN_Trans(CAN1, &CAN_Motor1);
}
//-------------end--------------

void MIT_mode_send(CAN_TypeDef * CAN, CAN_DATA * DATA, J6006 *M) //速度模式下电机发送函数
{
	uint8_t   txdata[8];

	M->MIT_P_des = float_to_uint(M->MIT_P_set, M->P_MIN, M->P_MAX, 16);
	M->MIT_V_des = float_to_uint(M->MIT_V_set, M->V_MIN, M->V_MAX, 12);
	M->MIT_KP = float_to_uint(0.0f, 0.0, 500.0, 12);
	M->MIT_KD = float_to_uint(1.0f, 0.0, 5.0, 12);
	M->MIT_T_ff = float_to_uint(M->MIT_T_set, M->T_MIN, M->T_MAX, 12);

	txdata[0] = (M->MIT_P_des >> 8);
	txdata[1] =  M->MIT_P_des;
	txdata[2] = (M->MIT_V_des >> 4);
	txdata[3] = ((M->MIT_V_des&0xF)<<4)|(M->MIT_KP>>8);
	txdata[4] = M->MIT_KP;
	txdata[5] = (M->MIT_KD >> 4);
	txdata[6] = ((M->MIT_KD&0xF)<<4)|(M->MIT_T_ff>>8);
	txdata[7] =  M->MIT_T_ff;

	CAN_SetDATA(DATA, txdata); CAN_Trans(CAN, DATA);
}
//-------------end--------------


void get_motor_data(J6006 * motor,uint8_t *rx_data)
{
    motor->p_int=(rx_data[1]<<8)|rx_data[2];
    motor->v_int=(rx_data[3]<<4)|(rx_data[4]>>4);
    motor->t_int=((rx_data[4]&0xF)<<8)|rx_data[5];
    motor->position = uint_to_float(motor->p_int+1, motor->P_MIN, motor->P_MAX, 16); // (-12.5,12.5)
    motor->velocity = uint_to_float(motor->v_int+1, motor->V_MIN, motor->V_MAX, 12); // (-45.0,45.0)
    motor->torque = uint_to_float(motor->t_int+1, motor->T_MIN, motor->T_MAX, 12); // (-18.0,18.0)
}












