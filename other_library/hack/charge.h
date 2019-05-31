#include "stm32f10x.h"
#ifndef _CHARGE_H_
#define _CHARGE_H_

#define task_total 4
extern u16 task_time[task_total];
extern void(*task_pool[task_total])(void);

void OLED_display(void);//显示一帧图像
void KEY_scan(void);//扫描按键
void real_task(void);//任务栏绘制
void boot_display(void);//开机显示
void three_scan(void);

void nop_function(void);//空函数
#endif
