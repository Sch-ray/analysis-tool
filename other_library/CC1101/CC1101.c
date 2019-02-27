#include "stm32f10x.h"
#include "CC1101.H"

//10, 7, 5, 0, -5, -10, -15, -20, dbm output power, 0x12 == -30dbm
u8 PaTabel[] = { 0xc0, 0xC8, 0x84, 0x60, 0x68, 0x34, 0x1D, 0x0E};

// Sync word qualifier mode = 30/32 sync word bits detected 
// CRC autoflush = false 
// Channel spacing = 199.951172 
// Data format = Normal mode 
// Data rate = 2.00224 
// RX filter BW = 58.035714 
// PA ramping = false 
// Preamble count = 4 
// Whitening = false 
// Address config = No address check 
// Carrier frequency = 400.199890 
// Device address = 0 
// TX power = 10 
// Manchester enable = false 
// CRC enable = true 
// Deviation = 5.157471 
// Packet length mode = Variable packet length mode. Packet length configured by the first byte after sync word 
// Packet length = 255 
// Modulation format = GFSK 
// Base frequency = 399.999939 
// Modulated = true 
// Channel number = 1 
// PA table 
#define PA_TABLE {0xc2,0x00,0x00,0x00,0x00,0x00,0x00,0x00,}

static const u8 CC1101InitData[24][2]=//前面是地址后面是要设置的数据
{
  {CC1101_IOCFG0,      0x0C},//GDO0功能设置
  {CC1101_IOCFG2,      0x0D},//GDO2功能设置，连续透传
  {CC1101_FIFOTHR,     0x47},//某个阈值
  {CC1101_PKTCTRL0,    0xF2},//数据包大小，异步和这个有关？改为F2，原来是45
  {CC1101_CHANNR,      0x01},//通道
  {CC1101_FSCTRL1,     0x06},//配置晶振分频的？
  {CC1101_FREQ2,       0x10},//频率433
  {CC1101_AGCCTRL2,    0xB0},//英文手册里的介绍详细多了，根据信号强度过滤
  {CC1101_FREQ1,       0xAA},//频率
  {CC1101_FREQ0,       0x87},//频率
  {CC1101_MDMCFG4,     0xF7},//调制解调速率4800*8=38400
  {CC1101_MDMCFG3,     0xA3},//调制解调速率低字节
  {CC1101_MDMCFG2,     0x33},//调制解调方式 0011 0011 OOK调制
  {CC1101_DEVIATN,     0x15},//偏率？
  {CC1101_MCSM0,       0x18},//频偏
  {CC1101_FOCCFG,      0x16},//位同步？
  {CC1101_WORCTRL,     0xFB},
  {CC1101_FSCAL3,      0xE9},
  {CC1101_FSCAL2,      0x2A},
  {CC1101_FSCAL1,      0x00},
  {CC1101_FSCAL0,      0x1F},
  {CC1101_TEST2,       0x81},
  {CC1101_TEST1,       0x35},
  {CC1101_MCSM1,       0x3B},
};


/*read a byte from the specified register*/
u8 CC1101ReadReg( u8 addr );

/*Read some bytes from the rigisters continously*/
void CC1101ReadMultiReg( u8 addr, u8 *buff, u8 size );

/*Write a byte to the specified register*/
void CC1101WriteReg( u8 addr, u8 value );

/*Flush the TX buffer of CC1101*/
void CC1101ClrTXBuff( void );

/*Flush the RX buffer of CC1101*/
void CC1101ClrRXBuff( void );

/*Get received count of CC1101*/
u8 CC1101GetRXCnt( void );

/*Reset the CC1101 device*/
void CC1101Reset( void );

/*向指定寄存器写数据*/
void CC1101WriteMultiReg( u8 addr, u8 *buff, u8 size );

extern u8 SPI_ExchangeByte(u8 input); // 通过SPI进行数据交换

/*
================================================================================
Function : CC1101WORInit( )
    Initialize the WOR function of CC1101
INPUT    : None
OUTPUT   : None
================================================================================
*/
void  CC1101WORInit( void )
{

    CC1101WriteReg(CC1101_MCSM0,0x18);
    CC1101WriteReg(CC1101_WORCTRL,0x78); //Wake On Radio Control
    CC1101WriteReg(CC1101_MCSM2,0x00);
    CC1101WriteReg(CC1101_WOREVT1,0x8C);
    CC1101WriteReg(CC1101_WOREVT0,0xA0);
	
	CC1101WriteCmd( CC1101_SWORRST );
}
/*
================================================================================
Function : CC1101ReadReg( )
    read a byte from the specified register
INPUT    : addr, The address of the register
OUTPUT   : the byte read from the rigister
================================================================================
*/
u8 CC1101ReadReg( u8 addr )
{
    u8 i;
    CC_CSN_LOW( );//拉低CS开始传输
    SPI_ExchangeByte( addr | READ_SINGLE);//最高位置一并发送数据，调用：stm的spi发送函数
    i = SPI_ExchangeByte( 0xFF );//此时要从从机读取数据，但是为了避免没必要的误操作所以使用0xFF
    CC_CSN_HIGH( );//拉高CS停止传输
    return i;
}
/*
================================================================================
Function : CC1101ReadMultiReg( )
    Read some bytes from the rigisters continously
INPUT    : addr, The address of the register
           buff, The buffer stores the data
           size, How many bytes should be read
OUTPUT   : None
================================================================================
*/
void CC1101ReadMultiReg( u8 addr, u8 *buff, u8 size )
{
    u8 i, j;
    CC_CSN_LOW( );
    SPI_ExchangeByte( addr | READ_BURST);
    for( i = 0; i < size; i ++ )
    {
        for( j = 0; j < 20; j ++ );
        *( buff + i ) = SPI_ExchangeByte( 0xFF );
    }
    CC_CSN_HIGH( );
}
/*
================================================================================
Function : CC1101ReadStatus( )
    Read a status register
INPUT    : addr, The address of the register
OUTPUT   : the value read from the status register
================================================================================
*/
u8 CC1101ReadStatus( u8 addr )
{
    u8 i;
    CC_CSN_LOW( );
    SPI_ExchangeByte( addr | READ_BURST);
    i = SPI_ExchangeByte( 0xFF );
    CC_CSN_HIGH( );
    return i;
}
/*
================================================================================
Function : CC1101SetTRMode( )
    Set the device as TX mode or RX mode
INPUT    : mode selection
OUTPUT   : None
================================================================================
*/
void CC1101SetTRMode( TRMODE mode )
{
    if( mode == TX_MODE )
    {
        CC1101WriteReg(CC1101_IOCFG0,0x46);
        CC1101WriteCmd( CC1101_STX );
    }
    else if( mode == RX_MODE )
    {
        CC1101WriteReg(CC1101_IOCFG0,0x46);
        CC1101WriteCmd( CC1101_SRX );
    }
}
/*
================================================================================
Function : CC1101WriteReg( )
    Write a byte to the specified register
INPUT    : addr, The address of the register
           value, the byte you want to write
OUTPUT   : None
================================================================================
*/
void CC1101WriteReg( u8 addr, u8 value )
{
    CC_CSN_LOW( );
    SPI_ExchangeByte( addr );
    SPI_ExchangeByte( value );
    CC_CSN_HIGH( );
}
/*
================================================================================
Function : CC1101WriteMultiReg( )
    Write some bytes to the specified register
INPUT    : addr, The address of the register
           buff, a buffer stores the values
           size, How many byte should be written
OUTPUT   : None
================================================================================
*/
void CC1101WriteMultiReg( u8 addr, u8 *buff, u8 size )
{
    u8 i;
    CC_CSN_LOW( );
    SPI_ExchangeByte( addr | WRITE_BURST );
    for( i = 0; i < size; i ++ )
    {
        SPI_ExchangeByte( *( buff + i ) );
    }
    CC_CSN_HIGH( );
}
/*
================================================================================
Function : CC1101WriteCmd( )
    Write a command byte to the device
INPUT    : command, the byte you want to write
OUTPUT   : None
================================================================================
*/
void CC1101WriteCmd( u8 command )
{
    CC_CSN_LOW( );
    SPI_ExchangeByte( command );
    CC_CSN_HIGH( );
}
/*
================================================================================
Function : CC1101Reset( )
    Reset the CC1101 device
INPUT    : None
OUTPUT   : None
================================================================================
*/
void CC1101Reset( void )
{
    u8 x;

    CC_CSN_HIGH( );
    CC_CSN_LOW( );
    CC_CSN_HIGH( );
    for( x = 0; x < 100; x ++ );
    CC1101WriteCmd( CC1101_SRES );
}
/*
================================================================================
Function : CC1101SetIdle( )
    Set the CC1101 into IDLE mode
INPUT    : None
OUTPUT   : None
================================================================================
*/
void CC1101SetIdle( void )
{
    CC1101WriteCmd(CC1101_SIDLE);
}
/*
================================================================================
Function : CC1101ClrTXBuff( )
    Flush the TX buffer of CC1101
INPUT    : None
OUTPUT   : None
================================================================================
*/
void CC1101ClrTXBuff( void )
{
    CC1101SetIdle();//MUST BE IDLE MODE
    CC1101WriteCmd( CC1101_SFTX );
}
/*
================================================================================
Function : CC1101ClrRXBuff( )
    Flush the RX buffer of CC1101
INPUT    : None
OUTPUT   : None
================================================================================
*/
void CC1101ClrRXBuff( void )
{
    CC1101SetIdle();//MUST BE IDLE MODE
    CC1101WriteCmd( CC1101_SFRX );
}
/*
================================================================================
Function : CC1101SendPacket( )
    Send a packet
INPUT    : txbuffer, The buffer stores data to be sent
           size, How many bytes should be sent
           mode, Broadcast or address check packet
OUTPUT   : None
================================================================================
*/
void CC1101SendPacket( u8 *txbuffer, u8 size, TX_DATA_MODE mode )
{
    u8 address;
    if( mode == BROADCAST )             { address = 0; }
    else if( mode == ADDRESS_CHECK )    { address = CC1101ReadReg( CC1101_ADDR ); }

    CC1101ClrTXBuff( );
    
    if( ( CC1101ReadReg( CC1101_PKTCTRL1 ) & ~0x03 ) != 0 )
    {
        CC1101WriteReg( CC1101_TXFIFO, size + 1 );
        CC1101WriteReg( CC1101_TXFIFO, address );
    }
    else
    {
        CC1101WriteReg( CC1101_TXFIFO, size );
    }

    CC1101WriteMultiReg( CC1101_TXFIFO, txbuffer, size );
    CC1101SetTRMode( TX_MODE );
    while( GPIO_ReadInputDataBit( GPIOA, GPIO_Pin_2 ) != 0 );
    while( GPIO_ReadInputDataBit( GPIOA, GPIO_Pin_2 ) == 0 );

    CC1101ClrTXBuff( );
}
/*
================================================================================
Function : CC1101GetRXCnt( )
    Get received count of CC1101
INPUT    : None
OUTPUT   : How many bytes hae been received
================================================================================
*/
u8 CC1101GetRXCnt( void )
{
    return ( CC1101ReadStatus( CC1101_RXBYTES )  & BYTES_IN_RXFIFO );
}
/*
================================================================================
Function : CC1101SetAddress( )
    Set the address and address mode of the CC1101
INPUT    : address, The address byte
           AddressMode, the address check mode
OUTPUT   : None
================================================================================
*/
void CC1101SetAddress( u8 address, ADDR_MODE AddressMode)
{
    u8 btmp = CC1101ReadReg( CC1101_PKTCTRL1 ) & ~0x03;
    CC1101WriteReg(CC1101_ADDR, address);
    if     ( AddressMode == BROAD_ALL )     {}
    else if( AddressMode == BROAD_NO  )     { btmp |= 0x01; }
    else if( AddressMode == BROAD_0   )     { btmp |= 0x02; }
    else if( AddressMode == BROAD_0AND255 ) { btmp |= 0x03; }   
}
/*
================================================================================
Function : CC1101SetSYNC( )
    Set the SYNC bytes of the CC1101
INPUT    : sync, 16bit sync 
OUTPUT   : None
================================================================================
*/
void CC1101SetSYNC( u16 sync )
{
    CC1101WriteReg(CC1101_SYNC1, 0xFF & ( sync>>8 ) );
    CC1101WriteReg(CC1101_SYNC0, 0xFF & sync ); 
}
/*
================================================================================
Function : CC1101RecPacket( )
    Receive a packet
INPUT    : rxBuffer, A buffer store the received data
OUTPUT   : 1:received count, 0:no data
================================================================================
*/
u8 CC1101RecPacket( u8 *rxBuffer )
{
    u8 status[2];
    u8 pktLen;

    if ( CC1101GetRXCnt( ) != 0 )//读取状态
    {
        pktLen = CC1101ReadReg(CC1101_RXFIFO);           // Read length byte
        if( ( CC1101ReadReg( CC1101_PKTCTRL1 ) & ~0x03 ) != 0 )
        {
            CC1101ReadReg(CC1101_RXFIFO);
        }
        if( pktLen == 0 )           { return 0; }
        else                        { pktLen --; }
        CC1101ReadMultiReg(CC1101_RXFIFO, rxBuffer, pktLen); // Pull data
        CC1101ReadMultiReg(CC1101_RXFIFO, status, 2);   // Read  status bytes

        CC1101ClrRXBuff( );

        if( status[1] & CRC_OK ) {   return pktLen; }
        else                     {   return 0; }
    }
    else   {  return 0; }                               // Error
}
/*
================================================================================
Function : CC1101Init( )
    Initialize the CC1101, User can modify it
INPUT    : None
OUTPUT   : None
================================================================================
*/
void CC1101Init( void )
{
    volatile u8 i, j;

    CC1101Reset( );    
    
    for( i = 0; i < 24; i++ )//使用数组配置
    {
        CC1101WriteReg( CC1101InitData[i][0], CC1101InitData[i][1] );
    }
    CC1101SetAddress( 0x05, BROAD_0AND255 );
    CC1101SetSYNC( 0x8799 );
    CC1101WriteReg(CC1101_MDMCFG1,   0x72); //调制解调器配置
    CC1101WriteMultiReg(CC1101_PATABLE, PaTabel, 8 );//连续写入寄存器

    i = CC1101ReadStatus( CC1101_PARTNUM );//for test, must be 0x80
    i = CC1101ReadStatus( CC1101_VERSION );//for test, refer to the datasheet
}


/*
================================================================================
------------------------------------THE END-------------------------------------
================================================================================
*/
