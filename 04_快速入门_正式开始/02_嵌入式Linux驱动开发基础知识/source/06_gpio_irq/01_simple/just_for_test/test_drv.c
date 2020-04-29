#include <linux/module.h>

#include <linux/init.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/pm.h>
#include <linux/slab.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/gpio_keys.h>
#include <linux/workqueue.h>
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
#include <linux/spinlock.h>

struct gpio_key {
	int gpio;
	int irq;
	enum of_gpio_flags flag;
};

static struct gpio_key *gpio_keys;


static const struct of_device_id ask100_gpios[] = {
    { .compatible = "100ask,gpio_key" },
    { },
};

static irqreturn_t gpio_key_irq_100ask(int irq, void *dev_id)
{
	struct gpio_key *gpio_key = dev_id;

	printk("key %d val %d\n", irq, gpio_get_value(gpio_key->gpio));

	return IRQ_HANDLED;
}


static int chip_demo_gpio_probe(struct platform_device *pdev)
{
	struct device_node *node = pdev->dev.of_node;
	int count;
	int i;
	enum of_gpio_flags flags;
	int gpio;
	int irq;
	int err;
	
	count = of_gpio_count(node);

	gpio_keys = kzalloc(count * sizeof(struct gpio_key), GFP_KERNEL);

	for (i = 0; i < count; i++)
	{
		gpio = of_get_gpio_flags(node, i, &flags);
		irq = gpio_to_irq(gpio);	

		gpio_keys[i].gpio = gpio;
		gpio_keys[i].irq  = irq;
		gpio_keys[i].flag = flags;
		
		err = request_irq(irq, gpio_key_irq_100ask, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "100ask_gpio_key", &gpio_keys[i]);
	}

	return 0;
}

static int chip_demo_gpio_remove(struct platform_device *pdev)
{
	struct device_node *node = pdev->dev.of_node;
	int count;
	int i;

	count = of_gpio_count(node);
	for (i = 0; i < count; i++)
	{
		free_irq(gpio_keys[i].irq, &gpio_keys[i]);
	}
	
    return 0;
}


/* 1. 定义platform_driver */
static struct platform_driver test_gpio_drv = {
    .probe      = chip_demo_gpio_probe,
    .remove     = chip_demo_gpio_remove,
    .driver     = {
        .name   = "100ask_led",
        .of_match_table = ask100_gpios,
    },
};


static int test_gpio_drv_init(void)
{
	platform_driver_register(&test_gpio_drv);
	return 0;
}

static void test_gpio_drv_exit(void)
{
	platform_driver_unregister(&test_gpio_drv);
}

module_init(test_gpio_drv_init);
module_exit(test_gpio_drv_exit);

MODULE_LICENSE("GPL");


