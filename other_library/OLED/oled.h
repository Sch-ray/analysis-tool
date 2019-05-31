#include "stm32f10x.h"

#ifndef __OLED_H
#define __OLED_H						  
 		     
#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据


//OLED控制用函数
void IIC_Start(void);
void IIC_Stop(void);
void IIC_Wait_Ack(void);

void Write_IIC_Byte(unsigned char IIC_Byte);
void Write_IIC_Command(unsigned char IIC_Command);
void Write_IIC_Data(unsigned char IIC_Data);
void fill_picture(u8 *fill_Data);;   							   		    
void OLED_Init(void);


#endif  
	 



