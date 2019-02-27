#include "24c64.h"

void delay_ss(u32 nTimer){
	u32 i=0;
	for(i=0;i<nTimer;i++){
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	}
}

u8 AT24CXX_ReadOneByte(u16 ReadAddr)//实际地址为13位
{	u8 temp=0;	
	IIC_Start(); 
	IIC_Send_Byte(0XA0);	   //发送写命令
	IIC_Wait_Ack();
	IIC_Send_Byte(ReadAddr>>8);//发送待读取的地址
	IIC_Wait_Ack();
    IIC_Send_Byte(ReadAddr);
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA1);           //进入接收模式			   
	IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		   
    IIC_Stop();	    
	return temp;
}
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite){				   	  	    																 
    IIC_Start();  
	IIC_Send_Byte(0XA0);	    //发送写命令
	IIC_Wait_Ack();
	IIC_Send_Byte(WriteAddr>>8);//发送高地址
	IIC_Wait_Ack();	   
    IIC_Send_Byte(WriteAddr);   //发送低地址
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite); //发送字节							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//产生一个停止条件 
}
void AT24CXX_WriteLenByte(u16 WriteAddr,u8 *buffer,u8 Len){  	
	u8 t;
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,buffer[t]);
		delay_ss(2000);//这个比较麻烦如果我要写1K数据岂不是要20s。
	}												    
}

void AT24CXX_ReadLenByte(u16 ReadAddr,u8 *buffer,u8 Len){  	
	u8 t;
	for(t=0;t<Len;t++){
		buffer[t]=AT24CXX_ReadOneByte(ReadAddr+t);
		delay_ss(500);
	}											    
}


