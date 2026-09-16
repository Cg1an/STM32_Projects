#include "headfile.h"

#define OLED_ADDR 0x78 // 0x3C << 1

char display_buf[20];
//int16_t encoder_count = 0;
//int16_t physical_step = 0;
// [新增] 运行状态标志位
uint8_t is_running = 0;    // 0: 设置模式(LED灭可以调参), 1: 运行模式(LED亮参数锁死)
// [新增] 步进电机状态机与脉冲计数
volatile uint32_t target_pulses = 0; // 还需要发送的脉冲数量
uint8_t motor_status =
    0;            // 0: 停止, 1: 正在转动, 2: 正在等待间隔时间 4：电机半路被叫停，正在执行退回坐标 0 的任务
uint32_t wait_start_tick = 0;        // 记录开始等待的时间戳
// [新增] 状态机与数值变量
uint8_t current_line = 1;  // 当前选中行：1 代表第一行，2 代表第二行
uint16_t val_line1 = 0;         // 第一行存储的数据 (比如速度)
uint16_t val_line2 = 0;         // 第二行存储的数据 (比如间隔时间)
uint16_t val_line3 = 0;
uint16_t val_line4 = 0;
uint16_t val_line5 = 1; // [新增] 第5个参数：步长倍率（默认必须是1）
// [新增] TIM3 专用的秒数倒计时
volatile uint32_t remaining_wait_ms = 0;
//volatile uint32_t remaining_wait_sec = 0;
volatile uint32_t remaining_total_ms = 0;
//volatile uint32_t remaining_total_sec = 0;//总时间
//记录电机的绝对物理位置 (脉冲为单位)。原点就是 0。
volatile int32_t absolute_position = 0;
// 记录当前电机的运行方向
uint8_t current_dir = GPIO_PIN_SET;
// 定义我们要使用的 Flash 地址 (选取 64K Flash 的最后一页 0x0800FC00，极其安全)
#define FLASH_SAVE_ADDR  0x0800FC00
#define FLASH_MAGIC_WORD 0x55AA      // 魔法数字，用来判断是不是第一次上电

// ==========================================
// 掉电记忆：保存参数到 Flash
// ==========================================
void Save_Parameters(void)
{
    HAL_FLASH_Unlock(); // 解锁 Flash

    // 1. 擦除这一页 (Flash 写入前必须先擦除)
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t PageError = 0;
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = FLASH_SAVE_ADDR;
    EraseInitStruct.NbPages = 1;
    HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);

    // 2. 写入数据 (每次写半字 HalfWord，即 16位)
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, FLASH_SAVE_ADDR,      val_line1);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, FLASH_SAVE_ADDR + 2,  val_line2);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, FLASH_SAVE_ADDR + 4,  val_line3);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, FLASH_SAVE_ADDR + 6,  val_line4);
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, FLASH_SAVE_ADDR + 8,  val_line5);
    // 3. 写入魔法数字，证明这块地盘我们存过数据了
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, FLASH_SAVE_ADDR + 10,  FLASH_MAGIC_WORD);

    HAL_FLASH_Lock(); // 重新上锁保护
}

// ==========================================
// 掉电记忆：从 Flash 读取参数
// ==========================================
void Load_Parameters(void)
{
    // 先读一下魔法数字
    uint16_t magic = *(__IO uint16_t*)(FLASH_SAVE_ADDR + 10);
    
    if (magic == FLASH_MAGIC_WORD) 
    {
        // 之前存过，直接读取赋值
        val_line1 = *(__IO uint16_t*)(FLASH_SAVE_ADDR);
        val_line2 = *(__IO uint16_t*)(FLASH_SAVE_ADDR + 2);
        val_line3 = *(__IO uint16_t*)(FLASH_SAVE_ADDR + 4);
				val_line4 = *(__IO uint16_t*)(FLASH_SAVE_ADDR + 6);
				val_line5 = *(__IO uint16_t*)(FLASH_SAVE_ADDR + 8);
			if(val_line5 == 0) 
        {
            val_line5 = 1;
        }
			
    }
    else 
    {
        // 第一次使用这个单片机，里面全是乱码，给个默认安全值
        val_line1 = 0;
        val_line2 = 0;
        val_line3 = 0;
				val_line4 = 0;
				val_line5 = 1;
    }
}
/* 8x16 ASCII 字库：空格, 负号(-), 数字 0-9, 以及新加的光标 > */
/* 8x16 ASCII 字库：包含原有字符及新增的 Q,a,e,f,k,l,n,o,r,t,u,w,_ */
const uint8_t F8x16[][16] =
{
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 0: 空格
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01}, // 1: - (负号)
    {0x00, 0xE0, 0x10, 0x08, 0x08, 0x10, 0xE0, 0x00, 0x00, 0x0F, 0x10, 0x20, 0x20, 0x10, 0x0F, 0x00}, // 2: 0
    {0x00, 0x00, 0x10, 0x10, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x20, 0x3F, 0x20, 0x20, 0x00}, // 3: 1
    {0x00, 0x70, 0x08, 0x08, 0x08, 0x08, 0xF0, 0x00, 0x00, 0x30, 0x28, 0x24, 0x22, 0x21, 0x30, 0x00}, // 4: 2
    {0x00, 0x30, 0x08, 0x08, 0x08, 0x88, 0x70, 0x00, 0x00, 0x18, 0x20, 0x21, 0x21, 0x22, 0x1C, 0x00}, // 5: 3
    {0x00, 0x00, 0x80, 0x40, 0x30, 0xF8, 0x00, 0x00, 0x00, 0x06, 0x05, 0x24, 0x24, 0x3F, 0x24, 0x24}, // 6: 4
    {0x00, 0xF8, 0x88, 0x88, 0x88, 0x08, 0x08, 0x00, 0x00, 0x19, 0x20, 0x20, 0x20, 0x11, 0x0E, 0x00}, // 7: 5
    {0x00, 0xE0, 0x10, 0x88, 0x88, 0x90, 0x00, 0x00, 0x00, 0x0F, 0x11, 0x20, 0x20, 0x20, 0x1F, 0x00}, // 8: 6
    {0x00, 0x18, 0x08, 0x08, 0x88, 0x68, 0x18, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x01, 0x00, 0x00, 0x00}, // 9: 7
    {0x00, 0x70, 0x88, 0x08, 0x08, 0x88, 0x70, 0x00, 0x00, 0x1C, 0x22, 0x21, 0x21, 0x22, 0x1C, 0x00}, // 10: 8
    {0x00, 0xF0, 0x08, 0x08, 0x08, 0x10, 0xE0, 0x00, 0x00, 0x01, 0x12, 0x22, 0x22, 0x11, 0x0F, 0x00}, // 11: 9
    {0x00, 0x08, 0x14, 0x22, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 12: > (箭头光标)
    {0x00, 0x00, 0x00, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x00}, // 13: 冒号 (:)
    {0x08, 0x78, 0x80, 0x00, 0x00, 0x80, 0x78, 0x08, 0x00, 0x00, 0x07, 0x38, 0x38, 0x07, 0x00, 0x00}, // 14: 字母 V/v
    {0x00, 0x02, 0x02, 0xFE, 0xFE, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x7F, 0x00, 0x00, 0x00}, // 15: 字母 T (大写)
    // ---- 以下为新增字符 ----
    {0x00, 0xF8, 0x08, 0x08, 0x08, 0x08, 0xF8, 0x00, 0x00, 0x0F, 0x20, 0x20, 0x20, 0x10, 0x3F, 0x20}, // 16: Q
    {0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x19, 0x24, 0x24, 0x24, 0x14, 0x3F, 0x00}, // 17: a
    {0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x24, 0x24, 0x24, 0x24, 0x17, 0x00}, // 18: e
    {0x00, 0x80, 0x80, 0xE0, 0x90, 0x90, 0x20, 0x00, 0x00, 0x20, 0x3F, 0x20, 0x20, 0x00, 0x00, 0x00}, // 19: f
    {0x00, 0xFE, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x04, 0x02, 0x09, 0x14, 0x22, 0x00}, // 20: k
    {0x00, 0x00, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x20, 0x00, 0x00, 0x00, 0x00}, // 21: l (小写L)
    {0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x3F, 0x02, 0x01, 0x01, 0x3E, 0x00, 0x00}, // 22: n
    {0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x1F, 0x20, 0x20, 0x20, 0x1F, 0x00, 0x00}, // 23: o
    {0x00, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00}, // 24: r
    {0x00, 0x00, 0x80, 0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x20, 0x20, 0x00, 0x00, 0x00}, // 25: t (小写t)
    {0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x1F, 0x20, 0x20, 0x10, 0x3F, 0x00, 0x00}, // 26: u
    {0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x0F, 0x30, 0x0C, 0x30, 0x0F, 0x00, 0x00}, // 27: w
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},  // 28: _ (下划线)
		{0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x1F, 0x20, 0x20, 0x20, 0x7F, 0x00, 0x00}  // 29: q (小写字母 q)
};


void OLED_WriteCommand(uint8_t cmd)
{
    HAL_I2C_Mem_Write(&hi2c1, OLED_ADDR, 0x00, I2C_MEMADD_SIZE_8BIT, &cmd, 1, HAL_MAX_DELAY);
}

void OLED_Init(void)
{
    HAL_Delay(100);
    OLED_WriteCommand(0xAE);
    OLED_WriteCommand(0xD5);
    OLED_WriteCommand(0x80);
    OLED_WriteCommand(0xA8);
    OLED_WriteCommand(0x3F);
    OLED_WriteCommand(0xD3);
    OLED_WriteCommand(0x00);
    OLED_WriteCommand(0x40);
    OLED_WriteCommand(0x8D);
    OLED_WriteCommand(0x14);
    OLED_WriteCommand(0x20);
    OLED_WriteCommand(0x00);
    OLED_WriteCommand(0xA1);
    OLED_WriteCommand(0xC8);
    OLED_WriteCommand(0xDA);
    OLED_WriteCommand(0x12);
    OLED_WriteCommand(0x81);
    OLED_WriteCommand(0xCF);
    OLED_WriteCommand(0xAF);
}

void OLED_Fill(uint8_t data)
{
    uint8_t i, j;

    for (i = 0; i < 8; i++)
    {
        OLED_WriteCommand(0xB0 + i);
        OLED_WriteCommand(0x00);
        OLED_WriteCommand(0x10);

        for (j = 0; j < 128; j++)
        {
            HAL_I2C_Mem_Write(&hi2c1, OLED_ADDR, 0x40, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);
        }
    }
}

void OLED_SetCursor(uint8_t x, uint8_t y)
{
    OLED_WriteCommand(0xB0 + y);
    OLED_WriteCommand(((x & 0xF0) >> 4) | 0x10);
    OLED_WriteCommand(x & 0x0F);
}

void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr)
{
    uint8_t c_index;

    // 增加对光标 '>' 的解析
    if (chr == ' ')
    {
        c_index = 0;
    }
    else if (chr == '-')
    {
        c_index = 1;
    }
    else if (chr == '>')
    {
        c_index = 12;
    }
    else if (chr >= '0' && chr <= '9')
    {
        c_index = chr - '0' + 2;
    }
    else if (chr == 'v' || chr == 'V')
    {
        c_index = 14;    // [新增] 识别字母 v 和 V
    }
    else if (chr == 'T')
    {
        c_index = 15;    // [新增] 识别字母 v 和 V
    }
    else if (chr == ':')
    {
        c_index = 13;    // [新增] 识别字母 v 和 V
    }
    else if (chr == 'Q') c_index = 16;
    else if (chr == 'a') c_index = 17;
    else if (chr == 'e') c_index = 18;
    else if (chr == 'f') c_index = 19;
    else if (chr == 'k') c_index = 20;
    else if (chr == 'l') c_index = 21;
    else if (chr == 'n') c_index = 22;
    else if (chr == 'o') c_index = 23;
    else if (chr == 'r') c_index = 24;
    else if (chr == 't') c_index = 25; // 这里独立出小写 t，因为 T_total 用到了小写
    else if (chr == 'u') c_index = 26;
    else if (chr == 'w') c_index = 27;
    else if (chr == '_') c_index = 28;
		else if (chr == 'q') c_index = 29;
    else return; // 遇到没存进字库的字符，直接返回防止死机报错

    OLED_SetCursor(x, y);
    //    for (i = 0; i < 8; i++)
    //    {
    //        HAL_I2C_Mem_Write(&hi2c1, OLED_ADDR, 0x40, I2C_MEMADD_SIZE_8BIT, (uint8_t *)&F8x16[c_index][i], 1, HAL_MAX_DELAY);
    //    }
    HAL_I2C_Mem_Write(&hi2c1, OLED_ADDR, 0x40, I2C_MEMADD_SIZE_8BIT, (uint8_t *)&F8x16[c_index][0], 8, HAL_MAX_DELAY);
    OLED_SetCursor(x, y + 1);
    HAL_I2C_Mem_Write(&hi2c1, OLED_ADDR, 0x40, I2C_MEMADD_SIZE_8BIT, (uint8_t *)&F8x16[c_index][8], 8, HAL_MAX_DELAY);
    //    for (i = 0; i < 8; i++)
    //    {
    //        HAL_I2C_Mem_Write(&hi2c1, OLED_ADDR, 0x40, I2C_MEMADD_SIZE_8BIT, (uint8_t *)&F8x16[c_index][i + 8], 1, HAL_MAX_DELAY);
    //    }
}

void OLED_ShowString(uint8_t x, uint8_t y, char *str)
{
    while (*str)
    {
        OLED_ShowChar(x, y, *str);
        x += 8;

        if (x > 120)
        {
            x = 0;
            y += 2;
        }

        str++;
    }
}

void motor_start(uint8_t fangxiang, uint16_t val_line1_sd, uint16_t val_line2_t)
{
    //		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); // 注意：这里多了 _IT ！
    //    __HAL_TIM_ENABLE_IT(&htim1, TIM_IT_UPDATE);
    if (val_line2_t == 0)
    {
        return;
    }

    target_pulses = val_line1_sd * 800;             // 总共要发的脉冲数
    uint32_t freq = target_pulses / val_line2_t;   // 需要的频率 (Hz)
    uint32_t arr = (1000000 / freq) - 1;         // 算出定时器的重装载值
    current_dir = fangxiang;
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, fangxiang);
    __HAL_TIM_SetAutoreload(&htim1, arr);		//设置速度快慢
    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1, (arr / 2)); //设置脉冲  用不到
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); // 注意：这里多了 _IT ！
    __HAL_TIM_ENABLE_IT(&htim1, TIM_IT_UPDATE);
}
void motor_stop(void)
{
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1); // 注意：这里多了 _IT ！
    __HAL_TIM_DISABLE_IT(&htim1, TIM_IT_UPDATE);
}

////返回原点的函数
//void motor_go_home(void)
//{
//    if (absolute_position == 0 || val_line2 == 0)
//    {
//        return;
//    }

//    uint32_t pulses_needed;
//    uint8_t dir;

//    // 判断该正转还是反转回城
//    if (absolute_position > 0)
//    {
//			if(absolute_position>800)
//			{pulses_needed = 800 - absolute_position % 800;

//        if (pulses_needed >= 400)
//        {
//            dir = GPIO_PIN_SET;
//        }

//        if (pulses_needed < 400)
//        {
//            dir = GPIO_PIN_RESET;
//        }
//			}
//			else if(absolute_position<800)
//			{
//				if(absolute_position>=400)
//				{
//				pulses_needed = 800-absolute_position;
//				dir = GPIO_PIN_SET;
//				}
//				else{
//					pulses_needed = absolute_position;
//				dir = GPIO_PIN_RESET;
//				}
//			}
//    }
//    else
//    {
//			if((-absolute_position)>800)
//			{
//        pulses_needed = 800 - ((-absolute_position) % 800);

//        if (pulses_needed >= 400)
//        {
//            dir = GPIO_PIN_RESET;
//        }

//        if (pulses_needed < 400)
//        {
//            dir = GPIO_PIN_SET;
//        }
//			}
//			else if((-absolute_position)<800)
//			{
//				if((-absolute_position)>400)
//				{
//					pulses_needed = 800+absolute_position;
//					dir = GPIO_PIN_RESET;
//				}
//				else if((-absolute_position)<400)
//				{
//					pulses_needed = -absolute_position;
//					dir = GPIO_PIN_SET;
//				}
//					
//			}
//    }

//    // 复用屏幕上 T1 的速度设定来回城
//    uint32_t freq = (val_line1 * 800) / val_line2;
//    uint32_t arr = (1000000 / freq) - 1;
//    current_dir = dir; // 记录方向
//    target_pulses = pulses_needed; // 填入精准的剩余脉冲数
//    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, dir);
//    __HAL_TIM_SetAutoreload(&htim1, arr);
//    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1, (arr / 2));
//    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
//    __HAL_TIM_ENABLE_IT(&htim1, TIM_IT_UPDATE);
//}

// 智能就近回零函数（最短路径版）
void motor_go_home(void)
{
    // 获取当前绝对位置的“纯正数大小” (消除负号干扰，方便计算)
    uint32_t current_abs = (absolute_position > 0) ? absolute_position : -absolute_position;
    
    // 计算当前位置距离上一个“整圈（0度）”多出了多少步
    uint32_t offset = current_abs % 800; 

    // 如果 offset 刚好是 0，说明它现在就停在某个整圈上，不用动了！
    if (offset == 0)
		{	
				motor_stop();
				is_running = 0;   // 解锁屏幕
        motor_status = 0; // 解除回城状态
        absolute_position = 0; // 顺手把坐标归零，防止几年后溢出
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET); // 灭灯
			
				HAL_TIM_Base_Stop_IT(&htim3);
			return; 
		}

    uint32_t pulses_needed;
    uint8_t dir;

    // 核心最短路径逻辑：
    if (offset <= 400) 
    {
        // 偏移量小于等于半圈：说明离“刚才走过的那个 0 点”更近。应该退回去。
        pulses_needed = offset;
        // 如果是在正半轴，退回去就是反转(RESET)；如果是负半轴，退回去就是正转(SET)
        dir = (absolute_position > 0) ? GPIO_PIN_RESET : GPIO_PIN_SET; 
    } 
    else 
    {
        // 偏移量大于半圈：说明离“下一个 0 点”更近。应该继续往前走补齐。
        pulses_needed = 800 - offset;
        // 如果是在正半轴，往前走就是正转(SET)；如果是负半轴，往前走就是反转(RESET)
        dir = (absolute_position > 0) ? GPIO_PIN_SET : GPIO_PIN_RESET; 
    }

    // 【安全修复】：坚决不用屏幕上的速度，给定一个固定的安全回城速度 (比如 1000Hz)
    uint32_t freq = 1000; 
    uint32_t arr = (1000000 / freq) - 1;

    current_dir = dir;             // 记录方向，告诉底层中断
    target_pulses = pulses_needed; // 装填精确计算好的脉冲数

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, dir);
    __HAL_TIM_SetAutoreload(&htim1, arr);		
    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1, (arr / 2)); 
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); 
    __HAL_TIM_ENABLE_IT(&htim1, TIM_IT_UPDATE);
}
void main_pro(void)
{
    // 1. 读取底层数据，记得强制转换！(你之前漏了 int16_t)
    if (is_running == 0)
    {
        int16_t encoder_count = __HAL_TIM_GET_COUNTER(&htim2);

//        if (encoder_count <= 0)
//        {
//            encoder_count = 0;
//            __HAL_TIM_SET_COUNTER(&htim2, 0);
//        }

        int16_t physical_step = encoder_count / 4;

//        if (current_line == 1) val_line1 = physical_step * val_line5;
//        else if (current_line == 2) val_line2 = physical_step * val_line5;
//        else if(current_line == 3) val_line3 = physical_step * val_line5;
//        else if(current_line == 4) val_line4 = physical_step * val_line5;
//        else if(current_line == 5)
//        {
//            // 调节步长本身时，它的步长永远是 1！且最小只能是 1！
//            if (physical_step <= 0) 
//            {
//                physical_step = 1;
//                __HAL_TIM_SET_COUNTER(&htim2, 4); // 强行拉回1格
//            }
//            val_line5 = physical_step;
//        }
				if (physical_step != 0) 
        {
            // 【极其重要】：清除已经用掉的格数，只保留余数（防止转太快丢失脉冲）
            __HAL_TIM_SET_COUNTER(&htim2, encoder_count % 4);

            int32_t temp = 0; // 用 32位 暂存，防止减到负数时越界变成 65535

//            // 把增量乘以倍率，加到当前行的参数里
//            if (current_line == 1) {
//                temp = (int32_t)val_line1 + (physical_step * val_line5);
//                val_line1 = (temp < 0) ? 0 : temp; // 限制最小值为 0
//            }
//            else if (current_line == 2) {
//                temp = (int32_t)val_line2 + (physical_step * val_line5);
//                val_line2 = (temp < 0) ? 0 : temp;
//            }
//            else if (current_line == 3) {
//                temp = (int32_t)val_line3 + (physical_step * val_line5);
//                val_line3 = (temp < 0) ? 0 : temp;
//            }
//            else if (current_line == 4) {
//                temp = (int32_t)val_line4 + (physical_step * val_line5);
//                val_line4 = (temp < 0) ? 0 : temp;
//            }
//            else if (current_line == 5) {
//                // 注意：调节倍率自己时，步长永远是 1！否则它自己乘自己会疯掉！
//                temp = (int32_t)val_line5 + physical_step;
//                val_line5 = (temp <= 0) ? 1 : temp; // 倍率最小只能是 1，坚决不能是 0
//            }
					
						// 把增量乘以倍率，加到当前行的参数里
            if (current_line == 1) {
                temp = (int32_t)val_line1 + (physical_step * val_line5);
                val_line1 = (temp < 0) ? 0 : ((temp > 65535) ? 65535 : temp); // 双向限幅
            }
            else if (current_line == 2) {
                temp = (int32_t)val_line2 + (physical_step * val_line5);
                val_line2 = (temp < 0) ? 0 : ((temp > 65535) ? 65535 : temp);
            }
            else if (current_line == 3) {
                temp = (int32_t)val_line3 + (physical_step * val_line5);
                val_line3 = (temp < 0) ? 0 : ((temp > 65535) ? 65535 : temp);
            }
            else if (current_line == 4) {
                temp = (int32_t)val_line4 + (physical_step * val_line5);
                val_line4 = (temp < 0) ? 0 : ((temp > 65535) ? 65535 : temp);
            }
            else if (current_line == 5) {
                temp = (int32_t)val_line5 + physical_step;
                val_line5 = (temp <= 0) ? 1 : ((temp > 10000) ? 10000 : temp); // 建议把倍率上限设为10000，够用了
            }
        }

    }

//    //第一行显示
//    char cursor1 = (current_line == 1) ? '>' : ' ';
//    sprintf(display_buf, "quan :%5d  %c", val_line1, cursor1);
//    OLED_ShowString(16, 0, display_buf);
//    //第二行显示
//    char cursor2 = (current_line == 2) ? '>' : ' ';
//    sprintf(display_buf, "work :%5d  %c", val_line2, cursor2);
//    OLED_ShowString(16, 2, display_buf);
//    //第二行显示
//    char cursor3 = (current_line == 3) ? '>' : ' ';
//    sprintf(display_buf, "free :%5d  %c", val_line3, cursor3);
//    OLED_ShowString(16, 4, display_buf);
//		
//		char cursor4 = (current_line == 4) ? '>' : ' ';
//    sprintf(display_buf, "total:%5d  %c", val_line4, cursor4);
//    OLED_ShowString(16, 6, display_buf);
		// ==========================================
    // 动态滚屏渲染逻辑 (滑动窗口)
    // ==========================================
    // 如果光标在 1~4 行，偏移量为 0（屏幕显示 1,2,3,4）
    // 如果光标在第 5 行，偏移量为 1（画面上移，屏幕显示 2,3,4,5）
    uint8_t offset = (current_line <= 4) ? 0 : 1; 

    // 渲染屏幕物理第 1 行 (y=0)
    char cursorA = (current_line == 1 + offset) ? '>' : ' ';
    if (offset == 0) sprintf(display_buf, "quan :%5d  %c", val_line1, cursorA);
    else             sprintf(display_buf, "work :%5d  %c", val_line2, cursorA);
    OLED_ShowString(16, 0, display_buf);

    // 渲染屏幕物理第 2 行 (y=2)
    char cursorB = (current_line == 2 + offset) ? '>' : ' ';
    if (offset == 0) sprintf(display_buf, "work :%5d  %c", val_line2, cursorB);
    else             sprintf(display_buf, "free :%5d  %c", val_line3, cursorB);
    OLED_ShowString(16, 2, display_buf);

    // 渲染屏幕物理第 3 行 (y=4)
    char cursorC = (current_line == 3 + offset) ? '>' : ' ';
    if (offset == 0) sprintf(display_buf, "free :%5d  %c", val_line3, cursorC);
    else             sprintf(display_buf, "total:%5d  %c", val_line4, cursorC);
    OLED_ShowString(16, 4, display_buf);
    
    // 渲染屏幕物理第 4 行 (y=6)
    char cursorD = (current_line == 4 + offset) ? '>' : ' ';
    if (offset == 0) sprintf(display_buf, "total:%5d  %c", val_line4, cursorD);
    else             sprintf(display_buf, "rate :%5d  %c", val_line5, cursorD); // 用 rate 代表倍率/步长
    OLED_ShowString(16, 6, display_buf);
		
		
    HAL_Delay(50);
}

//外部中断回调函数：处理按钮按下逻辑
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    // 静态变量用于记录上次触发的时间，实现防抖
    static uint32_t last_trigger_time_PA2 = 0;
    static uint32_t last_trigger_time_PA15 = 0;
    uint32_t current_time = HAL_GetTick();

    if (is_running == 0)
    {
        if (GPIO_Pin == GPIO_PIN_2) // 确认是 PA2 引脚触发
        {
            // 软件防抖：两次按下的间隔必须大于 200 毫秒才认为是有效按键
            if (current_time - last_trigger_time_PA2 > 200)
            {
                last_trigger_time_PA2 = current_time; // 更新触发时间

                // 核心逻辑：切换菜单，并【篡改底层定时器】防止数值跳跃
                if (current_line == 1)
                {
                    current_line = 2; // 切换到第二行
                    // 强制让定时器底层数值变成第二行之前保存的数据 (乘以4是因为4倍频)
                    //__HAL_TIM_SET_COUNTER(&htim2, val_line2/val_line5 * 4);
                }
                else if (current_line == 2)
                {
                    current_line = 3; // 切换回第一行
                    // 同理，同步回第一行的数据
                    //__HAL_TIM_SET_COUNTER(&htim2, val_line3/val_line5 * 4);
                }
								else if(current_line == 3)
								{
										current_line = 4; 
										//__HAL_TIM_SET_COUNTER(&htim2, val_line4/val_line5 * 4);
								}
								else if (current_line == 4)
                {
                    current_line = 5; // 【新增】切换到第五行，触发滚屏！
                   // __HAL_TIM_SET_COUNTER(&htim2, val_line5/val_line5 * 4);
                }
                else
                {
                    current_line = 1; // 切换回第一行
                    // 同理，同步回第一行的数据
                    //__HAL_TIM_SET_COUNTER(&htim2, val_line1/val_line5 * 4);
									OLED_Fill(0x00); // 滚屏复位时必须清屏！
									
                }
            }
        }
    }

    // ==========================================
    // 按键 2：PA15 (新增的 运行/设置 切换按键)
    // ==========================================
    if (GPIO_Pin == GPIO_PIN_15)
    {
        if (current_time - last_trigger_time_PA15 > 200)
        {
						
					 if (current_time < 2000) 
            {
                return; // 直接退出，什么都不做
            }
						
						if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15) != GPIO_PIN_RESET) 
            {
                // 如果引脚不是真实的按下状态，说明是断电杂波产生的幽灵中断，立刻忽略！
                return; 
            }
            last_trigger_time_PA15 = current_time;
            // 状态翻转：0变1，1变0
            is_running = !is_running;

            if (is_running == 1)//电机运行模式
            {
                // 1. 进入运行模式，点亮 LED (假设是高电平亮，根据你 LED 的接法可能要换成 RESET)
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
								
								Save_Parameters();//保存参数

                // 【修复逻辑缺陷】：智能判断是不是停在半路了？
                if (absolute_position % 800 != 0)
                {
                    // 不在原点！(肯定是半路被急停了)，先乖乖回家！
                    motor_status = 4; // 标记状态 4：紧急回城中
                    motor_go_home();  // 调用回城函数！
                }
								else
								{
                // 启动步进电机 (TB6600)
                // 安全检查：防止除以0或参数为0
								// 在整圈上！(可能是全新的开始，也可能是刚好跑完了几圈停下)
								absolute_position = 0;
//                if (val_line1 > 0 && val_line2 > 0)
//                {	
//										// 【新增核心逻辑】：启动瞬间，装填总时间并强行启动 TIM3！
//                    if (val_line4 > 0) 
//                    {
//                        remaining_total_ms = val_line4*1000;
//                        HAL_TIM_Base_Start_IT(&htim3); 
//                    }
//                    motor_start(up, val_line1, val_line2);
//                    motor_status = 1; // 标记为正在转动
//                }
								// 修改后的逻辑：先判定是否有休息时间，优先进入倒计时
								if (val_line1 > 0 && val_line2 > 0)
								{	
										// 1. 无论如何，先装填全局总时间（死神闹钟）
										if (val_line4 > 0) 
										{
												remaining_total_ms = val_line4 * 1000;
										}

										// 2. 核心修改：判断有没有设置休息时间 (val_line3 / free)
										if (val_line3 > 0)
										{
												// 如果有休息时间，先不转，直接进入等待状态！
												remaining_wait_ms = val_line3 * 1000;
												motor_status = 2;              // 状态 2 表示正在等待
												HAL_TIM_Base_Start_IT(&htim3); // 点火 TIM3 开始倒计时
										}
										else
										{
												// 如果用户没设休息时间 (free == 0)，那就按老规矩直接开转
												if (val_line4 > 0) 
												{
														HAL_TIM_Base_Start_IT(&htim3); // 只有在需要算总时间时才开 TIM3
												}
												motor_start(up, val_line1, val_line2);
												motor_status = 1; // 状态 1 表示正在转动
										}
								}
								else
								{
										// 【修复炸弹 2】：如果用户把参数设成了 0，拒绝启动，并立刻解锁屏幕！
                    is_running = 0; 
                    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
								}
							}
            }
            else// 退出运行模式，熄灭 LED
            {
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
                motor_stop();
                HAL_TIM_Base_Stop_IT(&htim3);//关掉计时功能
                motor_status = 0; // 回到停止状态
								__HAL_TIM_SET_COUNTER(&htim2, 0);
//                if (current_line == 1)
//                {
//                    __HAL_TIM_SET_COUNTER(&htim2, val_line1 * 4);
//                }
//                else if (current_line == 2)
//                {
//                    __HAL_TIM_SET_COUNTER(&htim2, val_line2 * 4);
//                }
//								else if (current_line == 3)
//                {
//                    __HAL_TIM_SET_COUNTER(&htim2, val_line3 * 4);
//                }
//								else if (current_line == 4)
//                {
//                    __HAL_TIM_SET_COUNTER(&htim2, val_line4 * 4);
//                }
//                else
//                {
//                    __HAL_TIM_SET_COUNTER(&htim2, val_line5 * 4);
//                }
            }
        }
    }

    //复位
    if (GPIO_Pin == GPIO_PIN_4)
    {
        static uint32_t last_trigger_time_PA4 = 0;

        if (current_time - last_trigger_time_PA4 > 200)
        {
            last_trigger_time_PA4 = current_time;

            // 【安全拦截逻辑】：只有在“待机状态”且“确实不在原点”时，按这个键才有效！
            // 如果机器正在全速跑循环，按这个键会被直接忽略，防止误触发生危险。
            if (is_running == 0 && (absolute_position %800 != 0))
            {
                is_running = 1; // 第一步：立刻锁死屏幕和旋钮，防止用户在回城路上乱调参数
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET); // 亮起指示灯，表示“机器忙碌”
                motor_status = 4; // 第二步：将状态机切入咱们之前写好的“状态 4：回城中”
                motor_go_home();  // 第三步：直接调用底层的回城发脉冲函数
            }
        }
    }
}
// 定时器周期中断回调函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM1)
    {
        if (target_pulses > 0)
        {
            target_pulses--;

            // 【神来之笔：记录绝对坐标】
            if (current_dir == GPIO_PIN_SET)
            {
                absolute_position++;    // 正转，坐标 +1
            }
            else
            {
                absolute_position--;    // 反转，坐标 -1
            }

            if (target_pulses == 0)
            {
                // 脉冲发完了，精准停机！两步走：
                motor_stop();

                if (motor_status == 4)
                {
                    // 状态 4 代表刚才是在“紧急回原点”，现在终于到家了
                    is_running = 0;   // 解锁屏幕
                    motor_status = 0; // 回到待机状态
                    absolute_position = 0; // 强行校准为绝对 0 点防误差
                    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET); // 灭灯
									
										HAL_TIM_Base_Stop_IT(&htim3);
                }
                else if (val_line3 > 0)
                {
                    motor_status = 2; // 进入等待状态
                    remaining_wait_ms = val_line3*1000; // 把 T2 的秒数装填给定时器
                    // 【点火启动 TIM3！】开始 1ms 1次 的精确倒计时
                    HAL_TIM_Base_Start_IT(&htim3);
                }
                else
                {
                    // 用户把 T2 设为了 0，不循环，直接收工
                    is_running = 0;
                    motor_status = 0;
                    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET); // 灭灯
										HAL_TIM_Base_Stop_IT(&htim3);
                }
            }
        }
    }
    else if (htim->Instance == TIM3)
    {
				// ===============================================
        // 任务 1：全局总时间倒计时 (死神闹钟，优先级最高)
        // ===============================================
        if (is_running == 1 && val_line4 > 0 && remaining_total_ms > 0)
        {
            remaining_total_ms--;
            
            if (remaining_total_ms == 0)
            {
                // 总时间到了！不管电机在休息还是在狂飙，立刻强制回家！
                motor_status = 4;
                motor_go_home();
                return; // 触发回城后，直接退出，不要再执行下面的间歇休息逻辑了！
            }
        }
        if (motor_status == 2 && remaining_wait_ms > 0)
        {
            remaining_wait_ms--; // 每次进中断，说明过去了 1 ms

            if (remaining_wait_ms == 0)
            {
                // 时间到了！
                if (val_line4 == 0)
                {
                    HAL_TIM_Base_Stop_IT(&htim3);
                }
                motor_start((GPIO_PinState)up, val_line1, val_line2);
                motor_status = 1; // 标记为正在转动
            }
        }
    }
}
