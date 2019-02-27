#include "Ji2c.h"

void delay_inside(u32 nTimer){
	u32 i=0;
	for(i=0;i<nTimer;i++){
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	}
}

void SDA_IN(){
	GPIO_InitTypeDef gpio_init;
	gpio_init.GPIO_Pin = GPIO_Pin_11;
	gpio_init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpio_init);
}
void SDA_OUT(){
	GPIO_InitTypeDef gpio_init;
	gpio_init.GPIO_Pin = GPIO_Pin_11;
	gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpio_init);
}

//产生IIC起始信号
void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	GPIO_SetBits(GPIOB,GPIO_Pin_11);	  	  
	GPIO_SetBits(GPIOB,GPIO_Pin_10);
 	GPIO_ResetBits(GPIOB,GPIO_Pin_11);//START:when CLK is high,DATA change form high to low 
	GPIO_ResetBits(GPIOB,GPIO_Pin_10);//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	GPIO_ResetBits(GPIOB,GPIO_Pin_10);
	GPIO_ResetBits(GPIOB,GPIO_Pin_11);//STOP:when CLK is high DATA change form low to high
	GPIO_SetBits(GPIOB,GPIO_Pin_10); 
	GPIO_SetBits(GPIOB,GPIO_Pin_11);//发送I2C总线结束信号	
}

void IIC_Nowait_Ack(void){
	GPIO_SetBits(GPIOB,GPIO_Pin_10);//时钟拉高然后拉低，产生信号但是不去管从机是否返回(等你等到花都谢了)
	GPIO_ResetBits(GPIOB,GPIO_Pin_10);
}

//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
	GPIO_SetBits(GPIOB,GPIO_Pin_11);
	GPIO_SetBits(GPIOB,GPIO_Pin_10);
	while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11))
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	GPIO_ResetBits(GPIOB,GPIO_Pin_10);//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void IIC_Ack(void)
{
	GPIO_ResetBits(GPIOB,GPIO_Pin_10);
	SDA_OUT();
	GPIO_ResetBits(GPIOB,GPIO_Pin_11);
	GPIO_SetBits(GPIOB,GPIO_Pin_10);
	GPIO_ResetBits(GPIOB,GPIO_Pin_10);
}
//不产生ACK应答		    
void IIC_NAck(void)
{
	GPIO_ResetBits(GPIOB,GPIO_Pin_10);
	SDA_OUT();
	GPIO_SetBits(GPIOB,GPIO_Pin_11);
	GPIO_SetBits(GPIOB,GPIO_Pin_10);
	GPIO_ResetBits(GPIOB,GPIO_Pin_10);
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    GPIO_ResetBits(GPIOB,GPIO_Pin_10);//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        if(1 == (txd&0x80)>>7)
			GPIO_SetBits(GPIOB,GPIO_Pin_11);
		else
			GPIO_ResetBits(GPIOB,GPIO_Pin_11);
        txd<<=1;
		GPIO_SetBits(GPIOB,GPIO_Pin_10);
		GPIO_ResetBits(GPIOB,GPIO_Pin_10);	
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        GPIO_ResetBits(GPIOB,GPIO_Pin_10);
		GPIO_SetBits(GPIOB,GPIO_Pin_10);
        receive<<=1;
        if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11))receive++;   
    }					 
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}
						    

