#include "RS485_work.h"


Device_information  Device_Information[DEVICE_ID_LENGTH];  //地址表格

//485从机c文件



void RS485_init_slave(void)  //从机初始化程序
{  
   RS485_OPEN_RX;
   __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
   HAL_UART_Receive_DMA(&huart2,(uint8_t *)Slave_RS485_RX_buffer, RS485_RX_BUFF_SIZE);  //打开串口空闲中断，配置DMA传输 	
}


//从机解析程序，首先主机发来查询报文，从机根据ID按照特定的长度回应报文
//
//
//
void Slave_Analyze_RS485_DATA()  //从机数据解析函数
{   
   RS485_TX_length=0; //发送长度归位	
   if(Slave_RS485_RX_buffer[0]==0xa5&&Slave_RS485_RX_buffer[1]!=0x00)   //帧头校验成功而且结尾不是结束帧OXFF
	 {
			uint32_t DEVICE_ID_list=freertos_task_list;
			for(uint8_t DEVICE_ID=1;DEVICE_ID<=32;DEVICE_ID++)
			{
			   //轮询并且查到相关ID,   freerto启动s线程id=轮询设备ID,地址表内ID==轮询设备ID ,485ID==轮询设备ID
			   if(
					  ((DEVICE_ID_list&0x00000001)!=0)&&
					    DEVICE_ID==Slave_RS485_RX_buffer[1]&&
				      DEVICE_ID==Device_Information[DEVICE_ID].DEVICE_ID
				   )
				 {   
				     //校验CRC16
				     //查表直接拷贝数据，完成接受
				     DATA_COPY_UINT8(Device_Information[DEVICE_ID].Device_rx_addr,Slave_RS485_RX_buffer+2,Device_Information[DEVICE_ID].Device_rx_length);
						 //装填回执信息，等待发送
						 Slave_RS485_TX_buffer[0]=0xA5;
						 Slave_RS485_TX_buffer[1]=DEVICE_ID;
						 DATA_COPY_UINT8(Slave_RS485_TX_buffer+2,Device_Information[DEVICE_ID].Device_tx_addr,Device_Information[DEVICE_ID].Device_tx_length);
						 RS485_TX_length=Device_Information[DEVICE_ID].Device_tx_length+4;//数据长度=1帧头，1位ID+发送数据长度+2位crc16
						 //添加CRC16校验

						 //更改接收标志
						 receive_point=1;   //接收标志变成1任务发送串口
				     break;
				 }
				 //移位ID继续查询
			   DEVICE_ID_list=DEVICE_ID_list>>1;
		  }	
	 }
	 else
	 {
	    receive_point=0;   //接收标志变成0
			RS485_TX_length=0; //发送长度归位
	 }
	 return ;
}


