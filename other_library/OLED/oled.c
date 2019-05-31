#include "oled.h"
void IIC_Start(){
	GPIO_SetBits(GPIOB,GPIO_Pin_6);//时钟线拉高
	 GPIO_SetBits(GPIOB,GPIO_Pin_7);//数据线拉高
	GPIO_ResetBits(GPIOB,GPIO_Pin_7);//数据线拉低
	GPIO_ResetBits(GPIOB,GPIO_Pin_6);//时钟线拉低
}

void IIC_Stop(){
	GPIO_SetBits(GPIOB,GPIO_Pin_6) ;//时钟线拉高
	GPIO_ResetBits(GPIOB,GPIO_Pin_7);//数据线拉低
	 GPIO_SetBits(GPIOB,GPIO_Pin_7);//数据线拉高
}	

void IIC_Wait_Ack(){
	GPIO_SetBits(GPIOB,GPIO_Pin_6);//时钟线拉高，忽略了ACK信号，避免了死锁等bug
	GPIO_ResetBits(GPIOB,GPIO_Pin_6);//时钟线拉低
}

void Write_IIC_Byte(unsigned char IIC_Byte){
    u8 t;  
    GPIO_ResetBits(GPIOB,GPIO_Pin_6);//拉低时钟开始数据传输
    for(t=0;t<8;t++){              
      if(1 == (IIC_Byte&0x80)>>7)
			GPIO_SetBits(GPIOB,GPIO_Pin_7);
		else
			GPIO_ResetBits(GPIOB,GPIO_Pin_7);
    IIC_Byte<<=1;
		GPIO_SetBits(GPIOB,GPIO_Pin_6);
		GPIO_ResetBits(GPIOB,GPIO_Pin_6);	
    }	 
} 	 

void Write_IIC_Command(unsigned char IIC_Command){
   IIC_Start();
   Write_IIC_Byte(0x78);            //Slave address,SA0=0
	IIC_Wait_Ack();	
   Write_IIC_Byte(0x00);			//write command
	IIC_Wait_Ack();	
   Write_IIC_Byte(IIC_Command); 
	IIC_Wait_Ack();	
   IIC_Stop();
}

void Write_IIC_Data(unsigned char IIC_Data){
   IIC_Start();
   Write_IIC_Byte(0x78);			//D/C#=0; R/W#=0
	IIC_Wait_Ack();	
   Write_IIC_Byte(0x40);			//write data
	IIC_Wait_Ack();	
   Write_IIC_Byte(IIC_Data);
	IIC_Wait_Ack();	
   IIC_Stop();
}

void fill_picture(u8 *fill_Data)
{
	u8 m,n;
	for(m=0;m<8;m++){
		Write_IIC_Command(0xb0+m);		//page0-page7
		Write_IIC_Command(0x00);		//low column start address
		Write_IIC_Command(0x10);		//high column start address
		for(n=0;n<128;n++){
				Write_IIC_Data(fill_Data[128*m+n]);
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

