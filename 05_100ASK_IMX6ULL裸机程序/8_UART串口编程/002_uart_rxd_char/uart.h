#ifndef _UART_H_
#define _UART_H_


/*UART1的寄存器的基地址*/
#define UART1_BASE          (0x2020000u)

#define UART1    ((UART_Type *)UART1_BASE)

/*根据IMX6ULL芯片手册<<55.15 UART Memory Map/Register Definition>>的3608页，定义UART的结构体,*/
typedef struct {
  volatile unsigned int  URXD;               /**< UART Receiver Register, offset: 0x0 	           串口接收寄存器，偏移地址0x0     */
  		   unsigned char RESERVED_0[60];		
  volatile unsigned int  UTXD;               /**< UART Transmitter Register, offset: 0x40          串口发送寄存器，偏移地址0x40*/
  		   unsigned char RESERVED_1[60];		
  volatile unsigned int  UCR1;               /**< UART Control Register 1, offset: 0x80 	       串口控制寄存器1，偏移地址0x80*/
  volatile unsigned int  UCR2;               /**< UART Control Register 2, offset: 0x84 	       串口控制寄存器2，偏移地址0x84*/
  volatile unsigned int  UCR3;               /**< UART Control Register 3, offset: 0x88            串口控制寄存器3，偏移地址0x88*/
  volatile unsigned int  UCR4;               /**< UART Control Register 4, offset: 0x8C            串口控制寄存器4，偏移地址0x8C*/
  volatile unsigned int  UFCR;               /**< UART FIFO Control Register, offset: 0x90         串口FIFO控制寄存器，偏移地址0x90*/
  volatile unsigned int  USR1;               /**< UART Status Register 1, offset: 0x94             串口状态寄存器1，偏移地址0x94*/
  volatile unsigned int  USR2;               /**< UART Status Register 2, offset: 0x98             串口状态寄存器2，偏移地址0x98*/
  volatile unsigned int  UESC;               /**< UART Escape Character Register, offset: 0x9C     串口转义字符寄存器，偏移地址0x9C*/
  volatile unsigned int  UTIM;               /**< UART Escape Timer Register, offset: 0xA0         串口转义定时器寄存器 偏移地址0xA0*/
  volatile unsigned int  UBIR;               /**< UART BRM Incremental Register, offset: 0xA4      串口二进制倍率增加寄存器 偏移地址0xA4*/
  volatile unsigned int  UBMR;               /**< UART BRM Modulator Register, offset: 0xA8 	   串口二进制倍率调节寄存器 偏移地址0xA8*/
  volatile unsigned int  UBRC;               /**< UART Baud Rate Count Register, offset: 0xAC      串口波特率计数寄存器 偏移地址0xAC*/
  volatile unsigned int  ONEMS;              /**< UART One Millisecond Register, offset: 0xB0      串口一毫秒寄存器 偏移地址0xB0*/
  volatile unsigned int  UTS;                /**< UART Test Register, offset: 0xB4                 串口测试寄存器 偏移地址0xB4*/		
  volatile unsigned int  UMCR;               /**< UART RS-485 Mode Control Register, offset: 0xB8  串口485模式控制寄存器 偏移地址0xB8*/
} UART_Type;
		  			 		  						  					  				 	   		  	  	 	  
/**********************************************************************
 * 函数名称： Uart_Init
 * 功能描述： 初始化UART，就是规定传输格式，设置波特率为115200，配置UART占用的GPIO管脚
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/02/16	     V1.0	  zh(angenao)	      创建
 ***********************************************************************/
void Uart_Init(void);		

 /**********************************************************************
 * 函数名称： PutChar
 * 功能描述： 从串口输出单个字符到上位机
 * 输入参数：单个字符
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/02/16	     V1.0	  zh(angenao)	      创建
 ***********************************************************************/
void PutChar(int c);

 #endif
