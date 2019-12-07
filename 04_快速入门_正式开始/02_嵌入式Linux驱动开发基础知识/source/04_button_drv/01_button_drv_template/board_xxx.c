#include <linux/module.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/fcntl.h>
#include <linux/fs.h>
#include <linux/signal.h>
#include <linux/mutex.h>
#include <linux/mm.h>
#include <linux/timer.h>
#include <linux/wait.h>
#include <linux/skbuff.h>
#include <linux/proc_fs.h>
#include <linux/poll.h>
#include <linux/capi.h>
#include <linux/kernelcapi.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/moduleparam.h>

#include "button_drv.h"


static void board_xxx_button_init_gpio (int which)
{
    printk("%s %s %d, init gpio for button %d\n", __FILE__, __FUNCTION__, __LINE__, which);
}

static int board_xxx_button_read_gpio (int which)
{
    printk("%s %s %d, read gpio for button %d\n", __FILE__, __FUNCTION__, __LINE__, which);
    return 1;
}

static struct button_operations my_buttons_ops ={
    .count = 2,
    .init  = board_xxx_button_init_gpio,
    .read  = board_xxx_button_read_gpio,
};

int board_xxx_button_init(void)
{
    register_button_operations(&my_buttons_ops);
    return 0;
}

void board_xxx_button_exit(void)
{
    unregister_button_operations();
}

module_init(board_xxx_button_init);
module_exit(board_xxx_button_exit);

MODULE_LICENSE("GPL");


