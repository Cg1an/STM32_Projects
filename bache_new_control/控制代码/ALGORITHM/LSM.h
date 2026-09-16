#ifndef __LSM_H
#define __LSM_H	 
#include "sys.h" 
#include <string.h>
#include "Matrix.h" 

#define ratio 10000.0f
#define Order 3
#define Size 9

typedef struct
{
	//double z;//Z的新的观测值
	//double newPart_phi[Order];//每一步的M_phi矩阵的新增观测值
	double delta_SquarePhi[Size];//stage0阶段，每一步的矩阵M_phi的平方的增量
	double SquarePhi[Size];//矩阵M_phi的平方
	double M_P[Size];//矩阵M_phi的平方的逆
	double M_P0[Size];//矩阵M_P的前一拍计算结果
	double phi_Z[Order];//M_phi矩阵的转置，乘以由Z的各个观测值组成的矢量，所得到的矢量。
	double thet[Order];//参数观测结果
	double K[Order];
	double P_newPhi[Order];//矩阵M_P与矩阵M_phi的新增观测值的乘积
	double M_temp[Size];//计算过程中用到的临时矩阵
	double temp;//计算过程中用到的临时参数
	double Ratio_Ill;
	u16 order;//矩阵的阶数
	u8 stage;//0-M1还不能求逆，处于积累状态；1-M1可以求逆。
	u8 reserved;
} LSM_t;

extern LSM_t LSM;

extern void InitLSM(int order);
extern void LSM_UpdateStatus(double z, double * pNewPart_phi);
extern void LSM_UpdateStatus_Forget(double z, double * pNewPart_phi, double rho2);
#endif

