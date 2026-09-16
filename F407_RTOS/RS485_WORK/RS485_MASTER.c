#include "RS485_work.h"
#include "cmsis_os.h"
#include "usbd_customhid.h"
#include "usbd_custom_hid_if.h"
#include "stm32f4xx_hal.h"
uint16_t CRC16;
uint8_t volatile usb_receive_point;  //usb接收标志
uint8_t USB_DATA_BUFFER[64]={0X0,0xA5,0xFF,0xFF};
uint8_t USB_TX_DATA_ADDR;
uint8_t aaa;
void RS485_init_Master(void)  //从机初始化程序
{  
   RS485_OPEN_TX;
   __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
   HAL_UART_Receive_DMA(&huart2,(uint8_t *)Master_RS485_RX_buffer, RS485_RX_BUFF_SIZE);  //打开串口空闲中断，配置DMA传输 	
}

//完成一次数据采集工作后，将接收数据拼接发给usb
//接收一次usb数据后，把接收数据拆分发给从机
void Master_Analyze_RS485_DATA()  //主机数据解析函数
{   
   if(Master_RS485_RX_buffer[0]==0xa5)   //帧头校验成功
	 {
			//数据ID校验成功
			if(Master_RS485_RX_buffer[1]==Device_Information[Master_RS485_RX_buffer[1]].DEVICE_ID)
			{
				//将数据拼装在usb发送帧内部
			  DATA_COPY_UINT8(USB_TX_buffer+1+USB_TX_DATA_ADDR,Master_RS485_RX_buffer+1,Device_Information[Master_RS485_RX_buffer[1]].Device_tx_length+1);
				USB_TX_DATA_ADDR+=Device_Information[Master_RS485_RX_buffer[1]].Device_tx_length+1;
				receive_point=0;	
				Master_RS485_RX_buffer[0]=0x00;
			}
	 }
	 return ;
}


void Analyze_USB_DATA(void)    //usb数据分析函数
{ 
     //这个没有00
     uint16_t CRC16_OUT=(uint16_t)(USB_RX_buffer[61]<<8)+USB_RX_buffer[62];
     CRC16=CRC_calculate_crc16(0xffff,0x8005,0,1,1,USB_RX_buffer,61);
    //先判断帧头和crc校验
    if(USB_RX_buffer[0]==0XA5 && CRC16==CRC16_OUT)
    { 
		    //把USB数据拷贝出来
			  memcpy(USB_DATA_BUFFER,USB_RX_buffer+1,64);
		    //DATA_COPY_UINT8(USB_DATA_BUFFER,USB_RX_buffer+1,60);
        usb_receive_point=1; //通过校验，usb接收标志变成1						
		}
		else
		{
		    usb_receive_point=0; //通过校验，usb接收标志变成0		
		}		
}

//拆分usb数据
void  Split_usb_data(void)
{  

   
    //usb接收终端完成CRC校验后的数据帧
		
		//首先循环拆分
		//确定帧头数据后拼装发送帧
		//等待接收帧收完拼接
		//拼接发送帧率
    //拼完发送回PC
		uint8_t RS485_TX_ADDR=0;
		//当帧头不为0xff，并且没有超出缓冲区
		//发送缓冲区地址变为1;
		USB_TX_DATA_ADDR=1;
		USB_TX_buffer[0]=0X00;
		USB_TX_buffer[1]=0XA5;
		
		while((USB_DATA_BUFFER[RS485_TX_ADDR]!=0)&&(USB_DATA_BUFFER[RS485_TX_ADDR]!=0xff)&&RS485_TX_ADDR<60)
		{ 
     
        if(Device_Information[USB_DATA_BUFFER[RS485_TX_ADDR]].DEVICE_ID==USB_DATA_BUFFER[RS485_TX_ADDR])//确定地址存在，获得接收数据长度
				{    //开启发送模式
				     RS485_OPEN_TX;	
				     //组装发送帧		 
						 Master_RS485_TX_buffer[0]=0XA5;
						 //带ID位拷贝数据
						 DATA_COPY_UINT8(Master_RS485_TX_buffer+1,USB_DATA_BUFFER+RS485_TX_ADDR,Device_Information[USB_DATA_BUFFER[RS485_TX_ADDR]].Device_rx_length+1);
						 //添加CRC16
						 
						 //发送帧
						 //发送前清除接收防止误判
						 Master_RS485_RX_buffer[0]=0;
						 HAL_UART_Transmit_DMA(&huart2,(uint8_t *)Master_RS485_TX_buffer,Device_Information[USB_DATA_BUFFER[RS485_TX_ADDR]].Device_rx_length+4); 
						 receive_point=1;//发送标志变0等待接收	
						 uint8_t time=2;
						 //等待回执
						 while(receive_point==1&&time--) //10ms无回传自动下一个
						 {
						    osDelay(10);	
						 }
				
				     //更新读取地址
					   RS485_TX_ADDR+=(Device_Information[USB_DATA_BUFFER[RS485_TX_ADDR]].Device_rx_length)+1;
						 
				}
				else
				{
				    //找不到合格ID整个帧无法解算，直接退出
				    break;
				} 
		}

		//加装CRC16 CRC16=CRC_calculate_crc16(0xffff,0x8005,0,1,1,TCP_OUT_DATA,62);
    USB_TX_buffer[1]=0xA5;
    CRC16=CRC_calculate_crc16(0xffff,0x8005,0,1,1,USB_TX_buffer+1,61);
	  USB_TX_buffer[62]=CRC16>>8;
	  USB_TX_buffer[63]=CRC16;
		USB_TX_buffer[USB_TX_DATA_ADDR+1]=0xff;
		
		 queue_data_in(USB_TX_buffer,64,&usb_rx_queue);
	//	USBD_CUSTOM_HID_SendReport(&hUsbDeviceHS, USB_TX_buffer, 64);	
		
		//清空状态
		USB_TX_DATA_ADDR=1;
		Master_RS485_TX_buffer[0]=0;	
		Master_RS485_RX_buffer[0]=0;	
}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
   if(huart==&huart2)
	 {
  	    aaa++;    RS485_OPEN_RX;	
	 }
}


