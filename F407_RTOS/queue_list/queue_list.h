#ifndef  __QUEUE_H
#define __QUEUE_H
#include "main.h"
//为usb创建收发缓冲区
#define   usb_rx_queue_size   1024
extern  uint8_t usb_rx_queue_buffer[usb_rx_queue_size];
typedef struct
{ 
      //队列队首队尾地址
      uint16_t  front_add;
			uint16_t    rear_add;
			//队列数组地址
      uint8_t    *queue_list;
			//队列长度
      uint16_t   list_size;
			//头部存放单个数据长度
			uint8_t  data_byte_size;
			uint16_t  space_size;
} queue;
//一个缓冲区+队列的创建+
extern  uint8_t usb_rx_queue_buffer[usb_rx_queue_size];
extern queue usb_rx_queue;



void queue_init(void);      //初始化队列信息
uint8_t queue_data_in(uint8_t *data,uint16_t size,queue* queue_list);    //入队
uint8_t queue_data_out(uint8_t *data,queue*  queue_list);                       //出队

//使用队列可以实现FIFO


#endif



