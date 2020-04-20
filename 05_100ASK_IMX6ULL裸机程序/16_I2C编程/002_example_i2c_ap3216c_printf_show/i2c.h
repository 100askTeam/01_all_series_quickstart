#ifndef _I2C_H
#define _I2C_H

#include "type.h"

/* 
 *IC2状态码，方便通过返回值对程序返回处进行定位
 */
#define I2C_OK				     (0)
#define I2C_ERROR                (1)
#define I2C_BUSY				 (2)
#define I2C_IDLE				 (3)
#define I2C_NAK				     (4)
#define I2C_ARBITRATIONLOST	     (5)
#define I2C_TIMEOUT			     (6)
#define I2C_ADDRNAK			     (7)

/*
 *The I2C contains five 16-bit registers.
 */
/*  绝对地址 寄存器名                                    位宽(bit) 权限 复位值 章节/页
 * 21A_0000  I2C Address Register (I2C1_IADR)           16       R/W  0000h 31.7.1/1463
 * 21A_0004  I2C Frequency Divider Register (I2C1_IFDR) 16       R/W  0000h 31.7.2/1463
 * 21A_0008  I2C Control Register (I2C1_I2CR)           16       R/W  0000h 31.7.3/1465
 * 21A_000C  I2C Status Register (I2C1_I2SR)            16       R/W  0081h 31.7.4/1466
 * 21A_0010  I2C Data I/O Register (I2C1_I2DR)          16       R/W  0000h 31.7.5/1468
 */
/*目前仅用I2C1即可，后面使用2-4可以参考如下定义，通过对I2C1-4有序号的宏的命名来区分I2C寄存器组，完成通用接口的编写后，后面使用其他组寄存器仅需要把带序号的宏补齐，函数可以通用*/
#define I2C1_BASE_ADDR                                (0x21A0000u)
/*  I2C1 Base address */
#define I2C1                                     ((I2C_REGISTERS *)I2C1_BASE_ADDR)
#define I2C2_BASE_ADDR                                (0x21A4000u)
/*  I2C1 Base address */
#define I2C2                                     ((I2C_REGISTERS *)I2C2_BASE_ADDR)

/* 寄存器地址的宏结构体定义，此种方式仅定义入口地址即可 */
/* all registers address is Base address + xh offset*/
typedef struct tagRegisters{
  volatile uint16_t IADR;                              /*I2C Address Register, offset: 0x0 */
           uint8_t ReservedIADR[2];
  volatile uint16_t IFDR;                              /*I2C Frequency Divider Register, offset: 0x4 */
           uint8_t ReservedIFDR[2];
  volatile uint16_t I2CR;                              /*I2C Control Register, offset: 0x8 */
           uint8_t ReservedI2CR[2];
  volatile uint16_t I2SR;                              /*I2C Status Register, offset: 0xC */
           uint8_t ReservedI2SR[2];
  volatile uint16_t I2DR;                              /*I2C Data I/O Register, offset: 0x10 */
} I2C_REGISTERS;

/*
 * IC2操作码定义
 */
typedef enum enI2C_OPCODE
{
    I2C_WRITE = 0,            /* 主机向从机写数据 */
    I2C_READ  = 1,  		/* 主机从从机读数据 */
    I2C_DONOTHING_BULL
} I2C_OPCODE;

/*
 * 主机传输结构体
 */
typedef struct tagI2cTransfer
{
    uint8_t  ucSlaveAddress;      	     /* 7位从机地址 */
    uint32_t ulOpcode  ; 		     /* 操作码*/
    uint32_t ulSubAddress;       		/* 目标寄存器地址 */
    uint8_t  ulSubAddressLen;    	     /* 寄存器地址长度 */
    volatile uint32_t ulLenth;  	     /* 数据长度 */
    uint8_t *volatile pbuf;    	     /* 数据*/
} I2C_TRANSFER;


/**********************************************************************
 * 函数名称： i2c_init
 * 功能描述： 初始化i2c
 * 输入参数： 1.I2C的基地址
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/02/22	     V1.0	    小火山	      创建
 ***********************************************************************/
void i2c_init(I2C_REGISTERS *I2C_BASE);

/**********************************************************************
 * 函数名称： i2c_check
 * 功能描述： 检查并清除错误
 * 输入参数： 1.I2C的基地址 2.传输结构体
 * 输出参数： 无
 * 返 回 值： 状态结果
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/02/22	     V1.0	    小火山	      创建
 ***********************************************************************/

uint8_t i2c_check(I2C_REGISTERS *I2C_BASE, uint32_t status);

/**********************************************************************
 * 函数名称： i2c_start
 * 功能描述： 发送开始信号
 * 输入参数： 1.I2C的基地址2.设备地址3.操作码
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/02/22	     V1.0	    小火山	      创建
 ***********************************************************************/

uint8_t i2c_start(I2C_REGISTERS *I2C_BASE, uint8_t ucSlaveAddr, uint32_t ulOpcode);
/**********************************************************************
 * 函数名称： i2c_stop
 * 功能描述： 停止信号
 * 输入参数： 1.I2C的基地址  
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/02/22	     V1.0	    小火山	      创建
 ***********************************************************************/

uint8_t i2c_stop(I2C_REGISTERS *I2C_BASE);
/**********************************************************************
 * 函数名称： i2c_restart
 * 功能描述： 发送重新开始信号
 * 输入参数： 1.I2C的基地址 2.传输结构体
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/02/22	     V1.0	    小火山	      创建
 ***********************************************************************/

uint8_t i2c_restart(I2C_REGISTERS *I2C_BASE, uint8_t ucSlaveAddr, uint32_t ulOpcode);

/**********************************************************************
 * 函数名称： i2c_write
 * 功能描述： i2c master 写操作
 * 输入参数： 1.I2C的基地址 2.数据 3.数据长度
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/02/22	     V1.0	    小火山	      创建
 ***********************************************************************/
void i2c_write(I2C_REGISTERS *I2C_BASE, const uint8_t *pbuf, uint32_t len);


/**********************************************************************
 * 函数名称： i2c_read
 * 功能描述： i2c_master 读操作
 * 输入参数： 1.I2C的基地址 2.数据 3.数据长度
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/02/22	     V1.0	    小火山	      创建
 ***********************************************************************/
void i2c_read(I2C_REGISTERS *I2C_BASE, uint8_t *pbuf, uint32_t len);

/**********************************************************************
 * 函数名称： i2c_transfer
 * 功能描述： i2c传输，包括读写功能
 * 输入参数： 1.I2C的基地址 2.传输结构体
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/02/22	     V1.0	    小火山	      创建
 ***********************************************************************/
uint8_t i2c_transfer(I2C_REGISTERS *I2C_BASE, I2C_TRANSFER *transfer);

/**********************************************************************
 * 函数名称： I2C_WriteOneByte
 * 功能描述： 向IC从设备写入数据
 * 输入参数： 1.设备地址 2.要写入寄存器 3.要写入数据
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/02/22	     V1.0	    小火山	      创建
 ***********************************************************************/

uint8_t i2c_write_one_byte(uint8_t addr,uint8_t reg, uint8_t data, I2C_REGISTERS *I2C_BASE);

/**********************************************************************
 * 函数名称： I2C_ReadOneByte
 * 功能描述： 向IC从设备读数据
 * 输入参数： 1.设备地址 2.要读的寄存器
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/02/22	     V1.0	    小火山	      创建
 ***********************************************************************/

uint8_t i2c_read_one_byte(uint8_t addr, uint8_t reg, I2C_REGISTERS *I2C_BASE);

#endif
