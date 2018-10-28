#include "stm32f10x.h"
#include "sys.h"
	
/*------------------------------------------------------------
                           主函数
------------------------------------------------------------*/
int main()
{
    
  Stm32_Clock_Init();//系统时钟设置
	RCC->APB2ENR |= 0x00000001; //开启afio时钟，|=任何数或0都为原数，或1相当于置一，所以这段代码什么意思
  //AFIO->MAPR = (0x00FFFFFF & AFIO->MAPR)|0x04000000;          //关闭JTAG 
				

	RCC->APB2ENR|=0X0000001c;//先使能外设IO PORTa,b,c时钟
	
	GPIOB->CRH=0X33333333;    //推挽输出，32位
	GPIOB->CRL=0X33333333;    //推挽输出，ABCD端口各有16个啊，怎么会是32，而且是高低位，33=B00110011
	GPIOC->CRH=0X33333333;    //推挽输出
	GPIOC->CRL=0X33333333;    //推挽输出
	GPIOD->CRH=0X33333333;    //推挽输出
	GPIOD->CRL=0X33333333;    //推挽输出
	GPIOA->CRH=0X33333333;    //推挽输出
	GPIOA->CRL=0X33333333;    //推挽输出

	while (1)
  {			
	delay_ms(100);     	 
	GPIOB->ODR=0;	  		 //全部输出0，48引脚的只有A和B，100引脚的甚至有E
	GPIOA->ODR=0;		
	GPIOC->ODR=0;
	GPIOD->ODR=0;
	delay_ms(100);
	GPIOB->ODR=0xffffffff;	 //全部输出1 
	GPIOA->ODR=0xffffffff;	  
	GPIOC->ODR=0xffffffff;
	GPIOD->ODR=0xffffffff;
	}
}
