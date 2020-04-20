#ifndef _UART_H_
#define _UART_H_


/*UART1的寄存器的基地址*/
#define UART1_BASE          (0x2020000u)

#define UART1    ((UART_Type *)UART1_BASE)

/*根据IMX6ULL芯片手册，定义UART的结构体*/
typedef struct {
  volatile unsigned int  URXD;                              /**< UART Receiver Register, offset: 0x0 */
  		   unsigned char RESERVED_0[60];
  volatile unsigned int  UTXD;                              /**< UART Transmitter Register, offset: 0x40 */
  		   unsigned char RESERVED_1[60];
  volatile unsigned int  UCR1;                              /**< UART Control Register 1, offset: 0x80 */
  volatile unsigned int  UCR2;                              /**< UART Control Register 2, offset: 0x84 */
  volatile unsigned int  UCR3;                              /**< UART Control Register 3, offset: 0x88 */
  volatile unsigned int  UCR4;                              /**< UART Control Register 4, offset: 0x8C */
  volatile unsigned int  UFCR;                              /**< UART FIFO Control Register, offset: 0x90 */
  volatile unsigned int  USR1;                              /**< UART Status Register 1, offset: 0x94 */
  volatile unsigned int  USR2;                              /**< UART Status Register 2, offset: 0x98 */
  volatile unsigned int  UESC;                              /**< UART Escape Character Register, offset: 0x9C */
  volatile unsigned int  UTIM;                              /**< UART Escape Timer Register, offset: 0xA0 */
  volatile unsigned int  UBIR;                              /**< UART BRM Incremental Register, offset: 0xA4 */
  volatile unsigned int  UBMR;                              /**< UART BRM Modulator Register, offset: 0xA8 */
  volatile unsigned int  UBRC;                              /**< UART Baud Rate Count Register, offset: 0xAC */
  volatile unsigned int  ONEMS;                             /**< UART One Millisecond Register, offset: 0xB0 */
  volatile unsigned int  UTS;                               /**< UART Test Register, offset: 0xB4 */
  volatile unsigned int  UMCR;                              /**< UART RS-485 Mode Control Register, offset: 0xB8 */
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
 /**********************************************************************
 * 函数名称： GetChar
 * 功能描述： 通过串口，从上位机获取单个字符
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 返回接收到的字符
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/02/16	     V1.0	  zh(angenao)	      创建
 ***********************************************************************/
unsigned char GetChar(void);	
  /**********************************************************************
 * 函数名称： PutStr
 * 功能描述： 输出字符串，就是连续发送单个字符
 * 输入参数： 字符串
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/02/16	     V1.0	  zh(angenao)	      创建
 ***********************************************************************/
void PutStr(const char *s);

 #endif
