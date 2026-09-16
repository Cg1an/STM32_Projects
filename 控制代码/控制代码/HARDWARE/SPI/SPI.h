#ifndef __AS5048A_H
#define __AS5048A_H
#include "sys.h"
#include "delay.h"
#include <Math.h>
#include "algorithm.h"

//附加了偶校验后的各个寄存器读取指令
#define CMD_ANGLE            0xffff
#define CMD_AGC              0x7ffd
#define CMD_MAG              0x7ffe
#define CMD_CLAER            0x4001
#define CMD_NOP              0xc000

#define		AS5048_CS_LOW()  PDout(2)=0
#define		AS5048_CS_HIGH() PDout(2)=1


void SPI_Init(void);
void IdentifyOffSet(void);

#endif

