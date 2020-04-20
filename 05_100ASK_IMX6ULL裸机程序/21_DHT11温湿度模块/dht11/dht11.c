#include "timer.h"
#include "my_printf.h"

static volatile unsigned int *CCM_CCGR3 							 ;
static volatile unsigned int *IOMUXC_SW_MUX_CTL_PAD_CSI_VSYNC        ;
static volatile unsigned int *GPIO4_GDIR							 ;
static volatile unsigned int *GPIO4_DR								 ;

/**********************************************************************
 * 函数名称： dht11_gpio_init
 * 功能描述： 初始化GPIO管脚
 * 输入参数： 无
 * 输出参数： 1-高电平 0-低电平
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/03/23	     V1.0	  来木	      创建
 ***********************************************************************/
static void dht11_gpio_init(void)
{
	unsigned int val;

	CCM_CCGR3								= (volatile unsigned int *)(0x20C4074);
	IOMUXC_SW_MUX_CTL_PAD_CSI_VSYNC			= (volatile unsigned int *)(0x20E01DC);
	GPIO4_GDIR								= (volatile unsigned int *)(0x20A8000 + 0x4);
	GPIO4_DR								= (volatile unsigned int *)(0x20A8000);
	
	/* 使能GPIO4
	 * set CCM to enable GPIO4
	 * CCM_CCGR3[CG6] 0x20C4074
	 * bit[13:12] = 0b11
	 */
	*CCM_CCGR3 |= (3<<12);
	
	/* 设置GPIO4_IO19用于GPIO
	 * set IOMUXC_SW_MUX_CTL_PAD_CSI_VSYNC
	 *      to configure GPIO4_IO19 as GPIO
	 * IOMUXC_SW_MUX_CTL_PAD_CSI_VSYNC  0x20E01DC
	 * bit[3:0] = 0b0101 alt5
	 */
	val = *IOMUXC_SW_MUX_CTL_PAD_CSI_VSYNC;
	val &= ~(0xf);
	val |= (5);
	*IOMUXC_SW_MUX_CTL_PAD_CSI_VSYNC = val;
}

/**********************************************************************
 * 函数名称： dht11_gpio_as_input
 * 功能描述： 设置GPIO4_19为输入引脚
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/03/23	     V1.0	  来木	      创建
 ***********************************************************************/
static void dht11_gpio_as_input(void)
{	
	/*  
	 * 设置GPIO4_IO19作为input引脚
	 * set GPIO4_GDIR to configure GPIO4_IO19 as input
	 * GPIO4_GDIR  0x20A8000
	 * bit[1] = 0b0
	 */
	 *GPIO4_GDIR &= ~(1<<19);
}

/**********************************************************************
 * 函数名称： dht11_gpio_as_output
 * 功能描述： 设置GPIO4_19为输出引脚
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/03/23	     V1.0	  来木	      创建
 ***********************************************************************/
static void dht11_gpio_as_output(void)
{		
	/*  
	 * 设置GPIO4_IO19作为output引脚
	 * set GPIO4_GDIR to configure GPIO4_IO19 as output
	 * GPIO4_GDIR  0x20A8000
	 * bit[1] = 0b1
	 */
	 *GPIO4_GDIR |= (1<<19);
}

/**********************************************************************
 * 函数名称： dht11_data_set
 * 功能描述： GPIO4_19管脚输出电平
 * 输入参数： 1-高电平 0-低电平
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/03/23	     V1.0	  来木	      创建
 ***********************************************************************/
static void dht11_data_set(int val)
{
	if(val)
		*GPIO4_DR |= (1<<19);
	else
		*GPIO4_DR &= ~(1<<19);
}

/**********************************************************************
 * 函数名称： dht11_data_get
 * 功能描述： 读取GPIO4_19管脚电平
 * 输入参数： 无
 * 输出参数： 1-高电平 0-低电平
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/03/23	     V1.0	  来木	      创建
 ***********************************************************************/
static int dht11_data_get(void)
{
	if((*GPIO4_DR>>19) & 0x1)
		return 1;
	else
		return 0;
}

/**********************************************************************
 * 函数名称： dht11_init
 * 功能描述： 初始化dht11，跳过dht11上电后不稳定状态
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/03/23	     V1.0	  来木	      创建
 ***********************************************************************/
static void dht11_init(void)
{
	dht11_gpio_as_output();
	dht11_data_set(1);
	gpt2_chan1_delay_us(2000000);
}

/**********************************************************************
 * 函数名称： dht11_start
 * 功能描述： 给dht11发出开始信号
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/03/23	     V1.0	  来木	      创建
 ***********************************************************************/
static void dht11_start(void)
{
	dht11_data_set(0);
	gpt2_chan1_delay_us(20000);
}

/**********************************************************************
 * 函数名称： dht11_wait_for_val
 * 功能描述： 等待响应信号结束
 * 输入参数： 电平(1--高电平  0--低电平) 延时时间 
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/03/23	     V1.0	  来木	      创建
 ***********************************************************************/
static int dht11_wait_for_val(int val, int timeout_us)
{
	while(timeout_us--)
	{
		if(dht11_data_get() == val)
			return 0;
		gpt2_chan1_delay_us(1);
	}
	return -1;
}

/**********************************************************************
 * 函数名称： dht11_recv_byte
 * 功能描述： 读取一个字节数据
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/03/23	     V1.0	  来木	      创建
 ***********************************************************************/
static unsigned char dht11_recv_byte(void)
{
	int i;
	int data = 0;
	 
	for (i = 0; i < 8; i++)
	{
		if (dht11_wait_for_val(1, 1000))
	  	{
	   		printf("dht11 wait for high data err!\n\r");
	   		return -1;
	  	}
	  	gpt2_chan1_delay_us(40);
	  	data <<= 1;				//数据左移一位
	  	if (dht11_data_get() == 1)	//如果引脚上是高电平，数据置1
	   		data |= 1;				//否则数据左移自动补0
	  
	  	if (dht11_wait_for_val(0, 1000))
	  	{
	  		printf("dht11 wait for low data err!\n\r");
	   		return -1;
	  	}
	}
	 
	 return data;
}

/**********************************************************************
 * 函数名称： dht11_data_read
 * 功能描述： 读取dht11数据
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/03/23	     V1.0	  来木	      创建
 ***********************************************************************/
void dht11_data_read(void)
{
	unsigned char val[5],tmp;
	dht11_gpio_init();						//初始化GPIO管脚
	dht11_init();							//初始化dht11
	dht11_start();							//发出开始信号
	dht11_data_set(1);						//主机拉高并延时20~40us	
	gpt2_chan1_delay_us(40);
	dht11_gpio_as_input();
	if(!dht11_data_get())					//判断是否接收到dht11响应信号
	{
		while(!dht11_data_get());			//等待响应信号结束
		while(dht11_data_get());			//dht11拉高80us，等待80us
		val[0] = dht11_recv_byte();			//读取8bit湿度整数数据
		val[1] = dht11_recv_byte();			//读取8bit湿度小数数据
		val[2] = dht11_recv_byte();			//读取8bit温度整数数据
		val[3] = dht11_recv_byte();			//读取8bit温度小数数据
		val[4] = dht11_recv_byte();			//读取8bit校验和
		tmp = val[0] + val[1] + val[2] + val[3];
		if(tmp==val[4])						//校验通过，通过串口打印数据
		{
			printf("Humidity    = %d.%d\n\r", val[0], val[1]);
			printf("Temperature = %d.%d\n\r\n\r", val[2], val[3]);
 		}
	}
}
