#ifndef _MIDDLE_H_
#define _MIDDLE_H_
#include "stm32f10x_gpio.h"
#include "stm32f10x.h"

#define task_total 8
extern u16 task_time[task_total];
extern u8 display_cache[1024];

// SPI引脚定义 SCLK(PB5), MOSI(PB6), MISO(PB7)
#define PORT_SPI        GPIOB
#define PIN_SCLK        GPIO_Pin_13        
#define PIN_MOSI        GPIO_Pin_15
#define PIN_MISO        GPIO_Pin_14

void usart_init(void);
void GPIO_Initial(void);                // 初始化通用IO端口
void SPI_Initial(void);                 // 初始化SPI
//void TIM3_Initial(void);                // 初始化定时器3，定时时间为1ms
u8 SPI_ExchangeByte(u8  input);    // 通过SPI进行数据交换 
u16 data_Transformation(u8 ready);//将一个8位数据转换为两个8位ASCLL码以int返回
void usart_send(u8 *text,u16 size);//给定数组和大小使用usart1输出字符串
void OLED_Reversal(u8 *copy_buffer,u8 *paste_buffer,u16 size);//反转数据
void OLED_printchar(u8 x,u8 y,u8 chr);//指定坐标显示一个6*8字符,但是y只能是0~7行(页)
void OLED_printstring(u8 x,u8 y,u8 *chr,u8 size);//指定坐标显示字符串,超长就换行覆盖
void cache_clear(void);
void OLED_curve(u16 *buffer,u8 size);

void rf_receive(u8 *data_buffer);//射频接收
void rf_send(u8 *data_buffer,int size);//射频发送
void set_freq(u16 freq);//设置频率
u32 rf_scan(void);//扫描信号强度

void IWDG_Init(void);
void TIM_init(void);
void TIM3_IRQHandler(void);//定时器处理函数

void delay_us(u32);
void delay_ms(u32);

u32 IR_scan(void);
#endif //_BSP_H_

