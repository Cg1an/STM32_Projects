#include "main.h"


uint8_t usedata[8];
extern imu_t imu;
extern int16_t StartFlag,Back_DM;
Data_From_C Data_C ;

DATA_pack_imu DATA_pack_imu_recive;


//uint8_t can1_rx_data[8];

void get_DAFU_measure(Big_Fu_info_t *ptr)                                    
{                                                                   
	ptr->Color = usedata[0];																
	ptr->Last_Color = usedata[1];  
	ptr->Hit_LED_state =  usedata[2];  
	ptr->Single_Hit_State =  usedata[3];                
	ptr->Single_Working_State = usedata[4];      
 	ptr->All_Hit_State = usedata[5];    
	ptr->Spin_State = usedata[6];    
	ptr->Twinkle_Already= usedata[7]; 
}

////////////////////////////////电机数据
extern motor_t motor[num];
dji_motor motor1,motor2,motor3,motor4={0,0,0,0,0,0,0,0,0,0,0,0};

void getEncoderData(volatile dji_motor *v)
{
	int i=0;
	int32_t temp_sum = 0;    
	v->last_raw_value = v->raw_value;
	v->raw_value = (usedata[0]<<8)|usedata[1];
	v->diff = v->raw_value - v->last_raw_value;
	if(v->diff < -4096)    //两次编码器的反馈值差别太大，表示圈数发生了改变
	{
		v->round_cnt++;
		v->ecd_raw_rate = v->diff + 8192;
	}
	else if(v->diff>4096)
	{
		v->round_cnt--;
		v->ecd_raw_rate = v->diff - 8192;
	}		
	else
	{
		v->ecd_raw_rate = v->diff;
	}
	//计算得到连续的编码器输出值
	v->ecd_value = v->raw_value + v->round_cnt * 8192;
	//计算得到角度值，范围正负无穷大
	v->ecd_angle = (float)(v->raw_value - v->ecd_bias)*360/8192 + v->round_cnt * 360;
	v->rate_buf[v->buf_count++] = v->ecd_raw_rate;
	if(v->buf_count == RATE_BUF_SIZE)
	{
		v->buf_count = 0;
	}
	
	//计算速度平均值
	for(i = 0;i < RATE_BUF_SIZE; i++)
	{
		temp_sum += v->rate_buf[i];
	}
	v->filter_rate = (int32_t)(temp_sum/RATE_BUF_SIZE);
	
	if(fabs((float)v->filter_rate_max) < fabs((float)v->filter_rate))
	{
		v->filter_rate_max = v->filter_rate;
	}
	v->re_speed = (int16_t)(usedata[2]<<8|usedata[3]);
	v->current = (uint16_t)(usedata[4] << 8 | usedata[5]);
	v->tempature = usedata[6];
}

void getControlData(Data_From_C *c)
{
	 c->VX  = (int16_t)(usedata[0] << 8 | usedata[1]);
   c->VY  = (int16_t)(usedata[2] << 8 | usedata[3]);
	 c->Turn=(int16_t)(usedata[4]);
	 c->DM_YAW=(int16_t)(usedata[5]<<8|usedata[6]);
   c->QB=(int8_t)(usedata[7]);
}

void getControlDatb(DATA_pack_imu *c)
{
   c->yaw.y1=(uint8_t)(usedata[0]);
	 c->yaw.y2=(uint8_t)(usedata[1]);
	 c->yaw.y3=(uint8_t)(usedata[2]);
	 c->yaw.y4=(uint8_t)(usedata[3]);
}
/////////////////////////////////////////////////////////////////////////////////////////
CAN_RxHeaderTypeDef	RxHeader;
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{   
//	CAN_RxHeaderTypeDef	RxHeader;
  if(hcan->Instance==CAN1)
	{
		HAL_CAN_GetRxMessage(&hcan1,CAN_FILTER_FIFO0,&RxHeader,usedata);
switch(RxHeader.StdId)                   
     {
			case 0x201:getEncoderData(&motor1);   break;}
		BOARD_CAN1_RX(RxHeader.IDE,usedata);
	}
 if(hcan->Instance==CAN2)
	{
		HAL_CAN_GetRxMessage(&hcan2,CAN_FILTER_FIFO0,&RxHeader,usedata);
     switch(RxHeader.StdId)                   
     {
			case 0x201:getEncoderData(&motor1);   break;}
  }
}
Board_info_t Board_info;
extern Big_Fu_info_t Big_Fu_info[5];
Big_Fu_info_t Big_Fu_recive[5];
void CAN1_rxDataHandler(uint32_t canId, uint8_t *rxBuf)
{	
	BOARD_CAN1_RX(canId,rxBuf);
}

void BOARD_CAN1_RX(uint32_t canId, uint8_t *rxBuf)
{
	Board_info.offline_cnt_max = OFFLINE_TIME_MAX;
	
	switch (RxHeader.StdId)
	{
			case CAN_BOARD_ID_1:
			{
				get_DAFU_measure(&Big_Fu_info[0]);
				Board_info.B1_offline_cnt = 0;
				break;
			}
			case CAN_BOARD_ID_2:	
			{
				get_DAFU_measure(&Big_Fu_info[1]);
				Board_info.B2_offline_cnt = 0;
				break;
			}				
			case CAN_BOARD_ID_3:
			{
				get_DAFU_measure(&Big_Fu_info[2]);
				Board_info.B3_offline_cnt = 0;
				break;
			}
			case CAN_BOARD_ID_4:		
			{
				get_DAFU_measure(&Big_Fu_info[3]);
				Board_info.B4_offline_cnt = 0;
				break;
			}
			case CAN_BOARD_ID_5:
			{ 
				get_DAFU_measure(&Big_Fu_info[4]);
				Board_info.B5_offline_cnt = 0;
				break;
			}			
			
			default:
			{
				break;
			}	
	
		}
}

