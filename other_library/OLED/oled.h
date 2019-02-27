#ifndef __OLED_H
#define __OLED_H
#include "stm32f10x.h"
#include "Ji2c.h"

void OLED_Display_On(void);
void OLED_Display_Off(void);	   							   		    
void OLED_Init(void);
void OLED_Set_Pos(u8 x, u8 y);
void OLED_Draw(u8 *BMP);
void Write_IIC_Command(u8 IIC_Command);
void Write_IIC_Data(u8 IIC_Data);
#endif  
	 



