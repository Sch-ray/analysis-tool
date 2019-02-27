#include "oled.h"

void Write_IIC_Command(u8 IIC_Command){
   IIC_Start();
   IIC_Send_Byte(0x78);            //地址+0写
	IIC_Nowait_Ack();	                //不管他
   IIC_Send_Byte(0x00);			//命令
	IIC_Nowait_Ack();	
   IIC_Send_Byte(IIC_Command);     //发送数据
	IIC_Nowait_Ack();	
   IIC_Stop();
}

void Write_IIC_Data(u8 IIC_Data)//没有延时但是每次都重新产生开始结束信号，理论上最快速度只有3.4Mbps啊~
{
   IIC_Start();
   IIC_Send_Byte(0x78);			//D/C#=0; R/W#=0
	IIC_Nowait_Ack();	
   IIC_Send_Byte(0x40);			//write data
	IIC_Nowait_Ack();	
   IIC_Send_Byte(IIC_Data);
	IIC_Nowait_Ack();	
   IIC_Stop();
}

//坐标设置
void OLED_Set_Pos(u8 x, u8 y) 
{ 	Write_IIC_Command(0xb0+y);
	Write_IIC_Command(((x&0xf0)>>4)|0x10);
	Write_IIC_Command((x&0x0f)); 
}   	  
//开启OLED显示    
void OLED_Display_On(void)
{
	Write_IIC_Command(0X8D);  //SET DCDC命令
	Write_IIC_Command(0X14);  //DCDC ON
	Write_IIC_Command(0XAF);  //DISPLAY ON
}
//关闭OLED显示     
void OLED_Display_Off(void)
{
	Write_IIC_Command(0X8D);  //SET DCDC命令
	Write_IIC_Command(0X10);  //DCDC OFF
	Write_IIC_Command(0XAE);  //DISPLAY OFF
}		   			 

void OLED_Draw(u8 *BMP){
	u8 m,n;
	for(m=0;m<8;m++){
		Write_IIC_Command(0xb0+m);		//page0-page7
		Write_IIC_Command(0x00);		//low column start address
		Write_IIC_Command(0x10);		//high column start address
		for(n=0;n<128;n++){
				Write_IIC_Data(BMP[128*m+n]);
			}
	}
} 

//初始化SSD1306					    
void OLED_Init(void){
	Write_IIC_Command(0xAE);//--display off
	Write_IIC_Command(0x00);//---set low column address
	Write_IIC_Command(0x10);//---set high column address
	Write_IIC_Command(0x40);//--set start line address  
	Write_IIC_Command(0xB0);//--set page address
	Write_IIC_Command(0x81); // contract control
	Write_IIC_Command(0xFF);//--128   
	Write_IIC_Command(0xA1);//set segment remap 
	Write_IIC_Command(0xA6);//--normal / reverse
	Write_IIC_Command(0xA8);//--set multiplex ratio(1 to 64)
	Write_IIC_Command(0x3F);//--1/32 duty
	Write_IIC_Command(0xC8);//Com scan direction
	Write_IIC_Command(0xD3);//-set display offset
	Write_IIC_Command(0x00);//
	
	Write_IIC_Command(0xD5);//set osc division
	Write_IIC_Command(0x80);//
	
	Write_IIC_Command(0xD8);//set area color mode off
	Write_IIC_Command(0x05);//
	
	Write_IIC_Command(0xD9);//Set Pre-Charge Period
	Write_IIC_Command(0xF1);//
	
	Write_IIC_Command(0xDA);//set com pin configuartion
	Write_IIC_Command(0x12);//
	
	Write_IIC_Command(0xDB);//set Vcomh
	Write_IIC_Command(0x30);//
	
	Write_IIC_Command(0x8D);//set charge pump enable
	Write_IIC_Command(0x14);//
	
	Write_IIC_Command(0xAF);//--turn on oled panel
}  





























