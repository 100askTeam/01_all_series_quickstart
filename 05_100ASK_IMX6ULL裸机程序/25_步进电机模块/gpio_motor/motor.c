#include "motor.h"


static int  S_CW[8]=  {0x2,0x3,0x1,0x9,0x8,0xc,0x4,0x6};
static int  Speed[10]={200,100,75,50,25,12,6,3,2,1};
static char phase = 0;
		  			 		  						  					  				 	   		  	  	 	  

static volatile unsigned int *CCM_CCGR3                             ;
static volatile unsigned int *IOMUXC_SW_MUX_CTL_PAD_CSI_VSYNC 		;
static volatile unsigned int *IOMUXC_SW_MUX_CTL_PAD_CSI_HSYNC 		;
static volatile unsigned int *IOMUXC_SW_MUX_CTL_PAD_CSI_DATA00		;
static volatile unsigned int *IOMUXC_SW_MUX_CTL_PAD_CSI_DATA01		;
		  			 		  						  					  				 	   		  	  	 	  

/**********************************************************************
 * 函数名称： gpio_init
 * 功能描述： 初始化GPIO4   _19 _20 _21 _22 
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/02/07	     V1.0	  韦东山	      创建
 ***********************************************************************/
void gpio_init(void)
{

/* a. 使能GPIO4
 * set CCM to enable GPIO4
 * CCM_CCGR1[CG15] 0x20C4074
 * bit[13:12] = 0b11
 */
	*CCM_CCGR3 |= (3<<12);

//设置 IOMUX 来选择引脚用于 GPIO4 4_19,4_20,4_21,4_22。
	*IOMUXC_SW_MUX_CTL_PAD_CSI_VSYNC 		= 5;
	*IOMUXC_SW_MUX_CTL_PAD_CSI_HSYNC 		= 5;
	*IOMUXC_SW_MUX_CTL_PAD_CSI_DATA00		= 5;
	*IOMUXC_SW_MUX_CTL_PAD_CSI_DATA01		= 5;


//设置 GPIO4_GDIR 中  4_19,4_20,4_21,4_22 引脚设置为输出功能。
	GPIO4->GDIR |= (15 << 19);

}
		  			 		  						  					  				 	   		  	  	 	  

void volatile delay1ms(void)
{
   volatile   int j;
   for(j=0;j<1270;j++)
           ;
}

void delaynms(unsigned int aa)
{
	while(aa--)
	delay1ms();	
}


/**********************************************************************
 * 函数名称： motor_sw
 * 功能描述： 电机顺时针转
 * 输入参数： 
 *     speed :转动速度     1-10 
       degree:转动步数
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 ***********************************************************************/
void motor_sw(int speed ,int degree)						
{
   if((speed  > 10)||(speed  < 1))
   	   speed  = 5;
   int  i,num;
   for(i=0;i<degree;i++)
   	{
  	phase ++;		 
	num  = S_CW[phase&7];
	num  = num << 19;
	GPIO4->DR = num;
	delaynms(Speed[speed-1]); 
   	}

}
		  			 		  						  					  				 	   		  	  	 	  
/**********************************************************************
 * 函数名称： motor_sw
 * 功能描述： 电机逆时针转
 * 输入参数： 
 *     speed :转动速度     1-10 
       degree:转动步数
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
  ***********************************************************************/
void motor_aw(int speed ,int degree)						
{
   if ((speed  > 10)||(speed  < 1))
   	   speed  = 5;

   int  i,num;
   for(i=0;i<degree;i++)
   	{
	phase --;	 
	num  = S_CW[phase&7];
	num  = num << 19;
	GPIO4->DR = num;
    delaynms(Speed[speed-1]);
   	}
}

/**********************************************************************
 * 函数名称： led_ctl
 * 功能描述： 设置LED状态
 * 输入参数： 
 *     on : 1-LED点亮, 0-LED熄灭
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/02/07	     V1.0	  韦东山	      创建
 ***********************************************************************/
void led_ctl(int on)
{
	if (on) /* on: output 0*/
	{
		/* d. 设置GPIO5_DR输出低电平
		 * set GPIO5_DR to configure GPIO5_IO03 output 0
		 * GPIO5_DR 0x020AC000 + 0
		 * bit[3] = 0b0
		 */
		GPIO5->DR &= ~(1<<3);
	}
	else  /* off: output 1*/
	{
		/* e. 设置GPIO5_IO3输出高电平
		 * set GPIO5_DR to configure GPIO5_IO03 output 1
		 * GPIO5_DR 0x020AC000 + 0
		 * bit[3] = 0b1
		 */ 
		GPIO5->DR |= (1<<3);
	}
}



