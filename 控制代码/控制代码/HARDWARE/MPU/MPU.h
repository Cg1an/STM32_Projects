#ifndef __MPU_H
#define __MPU_H
#include "sys.h"
#include "datatype.h"
//#include "UART_datarecord.h"
//#include "PWMIN.h"
//#include "PWMOUT.h"

#define		MPU_READ			0x80			//读取数据选择
#define		MPU_WRITE			0x00			//写入数据选择
#define		DUMMY				  0xFF			//读取数据时的DUMMY

/*MPU6000内部寄存器地址*/

#define		SMPRT_DIV			  0x19
#define		CONFIG				  0x1A
#define		GYRO_CONFIG			0x1B
#define		ACCEL_CONFIG		0x1C
#define		FIFO_EN         0x23
#define		INT_PIN_CFG     0x37
#define		INT_ENABLE      0x38
#define		ACCEL_XOUT_H		0x3B
#define		ACCEL_XOUT_L		0x3C
#define		ACCEL_YOUT_H		0x3D
#define		ACCEL_YOUT_L		0x3E
#define		ACCEL_ZOUT_H		0x3F
#define		ACCEL_ZOUT_L		0x40
#define		TEMP_OUT_H			0x41
#define		TEMP_OUT_L			0x42
#define		GYRO_XOUT_H			0x43
#define		GYRO_XOUT_L			0x44
#define		GYRO_YOUT_H			0x45
#define		GYRO_YOUT_L			0x46
#define		GYRO_ZOUT_H			0x47
#define		GYRO_ZOUT_L			0x48
#define		USER_CTRL			  0x6A
#define		PWR_MGMT_1			0x6B
#define 	PWR_MGMT_2			0x6C
#define 	FIFO_COUNT_H    0x72
#define 	FIFO_COUNT_L    0x73
#define 	FIFO_R_W        0x74
#define		WHO_AM_I			  0x75

#define		GFS_SEL_250			0
#define		GFS_SEL_500			1
#define		GFS_SEL_1000		2
#define		GFS_SEL_2000		3

#define		AFS_SEL_2			0
#define		AFS_SEL_4			1
#define		AFS_SEL_8			2
#define		AFS_SEL_16		3


#define		MPU_CS_LOW()  PDout(2)=0
#define		MPU_CS_HIGH() PDout(2)=1

extern FP_32          error_Angle_I;
extern FP_32          error_Angle;
extern FP_32          error_Gyro_I[3];

extern FP_32          Rc_Comm[4];
extern FP_32          Rc_Comm0[4];
extern FP_32          rc_Comm[4];
extern FP_32          Throt_Comm_LPF_a; //油门遥控信号的低通滤波参数
extern FP_32          Throt_Comm_LPF_b;

extern Sensor_Data		MPU_Gyro;
extern Sensor_Data		MPU_Accel;
extern Sensor_Data		MPU_Gyro_p;
extern Sensor_Data		MPU_Accel_p;

extern int16_t rcCommand[8];

extern uint8_t axis;
extern uint8_t armed;

void MPU_Init(void);
void MPU_StartRead(void);


#endif

