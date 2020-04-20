#include "uart.h"
#include <string.h>


static volatile unsigned int *IOMUXC_SW_MUX_CTL_PAD_UART1_TX_DATA ;
static volatile unsigned int *IOMUXC_SW_MUX_CTL_PAD_UART1_RX_DATA	;
/**********************************************************************
	 * 函数名称： Uart_Init
	 * 功能描述： 串口初始化，包括串口引脚、波特率等相关设置
	 * 输入参数：@uart_x :要设置的串口基址
	 			@uc_baudrate:要设置的串口波特率，如果为1则表示设置成115200，为0表示9600
	 * 输出参数：无
	 * 返 回 值： 无
	 * 修改日期 	   版本号	 修改人		  修改内容
	 * -----------------------------------------------
	 * 2020/02/20		 V1.0	  		  创建
	 * 2020/03/13		 V1.1	  芯晓		  修改     修改为可以向初始化指定串口和波特率
 ***********************************************************************/
void Uart_Init(UART_Type *uart_x,unsigned char uc_baudrate)				
{

	if(uart_x==UART1)
	{
		IOMUXC_SW_MUX_CTL_PAD_UART1_TX_DATA		= (volatile unsigned int *)(0x20E0084);
		IOMUXC_SW_MUX_CTL_PAD_UART1_RX_DATA		= (volatile unsigned int *)(0x20E0088);

		*IOMUXC_SW_MUX_CTL_PAD_UART1_RX_DATA = 0;
		*IOMUXC_SW_MUX_CTL_PAD_UART1_TX_DATA = 0;

	}
	if(uart_x==UART6)
	{
		IOMUXC_SW_MUX_CTL_PAD_UART1_TX_DATA		= (volatile unsigned int *)(0x20E01D4);
		IOMUXC_SW_MUX_CTL_PAD_UART1_RX_DATA		= (volatile unsigned int *)(0x20E01D8);

		*IOMUXC_SW_MUX_CTL_PAD_UART1_RX_DATA = 8;
		*IOMUXC_SW_MUX_CTL_PAD_UART1_TX_DATA = 8;

		IOMUXC_SW_MUX_CTL_PAD_UART1_RX_DATA		= (volatile unsigned int *)(0x20E064C);
		*IOMUXC_SW_MUX_CTL_PAD_UART1_RX_DATA = 03;
		  			 		  						  					  				 	   		  	  	 	  
	}
	uart_x->UCR1 |= (1 << 0) ;		/*关闭当前串口*/ 
	
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
	
	uart_x->UCR2 |= (1<<14) |(1<<5) |(1<<2)|(1<<1);

	/*
	 *  UART1中的UCR3寄存器关键bit如下
	 *  [2]:  1:根据官方文档表示，IM6ULL的UART用了这个MUXED模型，提示要设置	
	 */
	
	uart_x->UCR3 |= (1<<2);
	
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
	if(uc_baudrate==1)  //15200
	{
	    uart_x->UFCR = 5 << 7;       /* Uart的时钟clk：80MHz */
	    uart_x->UBIR = 71;
	    uart_x->UBMR = 3124;
	}
	else  //9600
	{
		uart_x->UFCR = 5 << 7;       /* Uart的时钟clk：80MHz */
	    uart_x->UBIR = 5;
	    uart_x->UBMR = 3124;
	}

	uart_x->UCR1 |= (1 << 0);		/*使能当前串口*/ 
}		  			 		  						  					  				 	   		  	  	 	  
/**********************************************************************
	 * 函数名称： PutChar
	 * 功能描述：串口数据发送
	 * 输入参数：@uart_x :要发送的串口基址
	 			@c:要发送的串口数据
	 * 输出参数：无
	 * 返 回 值： 无
	 * 修改日期 	   版本号	 修改人		  修改内容
	 * -----------------------------------------------
	 * 2020/02/20		 V1.0	  		  创建
	 * 2020/03/13		 V1.1	  芯晓		  修改 修改为可以向指定串口发送数据
 ***********************************************************************/
void PutChar(UART_Type *uart_x,int c)						
{
	while (!((uart_x->USR2) & (1<<3))); /*等待上个字节发送完毕*/
	uart_x->UTXD = (unsigned char)c;		
}
/**********************************************************************
	 * 函数名称： GetChar
	 * 功能描述：串口数据接收
	 * 输入参数：@uart_x :要接收的串口基址
	 * 输出参数：接收到的数据
	 * 返 回 值： 接收到的数据
	 * 修改日期 	   版本号	 修改人		  修改内容
	 * -----------------------------------------------
	 * 2020/02/20		 V1.0	  		  创建
	 * 2020/03/13		 V1.1	  芯晓		  修改 修改为可以从指定串口接收数据
 ***********************************************************************/
unsigned char GetChar(UART_Type *uart_x)						
{	
	while (!(uart_x->USR2 & (1<<0)));  /*等待接收数据*/
	return (unsigned char)uart_x->URXD;
}
/**********************************************************************
	 * 函数名称： PutStr
	 * 输入参数：@uart_x :要发送的串口基址
	 			 @c:要发送的串口数据地址
	 * 输出参数：无
	 * 返 回 值： 无
	 * 修改日期 	   版本号	 修改人		  修改内容
	 * -----------------------------------------------
	 * 2020/02/20		 V1.0	  		  创建
	 * 2020/03/13		 V1.1	  芯晓		  修改 修改为可以向指定串口发送数据
 ***********************************************************************/
void PutStr(UART_Type *uart_x,const char *s)				
{
	while (*s)
	{
		PutChar(uart_x,*s);
		s++;
	}
}		  			 		  						  					  				 	   		  	  	 	  
unsigned char flag_rec_ok = 0;//GPS数据是否接收完成，为1表示成功接收到一帧完整数据，为0表示没有接收到完整数据
unsigned char buf[128];//GPS接收到的数据缓存
/**********************************************************************
	 * 函数名称： GetStr
	 * 输入参数：@uart_x :要发送的串口基址
	 			 @c:要发送的串口数据地址
	 * 输出参数：无
	 * 返 回 值： 无
	 * 修改日期 	   版本号	 修改人		  修改内容
	 * -----------------------------------------------
	 * 2020/03/13		 V1.0	  		  创建
 ***********************************************************************/
void GetStr(UART_Type *uart_x, unsigned char *uc_read_cnt_x)				
{
static	unsigned char uc_read =0;
static	unsigned char uc_rec_cnt=0;
	uc_read = GetChar(uart_x);
	if(uc_read=='$')					//表示一帧GPS数据的开头
	{	
		uc_rec_cnt = 0;	
		flag_rec_ok = 0;	
	}		
	buf[uc_rec_cnt]=uc_read;
	uc_rec_cnt++;
	PutChar(UART1,uc_read); //打印得到的GPS模块传送过来的信息

	if(uc_read ==0x0a)					//表示一帧GPS数据的结尾
	{
		PutChar(UART1,'S');				//发送特定数据，表示收到 一帧数据
		flag_rec_ok = 1;				//表示收到一帧完整的数据
		buf[uc_rec_cnt]='\0';			//将缓存末尾清0
		*uc_read_cnt_x = uc_rec_cnt;	//传递接收到的帧数据长度
	}
}
/**********************************************************************
	 * 函数名称： printrawdata
	 * 输入参数：	 @uc_buf:要发送的串口数据地址
	 * 输出参数：无
	 * 返 回 值： 无
	 * 修改日期 	   版本号	 修改人		  修改内容
	 * -----------------------------------------------
	 * 2020/03/13		 V1.0	  		  创建
 ***********************************************************************/
void printrawdata(unsigned char *uc_buf)
{
	PutStr(UART1,uc_buf);//打印数据内容
}
/**********************************************************************
	 * 函数名称： get_pos_dot
	 * 输入参数：	 @uc_buf:原始数据缓存地址
	 				@uc_cnt：要得到第几个逗号的位置
	 * 输出参数：查询到的指定位置逗号的数据地址
	 * 返 回 值： 查询到的指定位置逗号的数据地址
	 * 修改日期 	   版本号	 修改人		  修改内容
	 * -----------------------------------------------
	 * 2020/03/13		 V1.0	  		  创建
 ***********************************************************************/
unsigned char  get_pos_dot(unsigned char *uc_buf,unsigned char uc_cnt)
{
	unsigned char *buf_x;
	buf_x = uc_buf;
	while(uc_cnt)
	{
		if(*buf_x==',') uc_cnt--;//如果是逗号，则减一
		buf_x++;//缓存地址+1
	}
	return buf_x-uc_buf;
	 
}		  			 		  						  					  				 	   		  	  	 	  
/**********************************************************************
	 * 函数名称： print_nmea_get_data 解析GPS帧信息里第N个和N+1之间的逗号内容，
	 * 输入参数：	 @uc_buf:要发送的串口数据地址
	 				@uc_posi 解析第几个逗号后的内容
	 * 输出参数：无
	 * 返 回 值： 无
	 * 修改日期 	   版本号	 修改人		  修改内容
	 * -----------------------------------------------
	 * 2020/03/13		 V1.0	  		  创建 
***********************************************************************/
unsigned char print_nmea_get_data(unsigned char *uc_buf,unsigned char uc_posi)
{
		unsigned char uc_cnt =0;
		unsigned char uc_pos[24];
		unsigned char uc_pos_x =0;
		unsigned char uc_pos_y =0;
		unsigned char uc_len = 0;
	
		memset(uc_pos,24,0);
		uc_pos_x=get_pos_dot(uc_buf,uc_posi);//得到指定位置的逗号位置
		uc_pos_y=get_pos_dot(uc_buf,uc_posi+1);//得到指定位置+1的逗号位置

		uc_len  = uc_pos_y-uc_pos_x;//计算两个逗号之间的数据长度
		
		for(uc_cnt=0;uc_cnt<uc_len;uc_cnt++)//将两个逗号之间的内容进行拷贝，就是实际要获取的相关内容
			uc_pos[uc_cnt]=uc_buf[uc_cnt+uc_pos_x];

		uc_pos[uc_len-1]='\0';		
		PutStr(UART1,uc_pos);
		
}
/**********************************************************************
	 * 函数名称： anglysysrawdata 解析接收到的数据，GPGGA信息，包括经纬度等
	 * 输入参数：	 @uc_buf:接收到的GPS帧数据缓存地址
	 * 输出参数：无
	 * 返 回 值： 无
	 * 修改日期 	   版本号	 修改人		  修改内容
	 * -----------------------------------------------
	 * 2020/03/13		 V1.0	  		  创建
 ***********************************************************************/
void anglysysrawdata(unsigned char *uc_buf)
{/*提取定位信息*/

	unsigned char uc_pos_x =0;
	unsigned char uc_pos_y =0;

	uc_pos_x=get_pos_dot(uc_buf,2);
	uc_pos_y=get_pos_dot(uc_buf,3);	
	PutStr(UART1,uc_buf);//打印得到的原始数据
	PutStr(UART1,"\n\r");
	PutChar(UART1,'@');
		
/*$GPGGA,082559.00,4005.22599,N,11632.58234,E,1,04,3.08,14.6,M,-5.6,M,,*76"*/	
	if(uc_pos_x==8)//表示当前内容不对，为空内容，没有得到任何有效GPS内容
	{////$GPGLL,,,,,,V,N*64这样的就是属于无效内容 可以看到第二个逗号位置在第八个位置
		PutStr(UART1,"Invalid data, Please move the module to the open area and wait 3-5 minutes.\n\r");
	}
	else	
	{//表示得到有效内容，比如UTC时间等
		PutStr(UART1,"@UTC Time:");
		print_nmea_get_data(uc_buf,1);
		PutStr(UART1,"\n\r");
		PutStr(UART1,"@定位卫星数量:");
		print_nmea_get_data(uc_buf,7);
		PutStr(UART1,"\n\r");
		PutStr(UART1,"@是否成功定位，0表示没有定位，非0表示定位成功:");
		print_nmea_get_data(uc_buf,6);
		PutStr(UART1,"\n\r");
		PutStr(UART1,"@纬度:");
		print_nmea_get_data(uc_buf,3);
		print_nmea_get_data(uc_buf,2);
		PutStr(UART1,"\n\r");
		PutStr(UART1,"@经度:");
		print_nmea_get_data(uc_buf,5);
		print_nmea_get_data(uc_buf,4);	
		PutStr(UART1,"\n\r");
		PutStr(UART1,"@海拔高度:");
		print_nmea_get_data(uc_buf,8);
		PutStr(UART1,"\n\r");
	
	}
	
}




