#include "RS485_work.h"
#include "usart.h"
#include "string.h"

//uint8_t RS485_RX_buffer[RS485_RX_BUFF_SIZE];
//uint8_t RS485_TX_buffer[RS485_RX_BUFF_SIZE];
//缓冲区长度64，协议长度最大63
//主机使用缓冲区
uint8_t Master_RS485_RX_buffer[RS485_RX_BUFF_SIZE];
uint8_t Master_RS485_TX_buffer[RS485_RX_BUFF_SIZE];
//从机使用缓冲区
uint8_t Slave_RS485_RX_buffer[RS485_RX_BUFF_SIZE];
uint8_t Slave_RS485_TX_buffer[RS485_RX_BUFF_SIZE];
//USB收发缓冲区
uint8_t USB_RX_buffer[RS485_RX_BUFF_SIZE];
uint8_t USB_TX_buffer[RS485_RX_BUFF_SIZE*2];  //做大发送缓冲区

//发送地址的长度//
uint8_t RS485_TX_length;
//设备默认指向缓冲区。

uint8_t TX_BUFFER[5];
uint8_t RX_BUFFER[5];
//从机通过拨码开关确定地址，等待主机发起询问时通信地址


//主机只负责轮询从机，把从机数据拼起来。
//主机通信数据一律63字节
//根据从机通信直接拼接从机数据。
//主机发送USB信息时根据信息裁剪数据
//uint8_t Master1_input_Slave[10]={};
//uint8_t Master2_input_Slave[10]={};


//uint8_t Slave1_input_decive[10]={};
//uint8_t Slave2_input_decive[10]={};


//usb协议头部默认00为ID
//主机从头开始轮询。询问内容由usb协议发起。


//usb协议ID位置解释了USB需要的上位机内容
//A5  帧结构报文id
//ID1  访问设备ID
//DATA
//ID2  访问设备ID
//
//
//62 63 CRC16校验


uint32_t freertos_task_list;    //任务启动标志位

uint8_t volatile receive_point; //数据接收标志位



void USER_RS485_USART2_IDLE_INT(UART_HandleTypeDef *huart)
{
   if(huart==&huart2)//串口2中断被触发
	 {
	   if((__HAL_UART_GET_FLAG(huart,UART_FLAG_IDLE)!=0))
		 {
			  RS485_OPEN_RX;
		    __HAL_UART_CLEAR_IDLEFLAG(huart);
				HAL_UART_DMAStop(huart);	
				if( RS485_WORK_SLAVE==OPEN)//从机分析模式打开，分析从机数据
				{
				    Slave_Analyze_RS485_DATA();		
            HAL_UART_Receive_DMA(huart,(uint8_t *) Slave_RS485_RX_buffer, RS485_RX_BUFF_SIZE);						
				}
				else
				{
				    Master_Analyze_RS485_DATA();	
            HAL_UART_Receive_DMA(huart,(uint8_t *)Master_RS485_RX_buffer, RS485_RX_BUFF_SIZE);						
				}
				
						 			 
		 }
	 }
}
//RS485主机模式，先发信息，然后等待接收,串口1版本
void USER_RS485_USART1_IDLE_INT(UART_HandleTypeDef *huart)
{
   if(huart==&huart1)//串口2中断被触发
	 {
	   if((__HAL_UART_GET_FLAG(huart,UART_FLAG_IDLE)!=0))
		 {
			  RS485_OPEN_RX;
		    __HAL_UART_CLEAR_IDLEFLAG(huart);				
			  HAL_UART_DMAStop(huart);	
				
			  //HAL_UART_Receive_DMA(huart,(uint8_t *)RS485_RX_buffer, RS485_RX_BUFF_SIZE);
//			  if(receive_point==1)
//			  {
//				   RS485_OPEN_TX;
//			     //HAL_UART_Transmit_DMA(huart,(uint8_t *)RS485_TX_buffer, RS485_RX_BUFF_SIZE);
//				   RS485_OPEN_RX;
//				   receive_point=0;
//			  }				 
		 }
	 }
}


void DATA_COPY_UINT8(uint8_t *objective,uint8_t *source,uint8_t length)
{
	  memcpy(objective,source,length);		 
//   uint8_t i;
//	 for(i=0;i<length;i++)
//	 {		
//	     objective[i]=source[i]; 
//	 }
 	 return;
}

//使用示例CRC_calculate_crc16(0xffff,0x8005,0,1,1,puchMsg,usDataLen)
//puchMsg 传入校验串首地址
//usDataLen 校验字符长度
//CRC16MODBUS校验代码
uint16 CRC_calculate_crc16(uint16 wCRCin,uint16 wCPoly,uint16 wResultXOR,char input_invert,char ouput_invert,unsigned char *puchMsg, int usDataLen)
{
    uint8 wChar = 0;
    while (usDataLen--)
    {
        wChar = *(puchMsg++);
        if(input_invert) //输入值反转
        {
            uint8 temp_char = wChar;
            wChar=0;
            for(int i=0;i<8;++i)
            {
                if(temp_char&0x01)
                    wChar|=0x01<<(7-i);
                temp_char>>=1;
            }
        }
        wCRCin ^= (wChar << 8);
        for (int i = 0; i < 8; i++)
        {
            if (wCRCin & 0x8000)
                wCRCin = (wCRCin << 1) ^ wCPoly;
            else
                wCRCin = wCRCin << 1;
        }
    }
    if(ouput_invert)
    {
        uint16 temp_short = wCRCin;
        wCRCin=0;
        for(int i=0;i<16;++i)
        {
            if(temp_short&0x01)
            wCRCin|=0x01<<(15-i);
            temp_short>>=1;
        }
    }
    return (wCRCin^wResultXOR);
}


void DEVICE_ID_LIST_INIT(void)  //初始化地址表格
{
//全部初始化 
//0 dufault no usea
   for(uint8_t i=0;i<DEVICE_ID_LENGTH;i++)
	 {
	   Device_Information[i].DEVICE_ID=0x00;                    //设备ID 0X00
	   Device_Information[i].Device_rx_addr=RX_BUFFER;                  //设备接收地址
	   Device_Information[i].Device_rx_length=0;                //接收数据长度
	   Device_Information[i].Device_tx_addr=TX_BUFFER;                  //设备发送地址
	   Device_Information[i].Device_tx_length=0;                //发送地址长度 
	 }

//the4SWitch is 0001 the task1 and task2 will be use
   Device_Information[1].DEVICE_ID=0x01;                    //设备ID 0X01
	 Device_Information[1].Device_rx_addr=CH453_12NUM_3LED;    //设备接收地址
	 Device_Information[1].Device_rx_length=13;               //接收数据长度
	 Device_Information[1].Device_tx_addr= (uint8_t *)&CH351_10KEY;  //设备发送地址
	 Device_Information[1].Device_tx_length=2;                //发送地址长度
//
   Device_Information[2].DEVICE_ID=0x02;                    //设备ID 0X02
	 Device_Information[2].Device_rx_addr=&CH351_8LED;                  //设备接收地址
	 Device_Information[2].Device_rx_length=1;                //接收数据长度
	 Device_Information[2].Device_tx_addr=&CH351_7_8KEY;                  //设备发送地址
	 Device_Information[2].Device_tx_length=1;                //发送地址长度
	 
//the4Switch is 0010 the task3 will be use
	 Device_Information[3].DEVICE_ID=0x03;                    //设备ID 0X03
	 Device_Information[3].Device_rx_addr=&lunshi_LED_8;                  //设备接收地址
	 Device_Information[3].Device_rx_length=1;                //接收数据长度
	 Device_Information[3].Device_tx_addr=(uint8_t *)&lunshi_ALL_KEY;     //设备发送地址
	 Device_Information[3].Device_tx_length=4;                //发送地址长度

//



}

//usb one frame data exp
/*
A5 01 FF FF 00 00 FF FF 00 00 FF FF 00 00 FF 02 01 03 ff ff 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 E5 96

A5 03 0f FF 00 00 FF FF 00 00 FF FF 00 00 FF 02 01 03 ff ff 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 5E 94

A5 03 07 FF 00 00 FF FF 00 00 FF FF 00 00 FF 02 01 03 ff ff 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 54 14

*/




