#include "stm32f10x.h"
#include "charge.h"//一些已经实现最终功能的函数
#include "middle.h"//一些中间层函数
#include "CC1101.h"
#include "oled.h"
#include "rfid.h"

#define task_total 8//任务数量，定时器中断处理函数里需要使用
u16 task_time[task_total]={50,200,80,1000,65535,65535,65535,65535};//每个任务的时间,单位ms，
void(*task_pool[task_total])(void)={KEY_scan,main_menu,OLED_display,LED_Shining,
									nop_function,nop_function,nop_function,nop_function};

int main(){
	u8 i=0;
	GPIO_Initial();//初始化所有用到的I/O口
	SPI_Initial();//初始化SPI2
	usart_init();//初始化USART
	CC1101Init();//初始化CC1101
	OLED_Init();//初始化屏幕
	RC522_Init();
	TIM_init();//定时器配置
	IWDG_Init();//初始化看门狗
	
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);
	delay_ms(300);
	GPIO_SetBits(GPIOA,GPIO_Pin_1);
	
	boot_display();
	while(1){
		IWDG_ReloadCounter();//喂狗，6秒吧
		for(i=0;i<task_total;i++){
			if(task_time[i]==0)//如果这个任务到时间了
				(*task_pool[i])();//开始任务
		}
	}
}
