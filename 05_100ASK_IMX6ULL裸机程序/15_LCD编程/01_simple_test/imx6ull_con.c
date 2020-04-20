#include"imx6ull_soc.h"
#include"lcd_controller_manager.h"

/*背光GPIO相关寄存器*/
static volatile unsigned int* GPIO1_IO08_PAD =  (volatile unsigned int*)0x20E0308;  /*背光引脚硬件属性地址*/; 
static volatile unsigned int* GPIO1_GDIR     =   (volatile unsigned int*)0x209C000;
static volatile unsigned int* GPIO1_DR        =   (volatile unsigned int*)0x209C004; 

static void Imx6ull_lcd_io_init(void)
{
	unsigned int* p_tmp;
	unsigned int i = 0 ;

	p_tmp = (unsigned int*)(SW_MUX);
	for(i = 0 ; i < 29 ; i++)		       /*共29个信号需要复用设置*/
	{
		*p_tmp = 0;		   /*29个寄存器设置为0表示LCD复用功能*/
		p_tmp++;
	}	
	/*LCD引脚硬件属性关键bit
     * [16]     :   0  : 施密特特性关闭  
	 * [15:14]  :   0  : 内部上拉22K电阻（默认）
	 * [13]     :   0  : Keeper
	 * [12]     :   0  : 关闭KEEP
	 * [11]     :   0  : 关闭开漏输出
	 * [7:6]    :   2  : 输出频率100Mhz
	 * [5:3]    :   7  : 驱动能力为R0/7 
	 * [0]      :   1  : 适用于高频信号
	 */
	
    p_tmp = (unsigned int*)(SW_PAD);
	for(i = 0 ; i < 29 ; i++)	
	{
		*p_tmp = 0;
		*p_tmp |= 0xB9;
		 p_tmp++;
	}

	/* 背光GPIO初始化 */		  			 		  						  					  				 	   		  	  	 	  
	
	*GPIO1_IO08_PAD     = 0xB9  ;							/*背光引脚硬件属性设置*/

    *GPIO1_GDIR        |= (1<<8);                             /*背光GPIO设置为输出模式*/

	*GPIO1_DR          |= (1<<8);                             /*背光默认关闭*/


}
static void Imx6ull_lcd_controller_init(p_lcd_params plcdparams)
{	
     int bpp_mode;
	 Imx6ull_lcd_io_init();  /*LCD控制器占据的GPIO引脚初始化*/

	 /* 时钟计算公式如下
	  * Video PLL output frequency(PLL5)= Fref * (DIV_SELECT + NUM/DENOM)
	  * PLL5_MAIN_CLK                    = PLL5 / POST_DIV_SELECT / VIDEO_DIV
	  * LCDIF1_CLK_ROOT                  = PLL5_MAIN_CLK /LCDIF1_PRED / LCDIF1_PODF
      * 
      * 输出LCDIF1_CLK_ROOT七寸屏：51.2Mhz
      * DIV_SELECT             = 32 （寄存器相应位设置为32）
      * NUM                    = 0  （寄存器相应位设置为0）
      * DENOM                  = 0  （寄存器相应位设置为0）
      * POST_DIV_SELECT        = 1  （寄存器相应位设置为2）
      * VIDEO_DIV              = 1  （寄存器相应位设置为0）
      * LCDIF1_PRED			   = 3  （寄存器相应位设置为2）
      * LCDIF1_PODF            = 5  （寄存器相应位设置为4）
      *
	  */ 
	  
	 /*
	  *为方便使用 ，不使用小数分频器
	  *NUM	   = 0
	  *DENOM = 0
	  *公式简化为：
	  * Video PLL output frequency(PLL5)= Fref * DIV_SELECT
	  * PLL5_MAIN_CLK                    = PLL5 / POST_DIV_SELECT / VIDEO_DIV
	  * LCDIF1_CLK_ROOT                  = PLL5_MAIN_CLK /LCDIF1_PRED / LCDIF1_PODF
	  */
	  CCM_ANALOG->PLL_VIDEO_NUM   = 0; 	 
	  CCM_ANALOG->PLL_VIDEO_DENOM = 0;	 

	  
	 /* 初始化寄存器：CCM_ANALOG_PLL_VIDEOn
	  * [20:19 ] :  2  : 不分频  
	  * [15:14]  :  0  : PLL5的时钟源为外部24Mhz晶振，默认为零无需设置
	  * [13]	 :  1  : 使能VIDEO PLL时钟
	  * [6:0]	 : 32  : 倍频值（DIV_SELECT）
	  *公式简化为：
	  * Video PLL output frequency(PLL5)= 24Mhz * 32  = 768Mhz
	  * PLL5_MAIN_CLK                    = 768Mhz   / POST_DIV_SELECT / 1
	  * LCDIF1_CLK_ROOT                  = PLL5_MAIN_CLK /LCDIF1_PRED / LCDIF1_PODF
	  */
		 CCM_ANALOG->PLL_VIDEO =  (2 << 19) | (1 << 13) | (32<< 0); 
	 
	 /* 初始化寄存器：CCM_ANALOG_MISC2n
	  * [31:30 ] :  0  : 不分频  
	  *公式简化为：
	  * Video PLL output frequency(PLL5)= 24Mhz * 32  = 768Mhz
	  * PLL5_MAIN_CLK                    = 768Mhz   / 1 / 1
	  * LCDIF1_CLK_ROOT                  = 768Mhz /LCDIF1_PRED / LCDIF1_PODF 
	  * 默认已为零，不需设置
	  * CCM_ANALOG->MISC2 &= ~(3 << 30);
	  */
	  
	 /*
	  *初始化寄存器：CCM_CSCDR2
      * [17:15] :	2  : PLL5_MAIN_CLK为驱动时钟源  
      * [12:14] :	2  : 分频系数为3
      * [11-9 ] :	0  : 时钟源选择LCDIF1_PRED分频后的时钟
      *公式简化为：
	  * Video PLL output frequency(PLL5)= 24Mhz * 32  = 768Mhz
	  * PLL5_MAIN_CLK                    = 768Mhz   / 1 / 1
	  * LCDIF1_CLK_ROOT                  = 768Mhz /3 / LCDIF1_PODF 
	  */
		 CCM->CSCDR2 &= ~(7 << 15); 	   
		 CCM->CSCDR2 |=  (2 << 15);  
		 
		 CCM->CSCDR2 &= ~(7 << 12); 	 
		 CCM->CSCDR2 |=  (2 << 12); 						 
	 
		 CCM->CSCDR2 &= ~(7 << 9);

	 /*
	  *初始化寄存器：CCM_CBCMR
	  *[25:23] :  4  :分频系数LCDIF1_PODF设置为5  
	  *公式简化为：
	  * Video PLL output frequency(PLL5)= 24Mhz * 32  = 768Mhz
	  * PLL5_MAIN_CLK                    = 768Mhz   / 1 / 1
	  * LCDIF1_CLK_ROOT                  = 768Mhz /3 / 5  
	  *
	  *最终得到F1_CLK_ROOT     =51.2Mhz
	  * 
	  */
		 CCM->CBCMR &= ~(7 << 23);					 
		 CCM->CBCMR |=	4 << 23;	/*[25:23] :4 : 表示5分频*/

   	  /* 重新设置时钟后，需要软复位LCD控制器，让LCD控制器像素时钟同步*/
		 LCDIF->CTRL  = 1<<31;   

	  /*软复位需要花费好几个时钟周期，这里需要一些时间等待*/
		 delay(100);

	  /*同步像素时钟结束*/
		LCDIF->CTRL  = 0<<31; /* 取消复位 */


	/*LCD控制寄存器初始化*/
		
	/* 
     * 初始化LCD控制器的CTRL寄存器
     * [19]       :  1      : DOTCLK和DVI modes需要设置为1 
     * [17]       :  1      : 设置为1工作在DOTCLK模式
     * [15:14]    : 00      : 输入数据不交换（小端模式）默认就为0，不需设置
     * [13:12]    : 00      : CSC数据不交换（小端模式）默认就为0，不需设置
     * [11:10]    : 11		: 数据总线为24bit
     * [9:8]    根据显示屏资源文件bpp来设置：8位0x1 ， 16位0x0 ，24位0x3
     * [5]        :  1      : 设置elcdif工作在主机模式
     * [1]        :  0      : 24位数据均是有效数据，默认就为0，不需设置
	 */
     bpp_mode = plcdparams->bpp == 8  ? 0x1 :(plcdparams->bpp == 16 ? 0x0 : 0x3);  /* 0x3: 24bpp or 32bpp */
     
	 LCDIF->CTRL |= (1 << 19) | (1 << 17) |(3 << 10) | (bpp_mode << 8) | (1 << 5) ;

     /* [3]当bpp为16时，数据格式为ARGB555*/
	 if(plcdparams->bpp == 16)
	 {
	 	LCDIF->CTRL |= 1<<3;
	 }
	 /*
      * 设置ELCDIF的寄存器CTRL1
      * 根据bpp设置，bpp为24或32才设置
      * [19:16]  : 111  :表示ARGB传输格式模式下，传输24位无压缩数据，A通道不用传输）
	  */	
	 if(plcdparams->bpp == 24 || plcdparams->bpp == 32)
	 {		
	 		LCDIF->CTRL1 &= ~(0xf << 16); 
		 	LCDIF->CTRL1 |=  (0x7 << 16); 
	 }
	 	
	  /*
      * 设置ELCDIF的寄存器TRANSFER_COUNT寄存器
      * [31:16]  : 垂直方向上的像素个数  
      * [15:0]   : 水平方向上的像素个数
	  */
	LCDIF->TRANSFER_COUNT  = (plcdparams->yres << 16) | (plcdparams->xres << 0);

	/*
	 * 设置ELCDIF的VDCTRL0寄存器
	 * [29] 0 : VSYNC输出  ，默认为0，无需设置
	 * [28] 1 : 在DOTCLK模式下，设置1硬件会产生使能ENABLE输出
	 * [27] 0 : VSYNC低电平有效  ,根据屏幕配置文件将其设置为0
	 * [26] 0 : HSYNC低电平有效 , 根据屏幕配置文件将其设置为0
	 * [25] 1 : DOTCLK下降沿有效 ，根据屏幕配置文件将其设置为1
	 * [24] 1 : ENABLE信号高电平有效，根据屏幕配置文件将其设置为1
	 * [21] 1 : 帧同步周期单位，DOTCLK mode设置为1
	 * [20] 1 : 帧同步脉冲宽度单位，DOTCLK mode设置为1
	 * [17:0] :  vysnc脉冲宽度 
	 */
		LCDIF->VDCTRL0 = (1 << 28)|( plcdparams->pins_pol.vsync << 27)\
						|( plcdparams->pins_pol.hsync << 26)\
						|( plcdparams->pins_pol.vclk << 25)\
						|(plcdparams->pins_pol.de << 24)\
						|(1 << 21)|(1 << 20)|( plcdparams->time_seq.tvp << 0);

	/*
	 * 设置ELCDIF的VDCTRL1寄存器
	 * 设置垂直方向的总周期:上黑框tvb+垂直同步脉冲tvp+垂直有效高度yres+下黑框tvf
	 */  	
	  LCDIF->VDCTRL1 = plcdparams->time_seq.tvb + plcdparams->time_seq.tvp + plcdparams->yres + plcdparams->time_seq.tvf;  

    /*
	 * 设置ELCDIF的VDCTRL2寄存器
	 * [18:31]  : 水平同步信号脉冲宽度
	 * [17: 0]   : 水平方向总周期
	 * 设置水平方向的总周期:左黑框thb+水平同步脉冲thp+水平有效高度xres+右黑框thf
	 */ 

	 LCDIF->VDCTRL2 = (plcdparams->time_seq.thp << 18) | (plcdparams->time_seq.thb + plcdparams->time_seq.thp + plcdparams->xres + plcdparams->time_seq.thf);

	 /*
	  * 设置ELCDIF的VDCTRL3寄存器
	  * [27:16] ：水平方向上的等待时钟数 =thb + thp
      * [15:0]  : 垂直方向上的等待时钟数 = tvb + tvp
      */ 
      
     LCDIF->VDCTRL3 = ((plcdparams->time_seq.thb + plcdparams->time_seq.thp) << 16) | (plcdparams->time_seq.tvb + plcdparams->time_seq.tvp);

	 /*
	  * 设置ELCDIF的VDCTRL4寄存器
	  * [18]     使用VSHYNC、HSYNC、DOTCLK模式此为置1
      * [17:0]  : 水平方向的宽度
      */ 

	 LCDIF->VDCTRL4 = (1<<18) | (plcdparams->xres);

	 /*
      * 设置ELCDIF的CUR_BUF和NEXT_BUF寄存器
      * CUR_BUF    :  当前显存地址
	  * NEXT_BUF :    下一帧显存地址
	  * 方便运算，都设置为同一个显存地址
	  */ 
	  
	LCDIF->CUR_BUF  =  plcdparams->fb_base;
    LCDIF->NEXT_BUF =  plcdparams->fb_base;
	
}
static void Imx6ull_lcd_controller_enable(void)
{	
	LCDIF->CTRL |= 1<<0; /* 使能6ULL的LCD控制器 */
}
static void Imx6ull_lcd_controller_disable(void)
{
	LCDIF->CTRL &= ~(1<<0); /* 关闭6ULL的LCD控制器 */
}

struct lcd_controller Imx6ull_lcd_controller = {
	.name         ="Imx6ull",
	.init         = Imx6ull_lcd_controller_init,
	.enable 	  = Imx6ull_lcd_controller_enable,
	.disable      = Imx6ull_lcd_controller_disable,
};

/**********************************************************************
 * 函数名称： Imx6ull_lcd_controller_add
 * 功能描述： 添加Imx6ull的lcd控制器
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/02/26	     V1.0	  zh(angenao)	      创建
 ***********************************************************************/
void Imx6ull_lcd_controller_add(void)
{
	register_lcd_controller(&Imx6ull_lcd_controller);
}
