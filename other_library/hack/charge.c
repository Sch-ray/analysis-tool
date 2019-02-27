#include "charge.h"
#include "middle.h"//一些中间层函数
#include "CC1101.h"//射频相关
#include "oled.h"//显示相关
#include "rfid.h"
#include "bmp.h"

u8 display_cache[1024]={0};//显存，但是是按照页的方式存储和绘制
u8 loggg=0;//当进行菜单跳跃的时候避免按键判断出错导致连续进入,1为跳跃
u8 key_cache=0;//按键缓存值

//显示一帧
void OLED_display(void){
	OLED_Draw(display_cache);
	task_time[2]=80;//记得任务执行完要为下一次准备
}
//主菜单刷新
void main_menu(void){
	cache_clear();
	static int arrow=0;//指针的位置
	
	OLED_printstring(0,0,"RF-scan",7);//信号强度
	OLED_printstring(0,1,"RF-send",7);//发送数据
	OLED_printstring(0,2,"RF-receice",10);//接收数据
	OLED_printstring(0,3,"IR-receive",10);//红外接收
	OLED_printstring(0,4,"RFID-receive",12);//RFID
	OLED_printstring(0,5,"DATA-browse",11);//数据库
	OLED_printchar(119,arrow,'<');
	
	if(key_cache != 0x3E){//如果指针有变化
		if(key_cache == 0x1E){//上
			if(arrow == 0)//如果到了最上面就不继续向上了
				return;
			arrow--;
		}
		if(key_cache == 0x2E){//下
			if(arrow == 7)
				return;//如果到了最下面就不继续向下了
			arrow++;
		}
		if(key_cache == 0x3C){//中键
			switch(arrow){
				case 0:
					//loggg=1;//要进行跳跃
					//task_pool[1]=rf_scan_menu;//任务指针重定义
					break;
				case 1:
					loggg=1;//要进行跳跃
					task_pool[1]=rf_send_menu;
					break;
				case 2:
					loggg=1;//要进行跳跃
					task_pool[1]=rf_receive_menu;//任务指针重定义
					break;
				case 3:
					break;
				case 4:
					loggg=1;//要进行跳跃
					task_pool[1]=RFID_receive;//任务指针重定义
					break;
				default:
					break;
			}
			return;
		}
	}
	task_time[1]=200;
}
//开机页面的一脸懵逼
void boot_display(void){
	OLED_Reversal(boot_picture,display_cache,1024);
	OLED_Draw(display_cache);
	delay_ms(2000);
}

//扫描5个按键并且存到变量里。
void KEY_refresh(u8 key){
	if(key==1)
		key_cache |= 1;
	else
		key_cache &= 0xFE;
	key_cache<<=1;
}
void KEY_scan(void){
	key_cache=0;
	KEY_refresh(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6));//上
	KEY_refresh(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5));//下
	KEY_refresh(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4));//左
	KEY_refresh(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3));//右
	KEY_refresh(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15));//中
	task_time[0]=50;//记得任务执行完要为下一次准备
}


void LED_Shining(void){
	static u8 logg=0;
	if(logg==1)
		GPIO_ResetBits(GPIOA,GPIO_Pin_3);
	else
		GPIO_SetBits(GPIOA,GPIO_Pin_3);
	//取反
	if(logg==1)
		logg=0;
	else
		logg=1;
	task_time[3]=1000;
}

void rf_scan_menu(void){
	//这个函数应该绘制一个波形图以显示信号强度
}

//接收菜单
void rf_receive_menu(void){
	cache_clear();
	static int arrow=0;
	
	Task_handover();
	KEY_scan();
	OLED_printstring(0,0,"315MHz",6);
	OLED_printstring(0,1,"433MHz",6);
	OLED_printstring(0,2,"868MHz",6);
	OLED_printstring(0,3,"915MHz",6);
	OLED_printchar(119,arrow,'<');
	
	if(key_cache != 0x3E){//如果指针有变化
		if(key_cache == 0x1E){//上
			if(arrow == 0)//如果到了最上面就不继续向上了
				return;
			arrow--;
		}
		if(key_cache == 0x2E){//下
			if(arrow == 7)
				return;//如果到了最下面就不继续向下了
			arrow++;
		}
		if(key_cache == 0x36){//左
			loggg=1;
			task_pool[1]=main_menu;//返回主菜单
		}
		if(key_cache == 0x3C){//中
			switch(arrow){
				case 0:
					loggg=1;
					set_freq(315);
					CC1101SetTRMode(RX_MODE);//避免在运行新任务的时候总是循环此步骤
					task_pool[1]=rf_scan_compatible;//任务指针重定义
					return;
				case 1:
					loggg=1;
					set_freq(433);
					CC1101SetTRMode(RX_MODE);
					task_pool[1]=rf_scan_compatible;//任务指针重定义
					return;
				case 2:
					loggg=1;
					set_freq(868);
					CC1101SetTRMode(RX_MODE);
					task_pool[1]=rf_scan_compatible;//任务指针重定义
					return;
				default:
					loggg=1;
					set_freq(915);
					CC1101SetTRMode(RX_MODE);
					task_pool[1]=rf_scan_compatible;//任务指针重定义
					return;
			}
		}
	}
	task_time[1]=200;
}

//发送菜单
void rf_send_menu(void){
	cache_clear();
	static int arrow=0;
	
	Task_handover();
	KEY_scan();
	OLED_printstring(0,0,"315MHz",6);
	OLED_printstring(0,1,"433MHz",6);
	OLED_printstring(0,2,"868MHz",6);
	OLED_printstring(0,3,"915MHz",6);
	OLED_printchar(119,arrow,'<');
	
	if(key_cache != 0x3E){//如果指针有变化
		if(key_cache == 0x1E){//上
			if(arrow == 0)//如果到了最上面就不继续向上了
				return;
			arrow--;
		}
		if(key_cache == 0x2E){//下
			if(arrow == 7)
				return;//如果到了最下面就不继续向下了
			arrow++;
		}
		if(key_cache == 0x36){//左
			loggg=1;
			task_pool[1]=main_menu;//返回主菜单
		}
		if(key_cache == 0x3C){//中
			switch(arrow){
				case 0:
					set_freq(315);
					CC1101SetTRMode(TX_MODE);
					rf_send_compatible();
					return;
				case 1:
					set_freq(433);
					CC1101SetTRMode(TX_MODE);
					rf_send_compatible();
					return;
				case 2:
					set_freq(868);
					CC1101SetTRMode(TX_MODE);
					rf_send_compatible();
					return;
				default:
					set_freq(915);
					CC1101SetTRMode(TX_MODE);
					rf_send_compatible();
					return;
			}
		}
	}
	task_time[1]=200;
}

//读取GDO2的值并且绘制
void rf_scan_compatible(void){
	u32 i=0;
	u16 data[24]={0};
	u8 sign=1,j=0;
	cache_clear();
	
	Task_handover();
	
	if(key_cache != 0x3E){//如果指针有变化
		if(key_cache == 0x36){//左
			loggg=1;
			task_pool[1]=rf_scan_menu;//返回选择菜单
		}
	}
	
	if(0 == GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)){
		while(0 == GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)){
			delay_us(1);
			i++;
			if(i >= 100000)//100ms
				return;//先退出，让其他任务也执行
		}
		i=0;
		//如果确实数据有变化再清除
		OLED_curve(data,24);
		for(j=0;j<24;j++)
			data[j]=0;
		
		for(j=0;j<24;j++){
			while(sign == GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)){
				delay_us(20);
				i++;
				if(i>=3000)//3ms
					break;
			}
			data[j]=i;
			
			if(sign==1)
				sign=0;
			else
				sign=1;
		}
	}
	
	task_time[1]=300;//300ms扫描一次
}

//根据延时发送准确的波形
void rf_send_compatible(void){
	u8 i,sign=1;
	//短持续350，长持续1000左右，很标准。实际延时的时候比us多一点
	u16 testda[46]={350,1000,350,1000,350,1000,350,1000,350,1000,350,1000,350,1000,350,1000,350,1000,
	};
	OLED_Reversal(baby_picture,display_cache,1024);
	OLED_Draw(display_cache);
	GPIO_ResetBits(GPIOB,GPIO_Pin_0);
	delay_ms(10);
	for(i=0;i<46;i++){
		if(sign==1)
			GPIO_SetBits(GPIOB,GPIO_Pin_0);
		else
			GPIO_ResetBits(GPIOB,GPIO_Pin_0);
		
		delay_us(testda[i]);
		
		if(sign==1)
				sign=0;
			else
				sign=1;
	}
	GPIO_ResetBits(GPIOB,GPIO_Pin_0);
	delay_ms(1000);
}

void RFID_receive(void){
	u8 i;
	static u32 rfid_id=0;
	Task_handover();
	
	OLED_Reversal(baby_picture,display_cache,1024);
	OLED_Draw(display_cache);
	
	RC522_Handel();
	
	if(((rfid_id>>24)!=SN[3]) || ((rfid_id>>16)!=SN[2]) || ((rfid_id>>8)!=SN[1]) || ((rfid_id)!=SN[0]))
		GPIO_ResetBits(GPIOA,GPIO_Pin_3);
		delay_ms(300);
		GPIO_SetBits(GPIOA,GPIO_Pin_3);
	
	for(i=0;i<4;i++){
		rfid_id<<=8;
		rfid_id=SN[i];
	}
	
	if(key_cache != 0x3E){//如果指针有变化
		if(key_cache == 0x36){//左
			loggg=1;
			task_pool[1]=main_menu;//返回主菜单
		}
	}
	task_time[1]=300;
}

void rf_warehouse_menu(void){
	//从存储IC读取数据
	//IC里应该存放文件目录头，给它2KByte应该够用了，剩下6K用来存储数据
	//将文件么固定为32字节，20字节用来命名，2字节存储所在地址的指针，2字节存储数据长度
	//8字节用来标识是什么数据，RF?还是红外，具体什么频率什么调制
	//用户保存的都起名为：user_rf(ir)_xx
}




//没有什么卵用的函数
void nop_function(void){
	return;
}


void Task_handover(void){
	if(loggg == 1){
		delay_ms(500);
		loggg--;
	}
}

