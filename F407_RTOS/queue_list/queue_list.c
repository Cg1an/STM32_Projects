#include "queue_list.h" 
#include "string.h"
//队列数据构成    02  55 55       03  77  77  77，首部存放数据长度信息
 uint8_t usb_rx_queue_buffer[usb_rx_queue_size];
 
 queue  usb_rx_queue;
 
 uint8_t usb_tx_queue_buffer[usb_rx_queue_size];
 
 queue  usb_tx_queue;
 
 void queue_init(void)
 {
     //usart_rx
     //初始化队首队尾地址
     usb_rx_queue.front_add=0;
		 usb_rx_queue.rear_add=0;
		 
		 //装填队列缓冲区地址
		 usb_rx_queue.queue_list=usb_rx_queue_buffer;
		 //装填缓冲区大小
		 usb_rx_queue.list_size=usb_rx_queue_size;
		 usb_rx_queue.space_size=usb_rx_queue_size;
		 //
		 usb_rx_queue.data_byte_size=1;
 }
 
 //入队操作输入数据地址，长度，进入队列名
 //输出1成功  非1失败
uint8_t queue_data_in(uint8_t *data,uint16_t size,queue* queue_list)
{
      if(queue_list->space_size<(size+queue_list->data_byte_size)) 
			{//如果队列剩余空间不够，存放失败
      			return 2;	
			}
			else
			{  
			   		//更新剩余空间
			      queue_list->space_size-=(size+queue_list->data_byte_size);
			      //从队列尾部存入数据
						if(queue_list->data_byte_size==1)
						{
						     queue_list->queue_list[queue_list->rear_add]=size;
						}
						else
						{
						      queue_list->queue_list[queue_list->rear_add]=size>>8;
									queue_list->queue_list[queue_list->rear_add+1]=size;
						}
						uint16_t add=0;
						queue_list->rear_add=(queue_list->rear_add+queue_list->data_byte_size)% queue_list->list_size;
				    while(size)
						{
						     queue_list->queue_list[queue_list->rear_add]=data[add];
						     queue_list->rear_add=(queue_list->rear_add+1)% queue_list->list_size;
								 add++;
								 size--;
						}
						
						return 1;
			}
			return 0;
}
 //出队操作      输出数据地址,进入队列名
 //输出1成功  非1失败
uint8_t queue_data_out(uint8_t *data,queue* queue_list)
{
      if(queue_list->space_size>=queue_list->list_size) 
			{//如果队列为空，取出失败
      			return 2;	
			}
			else
			{  
			      uint16_t  data_size;
						if(queue_list->data_byte_size==1)
						{
						     data_size=queue_list->queue_list[queue_list->front_add];
						}
						else
						{
						     data_size=(((uint16_t)queue_list->queue_list[queue_list->front_add])<<8)+queue_list->queue_list[queue_list->rear_add+1];
						}
						//更新剩余空间
			      queue_list->space_size+=(data_size+queue_list->data_byte_size);
					  //从队列头部取出数据
						uint16_t add=0;
						queue_list->front_add=(queue_list->front_add+queue_list->data_byte_size)% queue_list->list_size;
						while(data_size)
						{
						      data[add]=queue_list->queue_list[queue_list->front_add];
						      queue_list->front_add=(queue_list->front_add+1)% queue_list->list_size;
								  add++;
								  data_size--;					
						}
						return 1;
			}
			return 0;
}


//清除队列头部的数据，出队，丢弃队列头部的数据
 //输出1成功  非1失败
uint8_t queue_out(queue* queue_list)
{
       if(queue_list->space_size>=queue_list->list_size) 
			{//如果队列为空，取出失败
      			return 2;	
			}
			else
			{  
			      uint16_t  data_size;
						if(queue_list->data_byte_size==1)
						{
						     data_size=queue_list->queue_list[queue_list->front_add];
						}
						else
						{
						     data_size=(((uint16_t)queue_list->queue_list[queue_list->front_add])<<8)+queue_list->queue_list[queue_list->rear_add+1];
						}
						//更新剩余空间
			      queue_list->space_size+=(data_size+queue_list->data_byte_size);
					  //从队列头部取出数据
						uint16_t add=0;
						queue_list->front_add=(queue_list->front_add+queue_list->data_byte_size)% queue_list->list_size;
						while(data_size)
						{
						      //data[add]=queue_list->queue_list[queue_list->front_add];
						      queue_list->front_add=(queue_list->front_add+1)% queue_list->list_size;
								  add++;
								  data_size--;					
						}
						return 1;
			}
			return 0;
}


//多个队列放在一个缓冲区里
 
 