#include "charge.h"
#include "middle.h"//一些中间层函数
#include "CC1101.h"//射频相关
#include "oled.h"//显示相关
#include "bmp.h"

u8 display_cache[1024]={0};//显存，但是是按照页的方式存储和绘制
u8 task_menu[256]={0};//任务栏
u8 menu=0;//当前所在工作模式0-7
u8 menu_last=1;//保存上次的工作模式

//扫描按键
void KEY_scan(void){
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5) == 0){
		delay_ms(50);
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5) == 0){
			menu++;
			if(menu == 8)
				menu=0;
		}
	}
	task_time[0]=5;//记得任务执行完要为下一次准备
}

//按照每秒10帧的形式刷新屏幕
void OLED_display(void){
	fill_picture(display_cache);
	task_time[1]=10;//记得任务执行完要为下一次准备
}

void real_task(void){
	u8 i=0;
	if(menu != menu_last){//根据任务值绘制，节约CPU资源
		for(i=0;i<128;i++){//绘制整个16*128任务栏
			display_cache[768+i]=task_menu[i];
			display_cache[896+i]=task_menu[128+i];
		}
		for(i=0;i<16;i++){
			display_cache[768+menu*16+i]=~task_menu[menu*16+i];
			display_cache[896+menu*16+i]=~task_menu[128+menu*16+i];
		}
	}
	menu_last=menu;
	task_time[3]=10;
}

void three_scan(){
	static char cha='a';
	OLED_printstring(0,0,"this is test:",13);
	OLED_printchar(78,0,cha);
	cha++;
	task_time[2]=500;
}

//开机页面
void boot_display(void){
	OLED_Reversal(boot_picture,display_cache,1024);
	fill_picture(display_cache);
	OLED_Reversal(menu_picture,task_menu,256);
	delay_ms(1000);
	cache_clear();
}

//没有什么卵用的函数
void nop_function(void){
	task_time[3]=65535;
	return;
}


