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

#include "button_drv.h"

static volatile unsigned int *CM_PER_GPIO1_CLKCTRL;
static volatile unsigned int *conf_gpmc_a9;
static volatile unsigned int *GPIO1_OE;
static volatile unsigned int *GPIO1_DATAIN;

static void board_am335x_button_init (int which) /* 初始化button, which-哪个button */       
{
    if (which == 0)
    {
        if (!CM_PER_GPIO1_CLKCTRL)
        {
            CM_PER_GPIO1_CLKCTRL = ioremap(0x44E00000 + 0xAC, 4);
            conf_gpmc_a9 = ioremap(0x44E10000 + 0x864, 4);
            GPIO1_OE = ioremap(0x4804C000 + 0x134, 4);
            GPIO1_DATAIN = ioremap(0x4804C000 + 0x138, 4);
        }
        
        //printk("%s %s line %d, led %d\n", __FILE__, __FUNCTION__, __LINE__, which);
        /* a. 使能GPIO1
         * set PRCM to enalbe GPIO1 
         * set CM_PER_GPIO1_CLKCTRL (0x44E00000 + 0xAC)
         * val: (1<<18) | 0x2
         */
        *CM_PER_GPIO1_CLKCTRL = (1<<18) | 0x2;
        
        /* b. 设置GPIO1_25的功能，让它工作于GPIO模式
         * set Control Module to set GPIO1_25 (U16) used as GPIO 
         * conf_gpmc_a9 as mode 7
         * addr : 0x44E10000 + 0x864
         * bit[5]   : 1, Input enable value for the PAD
         * bit[2:0] : mode 7
         */
        *conf_gpmc_a9 = (1<<5) | 7;
        
        /* c. 设置GPIO1_25的方向，让它作为输入引脚
         * set GPIO1's registers , to set 设置GPIO1_25的方向'S dir (input) 
         * GPIO_OE 
         * addr : 0x4804C000 + 0x134
         * set bit 25
         */

        *GPIO1_OE |= (1<<25);
    }
    
}

static int board_am335x_button_read (int which) /* 读button, which-哪个 */
{
    printk("%s %s line %d, button %d, 0x%x\n", __FILE__, __FUNCTION__, __LINE__, which, *GPIO1_DATAIN);
    if (which == 0)
        return (*GPIO1_DATAIN & (1<<25)) ? 1 : 0;
    else
        return 0;
}
    
static struct button_operations my_buttons_ops = {
    .count = 1,
    .init = board_am335x_button_init,
    .read = board_am335x_button_read,
};

int board_am335x_button_drv_init(void)
{
    register_button_operations(&my_buttons_ops);
    return 0;
}

void board_am335x_button_drv_exit(void)
{
    unregister_button_operations();
}

module_init(board_am335x_button_drv_init);
module_exit(board_am335x_button_drv_exit);

MODULE_LICENSE("GPL");

