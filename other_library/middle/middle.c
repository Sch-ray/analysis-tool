#include "middle.h"
#include "CC1101.h"
#include "oled.h"
#include "oledfont.h"
#include "charge.h"

u8 freq_buffer[4][3]={
	{0x0C,0x18,0xE6},//315
	{0x10,0xAA,0x87},//433
	{0x21,0x5D,0xD2},//868
	{0x23,0x36,0x6F},//915
};

u8 textdata[]={"\nthe data is:"};

/**************
*延时一微秒
*************/
void delay_us(u32 nTimer){
	u32 i=0;
	for(i=0;i<nTimer;i++){
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	}
}

/**************
*延时一毫秒
*************/
void delay_ms(u32 time){
	u32 tim=8000*time;
	while(tim--);
}


//////////////////////////////所有GPIO和时钟配置//////////////////
void GPIO_Initial(void){
	//clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB,ENABLE);//所有I\O口时钟和SPI1
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3| RCC_APB1Periph_SPI2,ENABLE);//I2C是模拟的

	GPIO_InitTypeDef gpio_init;
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//禁用JTAG
	
	//SPI-SCK,SPI-MOSI

	gpio_init.GPIO_Pin =GPIO_Pin_13 | GPIO_Pin_15;//SCK MOSI
	gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽输出
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpio_init);
	GPIO_ResetBits(GPIOB,GPIO_Pin_13);//时钟空闲是低的

	//GD02,SPI-MISO
	gpio_init.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_14;
	gpio_init.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpio_init);

	//GDO0,I2C-SCL,I2C-SDA,SPI-CC1101-CS

	gpio_init.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;//SCL SDA SPI-CS
	gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpio_init);
	GPIO_SetBits(GPIOB,GPIO_Pin_12);//CS空闲时高的

	//右，左，下，上，红外
	
	gpio_init.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4
						| GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	gpio_init.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpio_init);
	
	//中
	gpio_init.GPIO_Pin = GPIO_Pin_15;
	gpio_init.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio_init);
	
	//RGB
	gpio_init.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_7 | GPIO_Pin_8;
	gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio_init);
	GPIO_SetBits(GPIOA,GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);
}
//////////////////////////////SPI2配置////////////////////////////
void SPI_Initial(void){
	SPI_InitTypeDef spi_init;

	spi_init.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//全双工
	spi_init.SPI_Mode = SPI_Mode_Master;//主机
	spi_init.SPI_DataSize = SPI_DataSize_8b;//8位数据
	spi_init.SPI_CPOL = SPI_CPOL_Low;//空闲的时候时钟是低电平
	spi_init.SPI_CPHA = SPI_CPHA_1Edge;//数据在第一个时钟沿捕获
	spi_init.SPI_NSS = SPI_NSS_Soft;//硬件CS
	spi_init.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;//16分频
	spi_init.SPI_FirstBit = SPI_FirstBit_MSB;//左对齐
	spi_init.SPI_CRCPolynomial = 7;//
	SPI_Init(SPI2, &spi_init);

	SPI_Cmd(SPI2, ENABLE);
}

//////////////////////////////串口配置////////////////////////////
void usart_init(void){
	GPIO_InitTypeDef gpio_init;
	USART_InitTypeDef usart_init;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//GPIOA已经使能过了
	
	gpio_init.GPIO_Mode=GPIO_Mode_AF_PP;//复用推挽输出
	gpio_init.GPIO_Pin=GPIO_Pin_9;
	gpio_init.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&gpio_init);
	
	gpio_init.GPIO_Mode=GPIO_Mode_IN_FLOATING;//浮空输入
	gpio_init.GPIO_Pin=GPIO_Pin_10;
	gpio_init.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&gpio_init);
	
	usart_init.USART_BaudRate=9600;//波特率
	usart_init.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//硬件流控制
	usart_init.USART_Mode=USART_Mode_Rx | USART_Mode_Tx;//模式
	usart_init.USART_Parity=USART_Parity_No;//不使用校验
	usart_init.USART_StopBits=USART_StopBits_1;//停止位1
	usart_init.USART_WordLength=USART_WordLength_8b;//字长8bit
	
	USART_Init(USART1,&usart_init);
	USART_Cmd(USART1,ENABLE);
}

//////////////////////////////看门狗配置//////////////////////////
void IWDG_Init(void){
 	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);  //使能对寄存器IWDG_PR和IWDG_RLR的写操作
	IWDG_SetPrescaler(IWDG_Prescaler_256);  //设置IWDG预分频值:设置IWDG预分频值为64
	IWDG_SetReload(937);  //设置IWDG重装载值,最大FFF，4095
	IWDG_ReloadCounter();  //按照IWDG重装载寄存器的值重装载IWDG计数
	IWDG_Enable();  //使能IWDG
}
/*
最终是x(IWDG_Prescaler_x)*装载值/40
#define IWDG_Prescaler_4            ((uint8_t)0x00)
.
.
.
#define IWDG_Prescaler_256          ((uint8_t)0x06)
*/
//////////////////////////////定时器配置//////////////////////////
void TIM_init(void){
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置分组，各两位即能表示4*4=16
	TIM_TimeBaseInitTypeDef tim_init;//定时器配置结构体
	NVIC_InitTypeDef nvic_init;//中断配置结构体

	//定时器配置
	tim_init.TIM_Prescaler=35;//36分频，1MHz
	tim_init.TIM_CounterMode=TIM_CounterMode_Up;//向上计数
	tim_init.TIM_Period =1000;//1ms
	tim_init.TIM_ClockDivision =0;
	TIM_TimeBaseInit(TIM3,&tim_init);
	TIM_ITConfig(TIM3,TIM_IT_Update|TIM_IT_Trigger,ENABLE);
	
	//中断配置
	nvic_init.NVIC_IRQChannel=TIM3_IRQn;
	nvic_init.NVIC_IRQChannelPreemptionPriority=0;
	nvic_init.NVIC_IRQChannelSubPriority=3;
	nvic_init.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&nvic_init);
	TIM_Cmd(TIM3,ENABLE);
}

//////////////////////////////定时器中断处理//////////////////////
void TIM3_IRQHandler(void){
	u8 i=0;
	if (TIM_GetITStatus(TIM3,TIM_IT_Update)!=RESET){
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);//清理标志位
		for (i=0;i<task_total;i++){
			if(task_time[i] > 0)
				task_time[i]--;
		}
	}
}


/////////////////////////////SPI发送一个字节//////////////////////
u8 SPI_ExchangeByte(u8 data){
    u8 retry=0;
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){//发送缓存空标志位
		retry++;
		if(retry>200) break;//先等着，要是好久都等不到就直接发
	}			  
	SPI_I2S_SendData(SPI2, data); //通过外设SPIx发送一个数据
	retry=0;
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET){//接受缓存非空标志位
		retry++;
		if(retry>200) break;
	}
	return SPI_I2S_ReceiveData(SPI2); //返回通过SPIx最近接收的数据	
}

////////////////////////////转换一个字节数据为两个字节ASCLL///////
u16 data_Transformation(u8 ready){
	u16 data=0;
	u8 test=0;
	test=(ready>>4)&0x0F;//原始数据高4位右移
	
	if(test<10)
		data=0x30+test;
	else
		data=0x37+test;
	
	data=data<<8;//转换后的数据左移8位
	test=ready&0x0F;//原始数据高四位置零
	
	if(test<10)
		data+=(0x30+test);
	else
		data+=(0x37+test);//与转换后的数据结合
	return data;
}

/////////////////////////////串口发送字符串///////////////////////
void usart_send(u8 *text,u16 size){
	u32 logg=0;
	for(logg=0;logg<size;logg++){
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1,*(text+logg));
	}
}

/////////////////////////////接收数据/////////////////////////////
void rf_receive(u8 *data_buffer){

	CC1101SetTRMode(RX_MODE);//设置为接收模式

	int i=0,length=0,fnum=0;//循环用，数据包长度，有效数据长度
	if (0 == CC_IRQ_READ()){//检测无线模块是否产生接收中断

		for (i=0; i<64; i++)// 数据请空
        	data_buffer[i] = 0;

        while (CC_IRQ_READ() == 0);//等待中断发送完毕，开始正式传输
		
        length = CC1101RecPacket(data_buffer);// 读取接收到的数据长度和数据内容
		
		for (i=0; i<64; i++){//过滤，如果接受到的有效数据超过一定数量才认为是接收到了数据
        	if(data_buffer[i] != 0)
				fnum++;
        }
		
		if(fnum>=4){//开始向串口发送数据
			usart_send(textdata,14);//the data is:
			for(i=0;i<length;i++){
				int data=data_Transformation(data_buffer[i]);//转换

				while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);//等待标志位
				USART_SendData(USART1,data>>8);//高字节右移
				while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);//等待标志位
				USART_SendData(USART1,data&0xFF);//高字节清零，不清应该也可以
				while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);//等待标志位
				USART_SendData(USART1,' ');//发一个空格
			}
		}
	}
}

/////////////////////////////使用CC1101发送一些数据///////////////
void rf_send(u8 *data_buffer,int size){
	
	int i;
	
	CC1101SetTRMode(TX_MODE);//设置为发送模式
	CC1101SendPacket(data_buffer,size,BROADCAST);//广播数据

	//串口反馈
	usart_send(textdata,14);//send data is:

	for(i=0;i<size;i++){
		int data=data_Transformation(data_buffer[i]);//转换

		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);//等待标志位
		USART_SendData(USART1,data>>8);//高字节右移
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);//等待标志位
		USART_SendData(USART1,data&0xFF);//高字节清零，不清应该也可以
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);//等待标志位
		USART_SendData(USART1,' ');//发一个空格
	}
}

/////////////////////////////设置工作频率/////////////////////////
void set_freq(u16 freq){
	switch(freq){
		case 315:{
			CC1101WriteReg(CC1101_FREQ2,freq_buffer[0][0]);
			CC1101WriteReg(CC1101_FREQ1,freq_buffer[0][1]);
			CC1101WriteReg(CC1101_FREQ0,freq_buffer[0][2]);
			break;
		}
		case 433:{
			CC1101WriteReg(CC1101_FREQ2,freq_buffer[1][0]);
			CC1101WriteReg(CC1101_FREQ1,freq_buffer[1][1]);
			CC1101WriteReg(CC1101_FREQ0,freq_buffer[1][2]);
			break;
		}
		case 868:{
			CC1101WriteReg(CC1101_FREQ2,freq_buffer[2][0]);
			CC1101WriteReg(CC1101_FREQ1,freq_buffer[2][1]);
			CC1101WriteReg(CC1101_FREQ0,freq_buffer[2][2]);
			break;
		}
		default:{//915
			CC1101WriteReg(CC1101_FREQ2,freq_buffer[3][0]);
			CC1101WriteReg(CC1101_FREQ1,freq_buffer[3][1]);
			CC1101WriteReg(CC1101_FREQ0,freq_buffer[3][2]);
			break;
		}
	}
}


/////////////////////////////扫描是否有数据产生///////////////////
///返回四个频率的信号强度,以32位的形式返回
u32 rf_scan(void){
	u32 Strength=0;
	
		set_freq(315);//设置频率
		delay_ms(5);
		Strength|=CC1101ReadStatus(0x34);//读取信号强度寄存器
		Strength=Strength<<8;//移位
	
		set_freq(433);
		delay_ms(5);
		Strength|=CC1101ReadStatus(0x34);
		Strength=Strength<<8;
	
		set_freq(868);
		delay_ms(5);
		Strength|=CC1101ReadStatus(0x34);
		Strength=Strength<<8;
	
		set_freq(915);
		delay_ms(5);
		Strength|=CC1101ReadStatus(0x34);
	
		return Strength;
}

/**************
*获取高电平时间，红外的附属程序
*************/
u8 Pulse_Width_Check(void)
{
    u8 t=0;	 
    while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7))//位读取
    {	 
		t++;delay_us(20);					 
        if(t==250)return t; //超时溢出
    }
    return t;
}

/////////////////////////////红外线扫描解码//////////////////////
u32 IR_scan(void){
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7)){//如果是低电平
		delay_ms(4);
		if(1==GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7))
			return 0;
		u8 res=0;//宽度
	    u8 OK=0; 
	    u8 RODATA=0;
	    u32 Remote_Odr=0;		 
		while(1){//一直检测
	        if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7))//等待高电平出现
	        {
	            res=Pulse_Width_Check();//获得此次高脉冲宽度，在进行时间捕获的时候已经错过第二个下降沿了，不过不影响
	            if(res==250){
	            	break;//退出
	            }


	            if(res>=200&&res<250){
	            	OK=1; //获得前导位(4.5ms)，标记为信号
	            }
		            else if(res>=50&&res<85)RODATA=1;//1.5ms
		           	else if(res>=10&&res<50)RODATA=0;//500us

	            if(OK)//如果接收到了前导位
	            {
	                Remote_Odr<<=1;//左移
	                Remote_Odr+=RODATA; 
	            }   
	        }			 						 
	    }
	    return Remote_Odr;
	}
	return 0;
}

//对输入的第一个数组里的字节全部反转并存到第二个数组
void OLED_Reversal(u8 *copy_buffer,u8 *paste_buffer,u16 size){
	u16 i;
	u8 m,n;
	for(i=0;i<size;i++){
		for(n=0;n<8;n++){//反转
				m<<=1;
				m+=(copy_buffer[i]>>n)&0x1;
			}
		paste_buffer[i]=m;
	}
}
//x可以是0~128像素，但是y只能是0~7行(页)
void OLED_printchar(u8 x,u8 y,u8 chr){
	u16 coordinate=y*128+x;
	u8 i,character=chr-' ';
	for(i=0;i<6;i++)
		display_cache[coordinate+i]=font6x8[character][i];
}
void OLED_printstring(u8 x,u8 y,u8 *chr,u8 size){
	u8 i=0,x1=x,y1=y;
	for(i=0;i<size;i++){
		OLED_printchar(x1,y1,chr[i]);
		if(x1<=122)
			x1+=6;
		else{
			x=0;
			y++;
		}
	}
}
//显存清空
void cache_clear(void){
	u16 i;
	for(i=0;i<1024;i++)
		display_cache[i]=0;
}
//波形图绘制，数组里存储的是每个变化沿的时间间隔，从高电平开始。
void OLED_curve(u16 *buffer,u8 size){
	//    竖线，     高电平， 低电平，临时,像素，需要绘制的状态
	u8 vertical=0xFE,high=0x2,low=0x80,i,j,pixel,sign=1;
	//已经绘制的像素数,注意是当前像素所对应的数组标号，包括0
	u16 already=0;
	//开头预先绘制3个像素低电平和1像素竖线
	display_cache[0]=display_cache[1]=display_cache[2]=low;
	display_cache[3]=vertical;
	already=3;
	
	for(i=0;i<size;i++){//循环时间数组
		
		pixel=buffer[i]/250;//需要绘制的像素数
		if(pixel==0)
			pixel=1;
		
		if((already+pixel)>=895)//避免超出显存破坏数据
			return;
		
		if(sign==1){//如果这次要绘制1
			for(j=1;j<=pixel;j++)
				display_cache[already+j]=high;//绘制偏移值加像素数长度的直线
		}
		else{
			for(j=1;j<=pixel;j++)
				display_cache[already+j]=low;
		}
		
		already+=pixel;//更新已绘制像素数
		display_cache[already+1]=vertical;//绘制竖线
		already++;
		if(sign==1)
			sign=0;
		else
			sign=1;
	}
}

