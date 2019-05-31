#include "stm32f10x.h"
#include "charge.h"//一些已经实现最终功能的函数
#include "middle.h"//一些中间层函数
#include "CC1101.h"//还在测试
#include "oled.h"//oled显示

#define task_total 4//任务数量，定时器中断处理函数里需要使用
u16 task_time[task_total]={5,10,10,10};//每个任务的时间,单位ms，
void(*task_pool[task_total])(void)={KEY_scan,OLED_display,three_scan,real_task};

int main(){
	u8 i=0;
	GPIO_Initial();//初始化所有用到的I/O口
	SPI_Initial();//初始化SPI2
	USART_init();//初始化USART
	//CC1101Init();//初始化CC1101
	OLED_Init();//初始化屏幕
	//RC522_Init();
	TIM_init();//定时器配置
	IWDG_Init();//初始化看门狗
	
	boot_display();
	while(1){
		IWDG_ReloadCounter();//喂狗，6秒吧
		for(i=0;i<task_total;i++){
			if(task_time[i]==0)//如果这个任务到时间了
				(*task_pool[i])();//开始任务
		}
	}
}
