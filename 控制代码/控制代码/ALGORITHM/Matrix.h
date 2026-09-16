#ifndef __MATRIX_H
#define __MATRIX_H	 
#include "sys.h" 

extern void MTranspose(double * pResult, double * pOrigin, int N, int M);  //计算矩阵M(N*M)的转置：T(M)，计算结果为M*N。
extern void MSquare(double * pResult, double * pOrigin, int N, int M);//计算矩阵M(M*N)的平方：T(M)*M。其中T()表示矩阵转置，计算结果为N*N的对称阵。
extern void MMultiple(double * pResult, double * pOrigin1, double * pOrigin2, int M, int N, int L);//计算矩阵M1(M*N)*M2(N*L)，计算结果为M*L。
extern void MAdd(double * pResult, double * pOrigin1, double * pOrigin2, int M, int N);//计算矩阵M1(M*N)+M2(M*N)，计算结果为M*N。
extern void NumMultiple(double * pResult, double Num, double * pOrigin, int M, int N);//计算数乘矩阵N * M(M*N)。
extern int MInverse(double * pResult, double * pOrigin, int N, double minV);//计算矩阵M(N*N)的逆。minV是用来判断病态矩阵元素值的阈值。

#endif

