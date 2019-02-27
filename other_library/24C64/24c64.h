#ifndef __24C64_H
#define __24C64_H
#include "stm32f10x.h"
#include "Ji2c.h"

u8 AT24CXX_ReadOneByte(u16 ReadAddr);
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite);

void AT24CXX_ReadLenByte(u16 ReadAddr,u8 *DataToRead,u8 Len);
void AT24CXX_WriteLenByte(u16 WriteAddr,u8 *DataToWrite,u8 Len);
#endif


