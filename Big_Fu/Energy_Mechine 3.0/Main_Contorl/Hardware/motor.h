#ifndef _motor_H
#define _motor_H
#include "stm32f4xx.h"                  // Device header
#include "main.h"
#include "Data_train.h"

#define RATE_BUF_SIZE 6
#define ALL_PID_INCR 6
#define MYABS(a)    ((a >= 0) ? (a) : -(a)) 
#define VALUE_LIMIT(v, l, h)   ((v) < (l) ? (l) : ((v) > (h) ? (h) : (v)))
#define VALUE_ABS_LIMIT(v, a)  VALUE_LIMIT(v, -a, a)

typedef struct{
	int16_t VX;
	int16_t VY;
	int8_t  Shift;
	int16_t SPIN;
	int8_t  win;
}Data;

typedef struct{
	int32_t raw_value;   					 //编码器不经处理的原始值
	int32_t last_raw_value;					 //上一次的编码器原始值
	int32_t ecd_value;                       //经过处理后连续的编码器值
	int32_t diff;							 //两次编码器之间的差值
	int32_t temp_count;                      //计数用
	uint8_t buf_count;						 //滤波更新buf用
	int32_t ecd_bias;						 //初始编码器值	
	int32_t ecd_raw_rate;					 //通过编码器计算得到的速度原始值
	int32_t rate_buf[6];	     //buf，for filter
	int32_t round_cnt;						 //圈数
	int32_t filter_rate;					 //速度
	int32_t filter_rate_max;				 //最大速度
	float ecd_angle;						 //角度
	float init_angle; 						 //开始读一次数据
	int16_t current;
	int16_t velo;
	int16_t electricity;
	int16_t re_speed;
	int8_t tempature;	
//	float real_angle;
//	float zero_angle;
//	float relative_zero_angle;
//	float ALL_angle;
//	int tool_flag;
}dji_motor;

typedef struct{
	int16_t VX;
	int16_t VY;
	int16_t DM_YAW;
	int8_t Turn;////yaw轴数据
	int8_t QB;
	float real_yaw;
}Data_From_C;

typedef union{
float     YAW;
struct{
uint8_t   y1    :8;
uint8_t    y2   :8;
uint8_t	  y3    :8;
uint8_t	 y4     :8;
}yaw;
}DATA_pack_imu;
	
void CAN1_rxDataHandler(uint32_t canId, uint8_t *rxBuf);
void getControlDatb(DATA_pack_imu *c);
void getEncoderData(volatile dji_motor *v);
void motor_set_gimbal(float iPit,float iYaw, float Pit, float Yaw);
void BOARD_CAN1_RX(uint32_t canId, uint8_t *rxBuf);
void get_DAFU_measure(Big_Fu_info_t *ptr)    ;
#endif
