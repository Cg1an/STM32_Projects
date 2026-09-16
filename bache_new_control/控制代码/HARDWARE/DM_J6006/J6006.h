#ifndef _J6006_
#define _J6006_

#include "sys.h"
#include "can.h"

#define  MAX_N 4     //额定扭矩4nm
#define  LIMIT_N 12  //峰值扭矩12nm


//J6006	电机接收函数结构体
typedef struct 
{
  uint8_t ID;       //电机ID
	uint8_t M_ERR;    //电机状态
	uint8_t Temp_MOS; //电机mos温度
	uint8_t Temp_MOTOR; //电机转子温度
	//MIT模式，原始传入数值
	uint16_t MIT_P_des; //位置给定
	uint16_t MIT_V_des; //速度给定
	uint16_t MIT_KP;    //位置比例系数
	uint16_t MIT_KD;    //位置微分系数
	uint16_t MIT_T_ff;  //转矩给定值
	
	//MIT模式，计算设定数值
	float MIT_P_set; //位置给定
	float MIT_V_set; //速度给定
	float MIT_KP_set;    //位置比例系数
	float MTI_KD_set;    //位置微分系数
	float MIT_T_set;    //转矩给定值
	
	//V模式，速度给定模式
	float V_v_des;      //V_f
	
//接收解算参数
//解算P V T时需要得知目标数据的上下限，才能得出准确的浮点数据
//电机默认P 位置 V速度 T扭矩的最大值
	float P_MAX;       
	float V_MAX;        
	float T_MAX;        
//电机默认P 位置 V速度 T扭矩的最小值	
  float P_MIN;       
	float V_MIN;        
	float T_MIN; 
//接收到的原始数据的P 位置 V速度 T扭矩
	float p_int;      //位置    
	float v_int;      //速度 
	float t_int;      //扭矩
//计算出来的P 位置 V速度 T扭矩
	float position;      //位置    
	float velocity;      //速度 
	float torque;        //扭矩
	
  float KT_OUT;      //扭矩转换电流系数，使用默认即可
}J6006;

//    p_int=(_hcan->pRxMsg->Data[1]<<8)|_hcan->pRxMsg->Data[2];
//    v_int=(_hcan->pRxMsg->Data[3]<<4)|(_hcan->pRxMsg->Data[4]>>4);
//    t_int=((_hcan->pRxMsg->Data[4]&0xF)<<8)|_hcan->pRxMsg->Data[5];
//    position = uint_to_float(p_int, P_MIN, P_MAX, 16); // (-12.5,12.5)
//    velocity = uint_to_float(v_int, V_MIN, V_MAX, 12); // (-45.0,45.0)
//    torque = uint_to_float(t_int, T_MIN, T_MAX, 12); // (-18.0,18.0)

typedef union
{
   float f_data[2];
   uint8_t u8_data[8];	 
}f_u8;

extern J6006 J6006_yaw;
extern J6006 J6006_pitch;

void DAMIAO_MOTOR_INIT(void);  //达妙电机初始化
void J6006_MOTOR_DATA_INIT(void); //电机设定参数初始化
void V_mode_send(CAN_TypeDef CAN, uint16_t ID, J6006 M);     //速度模式下电机发送函数
void get_motor_data(J6006 * motor,uint8_t *rx_data);                   //电机数据解算函数
void MIT_mode_send(CAN_TypeDef * CAN, CAN_DATA * DATA, J6006 *M);   //MIT模式下电机发送函数

#endif

