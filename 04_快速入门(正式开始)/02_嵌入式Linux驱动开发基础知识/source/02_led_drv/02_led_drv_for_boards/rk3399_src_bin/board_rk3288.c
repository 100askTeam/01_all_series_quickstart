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

static volatile unsigned int *CRU_CLKGATE14_CON;
static volatile unsigned int *GRF_GPIO8A_IOMUX ;
static volatile unsigned int *GPIO8_SWPORTA_DDR;
static volatile unsigned int *GPIO8_SWPORTA_DR ;

static int board_demo_led_init (int which) /* 初始化LED, which-哪个LED */	   
{
	//printk("%s %s line %d, led %d\n", __FILE__, __FUNCTION__, __LINE__, which);
	if (which == 0)
	{
		if (!CRU_CLKGATE14_CON)
		{
			CRU_CLKGATE14_CON = ioremap(0xFF760000 + 0x0198, 4);
			GRF_GPIO8A_IOMUX  = ioremap(0xFF770000 + 0x0080, 4);
			GPIO8_SWPORTA_DDR = ioremap(0xFF7F0000 + 0x0004, 4);
			GPIO8_SWPORTA_DR  = ioremap(0xFF7F0000 + 0x0000, 4);
		}
		
		/* rk3288 GPIO8_A1 */
		/* a. 使能GPIO8
		 * set CRU to enable GPIO8
		 * CRU_CLKGATE14_CON 0xFF760000 + 0x198
		 * (1<<(8+16)) | (0<<8)
		 */
		*CRU_CLKGATE14_CON = (1<<(8+16)) | (0<<8);

		/* b. 设置GPIO8_A1用于GPIO
		 * set PMU/GRF to configure GPIO8_A1 as GPIO
		 * GRF_GPIO8A_IOMUX 0xFF770000 + 0x0080
		 * bit[3:2] = 0b00
		 * (3<<(2+16)) | (0<<2)
		 */
		*GRF_GPIO8A_IOMUX =(3<<(2+16)) | (0<<2);

		/* c. 设置GPIO8_A1作为output引脚
		 * set GPIO_SWPORTA_DDR to configure GPIO8_A1 as output
		 * GPIO_SWPORTA_DDR 0xFF7F0000 + 0x0004
		 * bit[1] = 0b1
		 */
		*GPIO8_SWPORTA_DDR |= (1<<1);
	}
		return 0;
}

static int board_demo_led_ctl (int which, char status) /* 控制LED, which-哪个LED, status:1-亮, 0-灭*/
{
	//printk("%s %s line %d, led %d, %s\n", __FILE__, __FUNCTION__, __LINE__, which, status ? "on" : "off");
	if (which == 0)
	{
		if (status) /* on: output 0 */
		{
			/* e. 设置GPIO8_A1输出低电平
			 * set GPIO_SWPORTA_DR to configure GPIO8_A1 output 0
			 * GPIO_SWPORTA_DR 0xFF7F0000 + 0x0000
			 * bit[1] = 0b0
			 */ 
			*GPIO8_SWPORTA_DR &= ~(1<<1);
		}
		else /* off: output 1 */
		{ 
			/* d. 设置GPIO8_A1输出高电平
			 * set GPIO_SWPORTA_DR to configure GPIO8_A1 output 1
			 * GPIO_SWPORTA_DR 0xFF7F0000 + 0x0000
			 * bit[1] = 0b1
			 */ 
			*GPIO8_SWPORTA_DR |= (1<<1);
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

