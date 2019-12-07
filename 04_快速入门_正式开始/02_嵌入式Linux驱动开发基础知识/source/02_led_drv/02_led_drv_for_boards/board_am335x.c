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

static volatile unsigned int *CM_PER_GPIO1_CLKCTRL;
static volatile unsigned int *conf_gpmc_a0;
static volatile unsigned int *GPIO1_OE;
static volatile unsigned int *GPIO1_CLEARDATAOUT;
static volatile unsigned int *GPIO1_SETDATAOUT;

static int board_demo_led_init (int which) /* 初始化LED, which-哪个LED */       
{
    if (which == 0)
    {
        if (!CM_PER_GPIO1_CLKCTRL)
        {
            CM_PER_GPIO1_CLKCTRL = ioremap(0x44E00000 + 0xAC, 4);
            conf_gpmc_a0 = ioremap(0x44E10000 + 0x840, 4);
            GPIO1_OE = ioremap(0x4804C000 + 0x134, 4);
            GPIO1_CLEARDATAOUT = ioremap(0x4804C000 + 0x190, 4);
            GPIO1_SETDATAOUT = ioremap(0x4804C000 + 0x194, 4);
        }
        
        //printk("%s %s line %d, led %d\n", __FILE__, __FUNCTION__, __LINE__, which);
        /* a. 使能GPIO1
         * set PRCM to enalbe GPIO1 
         * set CM_PER_GPIO1_CLKCTRL (0x44E00000 + 0xAC)
         * val: (1<<18) | 0x2
         */
        *CM_PER_GPIO1_CLKCTRL = (1<<18) | 0x2;
        
        /* b. 设置GPIO1_16的功能，让它工作于GPIO模式
         * set Control Module to set GPIO1_16 (R13) used as GPIO 
         * conf_gpmc_ad0 as mode 7
         * addr : 0x44E10000 + 0x800
         * val  : 7
         */
        *conf_gpmc_a0 = 7;
        
        /* c. 设置GPIO1_16的方向，让它作为输出引脚
         * set GPIO1's registers , to set GPIO1_16'S dir (output) 
         * GPIO_OE 
         * addr : 0x4804C000 + 0x134
         * clear bit 16
         */

        *GPIO1_OE &= ~(1<<16);
    }
    
    return 0;
}

static int board_demo_led_ctl (int which, char status) /* 控制LED, which-哪个LED, status:1-亮,0-灭 */
{
    //printk("%s %s line %d, led %d, %s\n", __FILE__, __FUNCTION__, __LINE__, which, status ? "on" : "off");

    if (which == 0)
    {
        if (status) /* on: output 0 */
        {
            /* e. 清除GPIO1_16的数据，让它输出低电平
             * AM335X芯片支持set-and-clear protocol，设置GPIO_CLEARDATAOUT的bit 16为1即可让引脚输出0：
             * set GPIO1_16's registers , to output 0 
             * GPIO_CLEARDATAOUT
             * addr : 0x4804C000 + 0x190
             */
            *GPIO1_CLEARDATAOUT = (1<<16); 
        }
        else
        {
            /* d. 设置GPIO1_16的数据，让它输出高电平
             * AM335X芯片支持set-and-clear protocol，设置GPIO_SETDATAOUT的bit 16为1即可让引脚输出1：
             * set GPIO1_16's registers , to output 1 
             * GPIO_SETDATAOUT
             * addr : 0x4804C000 + 0x194
             */
            *GPIO1_SETDATAOUT = (1<<16); 
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

