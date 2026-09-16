#ifndef _DATATYPE_H_
#define _DATATYPE_H_
#include "stdint.h"
typedef float          FP_32;   /*!< Single precision floating point     */
typedef double         FP_64;   /*!< Double precision floating point     */

/*各传感器（加速度计、陀螺仪、磁罗盘）的测量数据*/
typedef struct
{
	int16_t	X;
	int16_t	Y;
	int16_t	Z;
}Tri_int16;


typedef union {
  int16_t S[3];
  Tri_int16 Sensor;
}Sensor_Data;

typedef struct
{
	FP_32	X;
	FP_32	Y;
	FP_32	Z;
}Tri_FP32;

typedef union {
  FP_32 A[3];
  Tri_FP32 Attitude;
}Attitude_Data;

typedef struct
{
	FP_64	X;
	FP_64	Y;
	FP_64	Z;
}Tri_FP64;

typedef union {
  FP_64 A[3];
  Tri_FP64 Attitude;
}Attitude_Data64;

#endif



