#ifndef _CC1101_H_
#define _CC1101_H_

#include "CC1101_REG.h"

#define PORT_CC_CSN     GPIOA
#define PIN_CC_CSN      GPIO_Pin_2

#define PORT_CC_IRQ     GPIOA//将GDO0配置为了IRQ中断
#define PIN_CC_IRQ      GPIO_Pin_11

#define PORT_CC_GDO2    GPIOA
#define PIN_CC_GDO2     GPIO_Pin_12

#define CC_CSN_LOW()    GPIO_ResetBits(PORT_CC_CSN, PIN_CC_CSN);
#define CC_CSN_HIGH()   GPIO_SetBits(PORT_CC_CSN, PIN_CC_CSN)

#define CC_IRQ_READ()   GPIO_ReadInputDataBit(PORT_CC_IRQ, PIN_CC_IRQ)

/*===========================================================================
----------------------------------macro definitions--------------------------
============================================================================*/
typedef enum { TX_MODE, RX_MODE } TRMODE;
typedef enum { BROAD_ALL, BROAD_NO, BROAD_0, BROAD_0AND255 } ADDR_MODE;
typedef enum { BROADCAST, ADDRESS_CHECK} TX_DATA_MODE;

/*===========================================================================
-------------------------------------exported APIs---------------------------
============================================================================*/

/*从指定寄存器读取数据*/
u8 CC1101ReadReg(u8 addr);

/*读取状态寄存器*/
u8 CC1101ReadStatus(u8 addr);

void CC1101WriteReg( u8 addr, u8 value );

/*设置模式*/
void CC1101SetTRMode(TRMODE mode);

/*写控制字节*/
void CC1101WriteCmd(u8 command);

/*设置为空闲模式*/
void CC1101SetIdle(void);

/*发送数据包*/
void CC1101SendPacket(u8 *txbuffer, u8 size, TX_DATA_MODE mode);

/*设置地址和地址模式*/
void CC1101SetAddress(u8 address, ADDR_MODE AddressMode);

/*设置同步字节*/
void CC1101SetSYNC(u16 sync);

/*接收数据包*/
u8 CC1101RecPacket(u8 *rxBuffer);

/*初始化WOR*/
void  CC1101WORInit(void);

/*初始化1101，以用户可以修改，里面是一些寄存器配置*/
void CC1101Init(void);

#endif
