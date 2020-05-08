#include "uart.h"


static volatile unsigned int *IOMUXC_SW_MUX_CTL_PAD_UART1_TX_DATA ;
static volatile unsigned int *IOMUXC_SW_MUX_CTL_PAD_UART1_RX_DATA	;
static volatile unsigned int *IOMUXC_UART1_RX_DATA_SELECT_INPUT ;
/**********************************************************************
 * 函数名称： Uart_Init
 * 功能描述： 初始化UART，就是规定传输格式，设置波特率为115200，配置UART占用的GPIO管脚
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/02/16	     V1.0	  zh(angenao)	      创建
 ***********************************************************************/                          ;
void Uart_Init(void)	 				
{
	
	IOMUXC_SW_MUX_CTL_PAD_UART1_TX_DATA		= (volatile unsigned int *)(0x20E0084);
	IOMUXC_SW_MUX_CTL_PAD_UART1_RX_DATA		= (volatile unsigned int *)(0x20E0088);
	IOMUXC_UART1_RX_DATA_SELECT_INPUT		= (volatile unsigned int *)(0x20E0624);

	*IOMUXC_SW_MUX_CTL_PAD_UART1_RX_DATA = 0;
	*IOMUXC_UART1_RX_DATA_SELECT_INPUT = 3;
	*IOMUXC_SW_MUX_CTL_PAD_UART1_TX_DATA = 0;

	UART1->UCR1 |= (1 << 0) ;		/*关闭当前串口*/ 
	
	/* 
	 *  设置UART传输格式：
	 *  UART1中的UCR2寄存器关键bit如下
	 *  [14]:	1：忽略RTS引脚
	 *  [8] :	0: 关闭奇偶校验 默认为0，无需设置
	 *  [6] :	0: 停止位1位	    默认为0，无需设置
	 *  [5] :	1: 数据长度8位
	 *  [2] :	1: 发送数据使能
	 *  [1] :	1: 接收数据使能
	 */
	
	UART1->UCR2 |= (1<<14) |(1<<5) |(1<<2)|(1<<1);

	/*
	 *  UART1中的UCR3寄存器关键bit如下
	 *  [2]:  1:根据官方文档表示，IM6ULL的UART用了这个MUXED模型，提示要设置	
	 */
	
	UART1->UCR3 |= (1<<2);
	
	/*
	 * 设置波特率
	 * 根据芯片手册得知波特率计算公式:
	 * Baud Rate = Ref Freq / (16 * (UBMR + 1)/(UBIR+1))
	 * 当我们需要设置 115200的波特率
	 * UART1_UFCR [9:7]=101，表示不分频，得到当前UART参考频率Ref Freq ：80M ，
	 * 带入公式：115200 = 80000000 /(16*(UBMR + 1)/(UBIR+1))
	 * 
	 * 选取一组满足上式的参数：UBMR、UBIR即可
	 *	
	 * UART1_UBIR    = 71
	 * UART1_UBMR = 3124  
	 */
	 
    UART1->UFCR = 5 << 7;       /* Uart的时钟clk：80MHz */
    UART1->UBIR = 71;
    UART1->UBMR = 3124;

	UART1->UCR1 |= (1 << 0);		/*使能当前串口*/ 
}
/**********************************************************************
* 函数名称： PutChar
* 功能描述： 从串口输出单个字符到上位机
* 输入参数：单个字符
* 输出参数： 无
* 返 回 值： 无
* 修改日期		  版本号 	修改人		 修改内容
* -----------------------------------------------
* 2020/02/16		V1.0	 zh(angenao)		 创建
***********************************************************************/

void PutChar(int c)						
{
	while (!((UART1->USR2) & (1<<3))); /*等待上个字节发送完毕*/
	UART1->UTXD = (unsigned char)c;		
}
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

unsigned char GetChar(void)						
{	
	while (!(UART1->USR2 & (1<<0)));  /*等待接收数据*/
	return (unsigned char)UART1->URXD;
}
		  			 		  						  					  				 	   		  	  	 	  
