#include "Matrix.h" 


//计算矩阵M(N*M)的转置：T(M)，计算结果为M*N。
void MTranspose(double * pResult, double * pOrigin, int N, int M)
{
	int i, j;
	for(i=0; i<N; i++)
	{
		for(j=0; j<M; j++)
		{
			pResult[j*N+i] = pOrigin[i*M+j];//将第i行第j列的元素，赋给第j行第i列。
		}
	}
}
//------------------end----------------------

//计算矩阵M(M*N)的平方：T(M)*M。其中T()表示矩阵转置，计算结果为N*N的对称阵。
void MSquare(double * pResult, double * pOrigin, int M, int N)
{
	double * tp;
	int i, j, k;
	for(i=0; i<N; i++)
	{
		for(j=0; j<=i; j++)//计算第i行的轴上及轴左侧的元素
		{
			tp = pResult+i*N+j;
			*tp = 0;
			for(k=0; k<M; k++)
			{
				*tp += pOrigin[k*N+i] * pOrigin[k*N+j];//将原来第i列（转置后变为第i行）与第j列逐个元素相乘。
			}
		}
		
		for(j=0; j<i; j++)//计算第i列的轴上方的元素
		{
			*(pResult+j*N+i) = *(pResult+i*N+j);
		}
	}
}
//------------------end----------------------

//计算矩阵M1(M*N)*M2(N*L)，计算结果为M*L。
void MMultiple(double * pResult, double * pOrigin1, double * pOrigin2, int M, int N, int L)
{
	double * tp;
	int i, j, k;
	for(i=0; i<L; i++)
	{
		for(j=0; j<M; j++)
		{
			tp = pResult+j*L+i;
			*tp = 0;
			for(k=0; k<N; k++)
			{
				*tp += pOrigin1[k+j*N] * pOrigin2[k*L+i];//将第j行与第i列逐个元素相乘。
			}
		}
	}
}
//------------------end----------------------

//计算矩阵M1(M*N)+M2(M*N)，计算结果为M*N。
void MAdd(double * pResult, double * pOrigin1, double * pOrigin2, int M, int N)
{
	int i, j;
	for(i=0; i<M; i++)
	{
		for(j=0; j<N; j++)
		{
			pResult[i*N+j] = pOrigin1[i*N+j] + pOrigin2[i*N+j];
		}
	}
}
//------------------end----------------------

//计算数乘矩阵N * M(M*N)。
void NumMultiple(double * pResult, double Num, double * pOrigin, int M, int N)
{
	int i, j;
	for(i=0; i<M; i++)
	{
		for(j=0; j<N; j++)
		{
			pResult[i*N+j] = Num * pOrigin[i*N+j];
		}
	}
}
//------------------end----------------------

//计算矩阵M(N*N)的逆。minV是用来判断病态矩阵元素值的阈值。
int MInverse(double * pResult, double * pOrigin, int N, double minV)
{
	double temp;
	int i, j, k;
	
	for(i=0; i<N; i++) pResult[i*N+i] = 1;//构建单位矩阵
	
	for(i=0; i<N; i++)
	{
		if((pOrigin[i*N+i] < minV)&&(pOrigin[i*N+i] > -minV))//如果发现某行的行首（轴上）元素太小，则找到该列下方的大的元素，并进行行行对调。
		{
			for(j=i+1; j<N; j++)
			{
				if(pOrigin[j*N+i] >= minV)
				{
					for(k=i; k<N; k++)
					{
						temp = pOrigin[j*N+k];
						pOrigin[j*N+k] = pOrigin[i*N+k];
						pOrigin[i*N+k] = temp;
					}
					
					for(k=0; k<N; k++)
					{
						temp = pResult[j*N+k];
						pResult[j*N+k] = pResult[i*N+k];
						pResult[i*N+k] = temp;
					}
					break;
				}
			}
			if(j==N) return -1;
		}
		
		
		//将原矩阵的行首（轴上）元素化为1
		temp = pOrigin[i*N+i];
		for(k=i; k<N; k++)  pOrigin[i*N+k] /= temp;
		for(k=0; k<N; k++)  pResult[i*N+k] /= temp;

		//消掉原矩阵的行首（轴上）元素共列的元素
		for(j=0; j<N; j++)
		{
			if(j==i) continue;
			temp = - pOrigin[j*N+i];
			for(k=i; k<N; k++) pOrigin[j*N+k] += temp * pOrigin[i*N+k];//原矩阵第j行中，从第i个元素起向后的各个（可能）非零元素
			for(k=0; k<N; k++) pResult[j*N+k] += temp * pResult[i*N+k];//结果矩阵第j行中，所有元素
		}

	}
	
	return 0;
}
//------------------end----------------------

