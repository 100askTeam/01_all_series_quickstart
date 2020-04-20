
#include "led.h"

static volatile unsigned int *CCM_CCGR1                              ;
static volatile unsigned int *IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3;
static volatile unsigned int *GPIO5_GDIR                             ;
static volatile unsigned int *GPIO5_DR                               ;

/**********************************************************************
 * 函数名称： led_init
 * 功能描述： 初始化LED引脚，就是把它设置为输出引脚
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/02/07	     V1.0	  韦东山	      创建
 ***********************************************************************/
void led_init(void)
{
	unsigned int val;
	
	CCM_CCGR1                               = (volatile unsigned int *)(0x20C406C);
	IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 = (volatile unsigned int *)(0x2290014);
	GPIO5_GDIR                              = (volatile unsigned int *)(0x020AC000 + 0x4);
	GPIO5_DR                                = (volatile unsigned int *)(0x020AC000);

	/* GPIO5_IO03 */
	/* a. 使能GPIO5
	 * set CCM to enable GPIO5
	 * CCM_CCGR1[CG15] 0x20C406C
	 * bit[31:30] = 0b11
	 */
	*CCM_CCGR1 |= (3<<30);
	
	/* b. 设置GPIO5_IO03用于GPIO
	 * set IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3
	 *      to configure GPIO5_IO03 as GPIO
	 * IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3  0x2290014
	 * bit[3:0] = 0b0101 alt5
	 */
	val = *IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3;
	val &= ~(0xf);
	val |= (5);
	*IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 = val;
	
	
	/* c. 设置GPIO5_IO03作为output引脚
	 * set GPIO5_GDIR to configure GPIO5_IO03 as output
	 * GPIO5_GDIR  0x020AC000 + 0x4
	 * bit[3] = 0b1
	 */
	*GPIO5_GDIR |= (1<<3);

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
		*GPIO5_DR &= ~(1<<3);
	}
	else  /* off: output 1*/
	{
		/* e. 设置GPIO5_IO3输出高电平
		 * set GPIO5_DR to configure GPIO5_IO03 output 1
		 * GPIO5_DR 0x020AC000 + 0
		 * bit[3] = 0b1
		 */ 
		*GPIO5_DR |= (1<<3);
	}
}




	
	
