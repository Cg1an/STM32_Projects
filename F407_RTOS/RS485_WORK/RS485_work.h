#ifndef	RS485_H
#define	RS485_H

#include "main.h"
#define RS485_RX_BUFF_SIZE  64

#define RS485_OPEN_RX     HAL_GPIO_WritePin(RS485_RE_GPIO_Port,RS485_RE_Pin,GPIO_PIN_RESET)
#define RS485_OPEN_TX     HAL_GPIO_WritePin(RS485_RE_GPIO_Port,RS485_RE_Pin,GPIO_PIN_SET)

#define RS4851_OPEN_RX     HAL_GPIO_WritePin(RS485_RE1_GPIO_Port,RS485_RE1_Pin,GPIO_PIN_RESET)
#define RS4851_OPEN_TX     HAL_GPIO_WritePin(RS485_RE1_GPIO_Port,RS485_RE1_Pin,GPIO_PIN_SET)

#define uint16 uint16_t
#define uint8  uint8_t

//从机ID代号,由拨码开关决定 0x01-0x0e 为主机ID对应 

#define  Slave_0_ID     0x00   //防误触发，暂时不用
#define  Slave_1_ID     0x01           
#define  Slave_2_ID     0x02          
#define  Slave_3_ID     0x03
#define  Slave_4_ID     0x04
#define  Slave_5_ID     0x05
#define  Slave_6_ID     0x06
#define  Slave_7_ID     0x07     
#define  Slave_8_ID     0x08
#define  Slave_9_ID     0x09
#define  Slave_10_ID    0x0A
#define  Slave_11_ID    0x0B
#define  Slave_12_ID    0x0C
#define  Slave_13_ID    0x0D     
#define  Slave_14_ID    0x0E

//主机ID代号 拨码开关拨到OXOF是主机代码 
#define  Master_1_ID    0x0F


//显示面板代号，显示面板代号和freertos任务ID同步，
extern   uint32_t freertos_task_list; //任务启动列表，初始化时可使用，list=Device_task_1_ID|Device_task_2_ID|Device_task_3_ID方式选择启动任务
#define  Device_task_1_ID    0x00000001
#define  Device_task_2_ID    0x00000002   //待定
#define  Device_task_3_ID    0x00000004
#define  Device_task_4_ID    0x00000008
#define  Device_task_5_ID    0x00000010
#define  Device_task_6_ID    0x00000020
#define  Device_task_7_ID    0x00000040
#define  Device_task_8_ID    0x00000080
#define  Device_task_9_ID    0x00000100
#define  Device_task_0A_ID   0x00000200
#define  Device_task_0B_ID   0x00000400
#define  Device_task_0C_ID   0x00000800
#define  Device_task_0D_ID   0x00001000
#define  Device_task_0E_ID   0x00002000
#define  Device_task_0F_ID   0x00004000
#define  Device_task_10_ID   0x00008000
#define  Device_task_11_ID   0x00010000
#define  Device_task_12_ID   0x00020000

//PC ID 代号，跟据PCID，决定发送数据如何发给各个从机
//8位二进制位，前4位决定PC数据页地址，后面4位是PC机ID
//通过PC机ID可以知道从机ID地址和数据段长度和数据位置
#define  PC_1_ID     0x0001 
         
#define DEVICE_ID_LENGTH    32
//自定义数据类型，每个任务都有，存放任务位置信息
typedef struct 
{
   uint8_t  DEVICE_ID;         //设备ID
	 uint8_t  Device_tx_length;  //设备输出字节长度
	 uint8_t* Device_tx_addr;    // 设备输出地址
	 uint8_t  Device_rx_length; //设备输入字节长度
	 uint8_t* Device_rx_addr;    // 设备输入地址 
}Device_information;

extern Device_information   Device_Information[DEVICE_ID_LENGTH];     //暂定，不够可以扩展，0ID暂时不用
extern uint8_t RS485_TX_length;                         //计算回执帧的长度
//RS485总线数据一律为 0xa5+ID+数据位诺干
//RS485环形总线从机



//extern uint8_t RS485_RX_buffer[RS485_RX_BUFF_SIZE];
//extern uint8_t RS485_TX_buffer[RS485_RX_BUFF_SIZE];

extern uint8_t Master_RS485_RX_buffer[RS485_RX_BUFF_SIZE];
extern uint8_t Master_RS485_TX_buffer[RS485_RX_BUFF_SIZE];
//从机使用缓冲区
extern uint8_t Slave_RS485_RX_buffer[RS485_RX_BUFF_SIZE];
extern uint8_t Slave_RS485_TX_buffer[RS485_RX_BUFF_SIZE];
//USB收发缓冲区
extern uint8_t USB_RX_buffer[RS485_RX_BUFF_SIZE];
extern uint8_t USB_TX_buffer[RS485_RX_BUFF_SIZE*2];

extern uint8_t volatile receive_point;
extern uint8_t volatile usb_receive_point;


//uint8_t   CH423_6LED_5LED_2KEY=0;    //车载电源
//uint8_t   CH423_20LED_1KEY=0;        //数化同传
//uint8_t   CH423_8LED_3KEY=0;         //安全防护
//uint8_t   CH453_12NUM_3LED_10KEY=0;  //电源管理器
//uint8_t   LCD_CH351KEY_CH45210KEY=0; //BD时钟接线盒
//uint8_t   CH351_20LED_2KEY=0;        //网络通信控制设备
//uint8_t   CH351_24LED_2KEY=0;        //综合接口设备
//uint8_t   CH351_8LED_24KEY=0;        //司机右面板
//uint8_t   OLED1_3_9KEY=0;            //同步设备
//uint8_t   RS485_WORK=0;             //RS485通信任务
//uint8_t   DECEIVE_ID=0;             //设备ID



//数据拷贝函数
//objective 拷贝目的地址
//source    拷贝源头地址
//length    拷贝长度

void DATA_COPY_UINT8(uint8_t *objective,uint8_t *source,uint8_t length);
void USER_RS485_USART2_IDLE_INT(UART_HandleTypeDef *huart);
void RS485_init_slave(void);  //从机初始化
void RS485_init_Master(void); //主机初始化
void DEVICE_ID_LIST_INIT(void); //地址表格初始化
void Analyze_USB_DATA(void);//从机初始化程序
uint16 CRC_calculate_crc16(uint16 wCRCin,uint16 wCPoly,uint16 wResultXOR,char input_invert,char ouput_invert,unsigned char *puchMsg, int usDataLen);//CRC16
void Master_Analyze_RS485_DATA(void);  //主机数据解析函数
void Slave_Analyze_RS485_DATA(void); //从机解析程序 

void  Split_usb_data(void);//主机拆分usb程序
typedef struct
{
  //CH423_6LED_5LED_2KEY
	uint16_t CH423_6LED_5LED_652;
	uint8_t CH423_2KEY_652;
}USER_DATA;


extern USER_DATA ALL_DATA;
extern uint16_t CRC16;

//数据拷贝函数
//objective 拷贝目的地址
//source    拷贝源头地址
//length    拷贝长度




#endif



