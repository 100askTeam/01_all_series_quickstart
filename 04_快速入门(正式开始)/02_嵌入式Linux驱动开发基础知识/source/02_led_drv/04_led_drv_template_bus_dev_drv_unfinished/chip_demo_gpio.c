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
#include "led_opr.h"
#include "led_resource.h"


static int g_ledpins[100];
static int g_ledcnt = 0;

static int board_demo_led_init (int which) /* 初始化LED, which-哪个LED */	   
{	
	//printk("%s %s line %d, led %d\n", __FILE__, __FUNCTION__, __LINE__, which);
	
	printk("init gpio: group %d, pin %d\n", GROUP(g_ledpins[which]), PIN(g_ledpins[which]));
	switch(GROUP(g_ledpins[which]))
	{
		case 0:
		{
			printk("init pin of group 0 ...\n");
			break;
		}
		case 1:
		{
			printk("init pin of group 1 ...\n");
			break;
		}
		case 2:
		{
			printk("init pin of group 2 ...\n");
			break;
		}
		case 3:
		{
			printk("init pin of group 3 ...\n");
			break;
		}
	}
	
	return 0;
}

static int board_demo_led_ctl (int which, char status) /* 控制LED, which-哪个LED, status:1-亮,0-灭 */
{
	//printk("%s %s line %d, led %d, %s\n", __FILE__, __FUNCTION__, __LINE__, which, status ? "on" : "off");
	printk("set led %s: group %d, pin %d\n", status ? "on" : "off", GROUP(g_ledpins[which]), PIN(g_ledpins[which]));

	switch(GROUP(g_ledpins[which]))
	{
		case 0:
		{
			printk("set pin of group 0 ...\n");
			break;
		}
		case 1:
		{
			printk("set pin of group 1 ...\n");
			break;
		}
		case 2:
		{
			printk("set pin of group 2 ...\n");
			break;
		}
		case 3:
		{
			printk("set pin of group 3 ...\n");
			break;
		}
	}

	return 0;
}

static struct led_operations board_demo_led_opr = {
	.num  = 1,
	.init = board_demo_led_init,
	.ctl  = board_demo_led_ctl,
};


static int chip_demo_gpio_led_probe(struct platform_device *dev)
{
	int i = 0;
	struct resource *res;
		
	while (1)
	{
		res = platform_get_resource(dev, IORESOURCE_IRQ, i++);
		if (!res)
			break;

		/* 记录引脚 */
		g_ledpins[g_ledcnt] = res->start;
		
		/* device_create */
		led_device_create(g_ledcnt);
		g_ledcnt++;
	}


	return 0;
}

static int chip_demo_gpio_led_remove(struct platform_device *dev)
{
	int i;
	/* device_destroy */
	for (i = 0; i < g_ledcnt; i++)
	{
		led_device_destroy(i);
	}
	g_ledcnt = 0;

	return 0;
}

static struct platform_driver chip_demo_gpio_drv = {
	.probe		= chip_demo_gpio_led_probe,
	.remove		= chip_demo_gpio_led_remove,
	.driver		= {
		.name	= "100ask_led",
	},
};


static int chip_demo_gpio_drv_init(void)
{
	int err;
	
	err = platform_driver_register(&chip_demo_gpio_drv);
	register_led_operations(&board_demo_led_opr);
	
	return 0;
}

static void chip_demo_gpio_drv_exit(void)
{
	platform_driver_unregister(&chip_demo_gpio_drv);
}

module_init(chip_demo_gpio_drv_init);
module_exit(chip_demo_gpio_drv_exit);
MODULE_LICENSE("GPL");


