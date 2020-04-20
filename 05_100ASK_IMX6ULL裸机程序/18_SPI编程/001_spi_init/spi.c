#include "spi.h"
#include "my_printf.h"

/*spi3对应的iomux基址*/
#define UART2_TX_BASE	0x20e0094
#define UART2_RX_BASE	0x20e0098
#define UART2_RTS_BASE	0x20e009C
#define UART2_CTS_BASE	0x20e00A0
		  			 		  						  					  				 	   		  	  	 	  
static volatile unsigned int *GPIO1_GDIR                             ;
static volatile unsigned int *GPIO1_DR                               ;
/**********************************************************************
	 * 函数名称： iomuxc_sw_set
	 * 功能描述： 多功能引脚IO模式设置
	 * 输入参数：@base :要设置的引脚基址
	 			@uc_mode:引脚要设置的模式，值为0/1/2/3/4/5/6/7/8，具体查询手册确定
	 * 输出参数：无
	 * 返 回 值： 无
	 * 修改日期 	   版本号	 修改人		  修改内容
	 * -----------------------------------------------
	 * 2020/02/20		 V1.0	  芯晓		  创建
 ***********************************************************************/

static void iomuxc_sw_set(unsigned int base,unsigned int uc_mode)
{
	*((volatile unsigned int *)base)  = uc_mode&0x000f;
}
/**********************************************************************
 * 函数名称： spi_init
 * 功能描述： spi初始化，包括引脚等 默认设置频率为1M，，，，，
 * 输入参数： @uc_num :要控制的spi接口的基址
 * 输出参数：初始化结果
 * 返 回 值：  返回0，表示函数正常返回
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 *  2020/02/20		 V1.0	  xy(xinyang)		  创建
 ***********************************************************************/
unsigned char  spi_init(SPI_CTRL *uc_num)
{
	/*
		1、清除CONREG寄存器的EN位 来复位模块
		2、在ccm中使能spi时钟
		3、配置control register，然后设置CONREG的EN位来使spi模块退出复位
		4、配置spi对应的IOMUX引脚
		5、根据外部spi设备规格来合适的配置spi寄存器
		
	*/
		printf("spi 初始化开始\n\r") ; 

	/**/
	uc_num->CONREG =  0;// clear all bits
	/*
		bit0:使能SPI
		bit3:写入TXDATA之后，立即发送
		bit4:设置通道0为master mode
		bit31:20 设置burst length ，7表示为8bits，一个字节
	*/
	uc_num->CONREG |= (7<<20)|(1<<4)|(1<<3)|(1<<0);
	/*	CONFIGREG采用默认设置
		*
		*bit0 		PHA=0
		*bit7:4 	sclk高电平有效
		*bit11:8	通道片选信号，当SMC =1的时候，无效（当前处于SMC=1模式）
		*bit15:12	POL=0
		*bit19:16	数据线空闲为高电平
		*bit23:20	sclk空闲为低电平
		*bit28:24	设置消息长度 ，该产品不进行使用
		*
	*/
	uc_num->CONFIGREG = 0;//	
	/*设置时钟相关的*/
	/*  
	从RM手册chapter18中，我们得知时钟来源为PLL3
	1、pll3_sw_clk_sel为0，则选择pll3；为1则选择ccm_pll3_bys，时钟   默认选择pll3 。输出pll3_sw_clk给spi进行使用  输出给spi的时钟为480M/8=60Mhz
	2、我们需要使能spi的时钟进行使用，通过CCM_CCGR1的bit5:2来进行设置 这部分在制作.imx文件的时候初始化，可以不处理
	3、计算时钟频率 CONREG寄存器
		bit15:12 div_1 
		bit11:8	div_2
	最终提供给spip的时钟为
	60M/(div+1)*(2^div_2))
	假设我们要使用的时钟是4M
	则我们设置bit15:12 = 15即可  60M/4 = 15Mhz	
	*/
	uc_num->CONREG &= ~(0xf<<12|0xf<<8);//清除原先的时钟频率设置
	uc_num->CONREG |= (14<<12); //设置clk = 60/(14+1) = 4M
	printf("spi 初始化结束\n\r"); 
	//引脚初始化
	iomuxc_sw_set(UART2_TX_BASE,5);//设置为GPIO作为片选来进行使用。GPIO1_IO20
	GPIO1_GDIR  = (volatile unsigned int *)(0x209C000 + 0x4);
	GPIO1_DR  = (volatile unsigned int *)(0x209C000);
	*GPIO1_GDIR |= (1<<20);//设置为输出
	*GPIO1_DR |= (1<<20);	
//	iomuxc_sw_set(UART2_TX_BASE,8);
	iomuxc_sw_set(UART2_RX_BASE,8);
	iomuxc_sw_set(UART2_RTS_BASE,8);
	iomuxc_sw_set(UART2_CTS_BASE,8);
	return 0;
}
/**********************************************************************
 * 函数名称： spi_select
 * 功能描述： spi片选拉低，GPIO来实现，
 * 输入参数： 无
 * 输出参数：无
 * 返 回 值： 
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 *  2020/02/20		 V1.0	  xy(xinyang)		  创建
 ***********************************************************************/
		  			 		  						  					  				 	   		  	  	 	  
void spi_select(void)
{
	*GPIO1_DR &= ~(1<<20);
}
/**********************************************************************
 * 函数名称： spi_select
 * 功能描述： spi片选拉高，GPIO来实现，
 * 输入参数： 无
 * 输出参数：无
 * 返 回 值： 
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 *  2020/02/20		 V1.0	  xy(xinyang)		  创建
 ***********************************************************************/
void spi_deselect(void)
{
	*GPIO1_DR |= (1<<20);
}
/**********************************************************************
 * 函数名称： spi_writeread
 * 功能描述： spi输入和输出数据
 * 输入参数： @SPI_CTRL SPI控制器基址
 			@uc_txdata 要发送的数据
 * 输出参数：读出的数据
 * 返 回 值： 
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 *  2020/02/20		 V1.0	  xy(xinyang)		  创建
 ***********************************************************************/
unsigned char spi_writeread(SPI_CTRL *spi_num,unsigned char uc_txdata)
{
	/*片选型号*/
	spi_num->CONREG &= ~(3<<18);
	spi_num->CONREG |= 0<<18 ;
	
	while(!(spi_num->STATREG&(1<<0)));//如果FIFO时空的话，则填充数据以开始下一次发送
	spi_num->TXDATA = uc_txdata;

	while(!(spi_num->STATREG&(1<<3)));//等待接收数据完成，当为1的时候表示有接收数据存在，可以进行读取
	return spi_num->RXDATA;
}
