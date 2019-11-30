#include <linux/module.h>

#include <linux/fs.h>
#include <linux/io.h>
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
#include "led_opr.h"

struct iomux {
	volatile unsigned int unnames[23];
	volatile unsigned int IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO00;  /* offset 0x5c*/
	volatile unsigned int IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO01;
	volatile unsigned int IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO02;
	volatile unsigned int IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO03;
	volatile unsigned int IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO04;
	volatile unsigned int IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO05;
	volatile unsigned int IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO06;
	volatile unsigned int IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO07;
	volatile unsigned int IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO08;
	volatile unsigned int IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO09;
};

struct imx6ull_gpio {
	volatile unsigned int dr;
	volatile unsigned int gdir;
	volatile unsigned int psr;
	volatile unsigned int icr1;
	volatile unsigned int icr2;
	volatile unsigned int imr;
	volatile unsigned int isr;
	volatile unsigned int edge_sel;
};


/* enable GPIO1,GPIO5 */
static volatile unsigned int *CCM_CCGR1; 

/* set GPIO5_IO03 as GPIO */
static volatile unsigned int *IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3;

static struct iomux *iomux;

static struct imx6ull_gpio *gpio1;
static struct imx6ull_gpio *gpio5;

static int board_demo_led_init (int which) /* 初始化LED, which-哪个LED */	   
{
	if (!CCM_CCGR1)
	{
		CCM_CCGR1 = ioremap(0x20C406C, 4);
		IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 = ioremap(0x2290014, 4);

		iomux = ioremap(0x20e0000, sizeof(struct iomux));
		gpio1 = ioremap(0x209C000, sizeof(struct imx6ull_gpio));
		gpio5 = ioremap(0x20AC000, sizeof(struct imx6ull_gpio));
	}

	if (which == 0)
	{
		/* 1. enable GPIO5 
		 * CG15, b[31:30] = 0b11
	     */
		*CCM_CCGR1 |= (3<<30);
		
		/* 2. set GPIO5_IO03 as GPIO 
		 * MUX_MODE, b[3:0] = 0b101
	     */
    	*IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 = 5;

		/* 3. set GPIO5_IO03 as output 
		 * GPIO5 GDIR, b[3] = 0b1
	     */
    	gpio5->gdir |= (1<<3);
	}
	else if(which == 1)
	{
		/* 1. enable GPIO1 
		 * CG13, b[27:26] = 0b11
	     */
		*CCM_CCGR1 |= (3<<26);
		
		/* 2. set GPIO1_IO03 as GPIO 
		 * MUX_MODE, b[3:0] = 0b101
	     */
    	iomux->IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO03 = 5;

		/* 3. set GPIO1_IO03 as output 
		 * GPIO1 GDIR, b[3] = 0b1
	     */
    	gpio1->gdir |= (1<<3);
	}
	else if(which == 2)
	{
		/* 1. enable GPIO1 
		 * CG13, b[27:26] = 0b11
	     */
		*CCM_CCGR1 |= (3<<26);
		
		/* 2. set GPIO1_IO05 as GPIO 
		 * MUX_MODE, b[3:0] = 0b101
	     */
    	iomux->IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO05 = 5;

		/* 3. set GPIO1_IO05 as output 
		 * GPIO1 GDIR, b[5] = 0b1
	     */
    	gpio1->gdir |= (1<<5);
	}
	else if(which == 3)
	{
		/* 1. enable GPIO1 
		 * CG13, b[27:26] = 0b11
	     */
		*CCM_CCGR1 |= (3<<26);
		
		/* 2. set GPIO1_IO06 as GPIO 
		 * MUX_MODE, b[3:0] = 0b101
	     */
    	iomux->IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO06 = 5;

		/* 3. set GPIO1_IO06 as output 
		 * GPIO1 GDIR, b[6] = 0b1
	     */
    	gpio1->gdir |= (1<<6);
	}
	
	//printk("%s %s line %d, led %d\n", __FILE__, __FUNCTION__, __LINE__, which);
	return 0;
}

static int board_demo_led_ctl (int which, char status) /* 控制LED, which-哪个LED, status:1-亮,0-灭 */
{
	//printk("%s %s line %d, led %d, %s\n", __FILE__, __FUNCTION__, __LINE__, which, status ? "on" : "off");
	if (which == 0)
	{
		if (status)  /* on : output 0 */
			gpio5->dr &= ~(1<<3);
		else         /* on : output 1 */
			gpio5->dr |= (1<<3);
	}
	else if (which == 1)
	{
		if (status)  /* on : output 0 */
			gpio1->dr &= ~(1<<3);
		else         /* on : output 1 */
			gpio1->dr |= (1<<3);
	}
	else if (which == 2)
	{
		if (status)  /* on : output 0 */
			gpio1->dr &= ~(1<<5);
		else         /* on : output 1 */
			gpio1->dr |= (1<<5);
	}
	else if (which == 3)
	{
		if (status)  /* on : output 0 */
			gpio1->dr &= ~(1<<6);
		else         /* on : output 1 */
			gpio1->dr |= (1<<6);
	}
	return 0;
}

static struct led_operations board_demo_led_opr = {
	.num  = 4,
	.init = board_demo_led_init,
	.ctl  = board_demo_led_ctl,
};

struct led_operations *get_board_led_opr(void)
{
	return &board_demo_led_opr;
}

