#include <linux/module.h>

#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/tty.h>
#include <linux/kmod.h>
#include <linux/gfp.h>
#include <asm/io.h>

#include "led_opr.h"

static volatile unsigned int *CCM_CCGR1                       ;
static volatile unsigned int *IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO03;
static volatile unsigned int *GPIO1_GDIR                      ;
static volatile unsigned int *GPIO1_DR                        ;

static int board_demo_led_init (int which) /* 初始化LED, which-哪个LED */	   
{
	unsigned int val;

	//printk("%s %s line %d, led %d\n", __FILE__, __FUNCTION__, __LINE__, which);
	if (which == 0)
	{
		if (!CCM_CCGR1)
		{
			CCM_CCGR1								= ioremap(0x20C406C, 4);
			IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO03        = ioremap(0x20E0068, 4);
			GPIO1_GDIR								= ioremap(0x0209C000 + 0x4, 4);
			GPIO1_DR								= ioremap(0x0209C000 + 0, 4);
		}
		
		/* GPIO1_IO03 */
		/* a. 使能GPIO1
		 * set CCM to enable GPIO1
		 * CCM_CCGR1[CG13] 0x20C406C
		 * bit[27:26] = 0b11
		 */
		*CCM_CCGR1 |= (3<<26);
		
		/* b. 设置GPIO1_IO03用于GPIO
		 * set IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO03
		 *		to configure GPIO1_IO03 as GPIO
		 * IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO03  0x20E0068
		 * bit[3:0] = 0b0101 alt5
		 */
		val = *IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO03;
		val &= ~(0xf);
		val |= (5);
		*IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO03 = val;
		
		
		/* c. 设置GPIO1_IO03作为output引脚
		 * set GPIO1_GDIR to configure GPIO1_IO03 as output
		 * GPIO1_GDIR  0x0209C000 + 0x4
		 * bit[3] = 0b1
		 */
		*GPIO1_GDIR |= (1<<3);
	}
	
	return 0;
}

static int board_demo_led_ctl (int which, char status) /* 控制LED, which-哪个LED, status:1-亮,0-灭 */
{
	//printk("%s %s line %d, led %d, %s\n", __FILE__, __FUNCTION__, __LINE__, which, status ? "on" : "off");
	if (which == 0)
	{
		if (status) /* on: output 0*/
		{
			/* d. 设置GPIO1_DR输出低电平
			 * set GPIO1_DR to configure GPIO1_IO03 output 0
			 * GPIO1_DR 0x0209C000 + 0
			 * bit[3] = 0b0
			 */
			*GPIO1_DR &= ~(1<<3);
		}
		else  /* off: output 1*/
		{
			/* e. 设置GPIO1_IO03输出高电平
			 * set GPIO1_DR to configure GPIO1_IO03 output 1
			 * GPIO1_DR 0x0209C000 + 0
			 * bit[3] = 0b1
			 */ 
			*GPIO1_DR |= (1<<3);
		}
	
	}
	return 0;
}

static struct led_operations board_demo_led_opr = {
	.num  = 1,
	.init = board_demo_led_init,
	.ctl  = board_demo_led_ctl,
};

struct led_operations *get_board_led_opr(void)
{
	return &board_demo_led_opr;
}

