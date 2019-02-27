#include "stm32f10x.h"
#ifndef _CHARGE_H_
#define _CHARGE_H_

#define task_total 8
extern u16 task_time[task_total];
extern void(*task_pool[task_total])(void);
extern unsigned char SN[4]; //卡号

void OLED_display(void);//显示一帧图像
void KEY_scan(void);//扫描按键
void LED_Shining(void);//指示灯闪烁
void main_menu(void);//主菜单
void boot_display(void);//开机显示
void rf_scan_menu(void);//rf_scan菜单
void rf_send_menu(void);//rf_scan菜单
void rf_receive_menu(void);
void rf_scan_compatible(void);//rf接收并更新显存函数
void rf_send_compatible(void);//rf发送并更新显存函数
void rf_warehouse_menu(void);
void RFID_receive(void);

void Task_handover(void);//在进行任务跳跃的时候需要避免按键出错的函数
void nop_function(void);//空函数
#endif
