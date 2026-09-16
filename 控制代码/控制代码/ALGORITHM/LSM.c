#include "LSM.h" 

LSM_t LSM;

void InitLSM(int order)
{
	int i, Size_M;
	
	LSM.stage = 0;
	LSM.order = order;//阶数
	Size_M = LSM.order * LSM.order;
	LSM.Ratio_Ill = 1e-10;
	
	for(i=0; i<Size_M; i++)
	{
		LSM.delta_SquarePhi[i] = 0;
	}
	
	for(i=0; i<LSM.order; i++)
	{
		LSM.phi_Z[i] = 0;
	}
}

void LSM_UpdateStatus(double z, double * pNewPart_phi)
{
	int i;
	
	switch(LSM.stage)
	{
		case 0:
			for(i=0; i<LSM.order; i++)
			{
				LSM.phi_Z[i] += pNewPart_phi[i] * z;
			}
			
			//计算新的矩阵Phi的平方：
			MSquare(LSM.delta_SquarePhi, pNewPart_phi, 1, LSM.order);
			MAdd(LSM.SquarePhi, LSM.SquarePhi, LSM.delta_SquarePhi, LSM.order, LSM.order);
			
			if(0 == MInverse(LSM.M_P, LSM.SquarePhi, LSM.order, LSM.Ratio_Ill))//计算矩阵Phi的平方的逆，并判断是否病态：
			{
				LSM.stage = 1;
				MMultiple(LSM.thet, LSM.M_P, LSM.phi_Z, LSM.order, LSM.order, 1);//计算初始估计值
			}
			
			break;
		
		case 1:
			//计算K：
			MMultiple(LSM.P_newPhi, LSM.M_P, pNewPart_phi, LSM.order, LSM.order, 1);
			MMultiple(&LSM.temp, pNewPart_phi, LSM.P_newPhi, 1, LSM.order, 1);
			LSM.temp = 1/(1+LSM.temp);
			NumMultiple(LSM.K, LSM.temp, LSM.P_newPhi, LSM.order, 1);
		
			//计算P：
			MMultiple(LSM.M_temp, LSM.K, pNewPart_phi, LSM.order, 1, LSM.order);//计算向量K与新增观测值的乘积所形成的矩阵
			for(i=0; i<LSM.order; i++)//矩阵LSM.M_temp减去单位矩阵
			{
				LSM.M_temp[i*LSM.order+i] -= 1;
			}
			NumMultiple(LSM.M_temp, -1, LSM.M_temp, LSM.order, LSM.order);
			memcpy(LSM.M_P0, LSM.M_P, LSM.order*LSM.order*8);
			MMultiple(LSM.M_P, LSM.M_temp, LSM.M_P0, LSM.order, LSM.order, LSM.order);
		
			//计算thet：
			MMultiple(&LSM.temp, pNewPart_phi, LSM.thet, 1, LSM.order, 1);
			LSM.temp = z - LSM.temp;
			NumMultiple(LSM.K, LSM.temp, LSM.K, LSM.order, 1);//因为向量K已经不再使用，用它来装载K乘以(z-phi*theta)后的结果。
			MAdd(LSM.thet, LSM.thet, LSM.K, LSM.order, 1);
					
			break;
		
		default:
			break;
	}
}


void LSM_UpdateStatus_Forget(double z, double * pNewPart_phi, double rho2)
{
	int i;
	
	switch(LSM.stage)
	{
		case 0:
			for(i=0; i<LSM.order; i++)
			{
				LSM.phi_Z[i] += pNewPart_phi[i] * z;
			}
			
			//计算新的矩阵Phi的平方：
			MSquare(LSM.delta_SquarePhi, pNewPart_phi, 1, LSM.order);
			MAdd(LSM.SquarePhi, LSM.SquarePhi, LSM.delta_SquarePhi, LSM.order, LSM.order);
			
			if(0 == MInverse(LSM.M_P, LSM.SquarePhi, LSM.order, LSM.Ratio_Ill))//计算矩阵Phi的平方的逆，并判断是否病态：
			{
				LSM.stage = 1;
				MMultiple(LSM.thet, LSM.M_P, LSM.phi_Z, LSM.order, LSM.order, 1);//计算初始估计值
			}
			
			break;
		
		case 1:
			//计算K：
			MMultiple(LSM.P_newPhi, LSM.M_P, pNewPart_phi, LSM.order, LSM.order, 1);
			MMultiple(&LSM.temp, pNewPart_phi, LSM.P_newPhi, 1, LSM.order, 1);
			LSM.temp = 1/(rho2+LSM.temp);
			NumMultiple(LSM.K, LSM.temp, LSM.P_newPhi, LSM.order, 1);
		
			//计算P：
			MMultiple(LSM.M_temp, LSM.K, pNewPart_phi, LSM.order, 1, LSM.order);//计算向量K与新增观测值的乘积所形成的矩阵
			for(i=0; i<LSM.order; i++)//矩阵LSM.M_temp减去单位矩阵
			{
				LSM.M_temp[i*LSM.order+i] -= 1;
			}
			NumMultiple(LSM.M_temp, -1, LSM.M_temp, LSM.order, LSM.order);
			memcpy(LSM.M_P0, LSM.M_P, LSM.order*LSM.order*8);
			MMultiple(LSM.M_P, LSM.M_temp, LSM.M_P0, LSM.order, LSM.order, LSM.order);
			rho2 = 1/rho2;
			NumMultiple(LSM.M_P, rho2, LSM.M_P, LSM.order, LSM.order);//将新的矩阵P除以rho2
		
			//计算thet：
			MMultiple(&LSM.temp, pNewPart_phi, LSM.thet, 1, LSM.order, 1);
			LSM.temp = z - LSM.temp;
			NumMultiple(LSM.K, LSM.temp, LSM.K, LSM.order, 1);//因为向量K已经不再使用，用它来装载K乘以(z-phi*theta)后的结果。
			MAdd(LSM.thet, LSM.thet, LSM.K, LSM.order, 1);
					
			break;
		
		default:
			break;
	}
}

