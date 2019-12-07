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

static volatile unsigned int *CRU_CLKGATE_CON31;
static volatile unsigned int *GRF_GPIO2D_IOMUX ;
static volatile unsigned int *GPIO2_SWPORTA_DDR;
static volatile unsigned int *GPIO2_SWPORTA_DR ;

static int board_demo_led_init (int which) /* 初始化LED, which-哪个LED */       
{
    //printk("%s %s line %d, led %d\n", __FILE__, __FUNCTION__, __LINE__, which);
    if (which == 0)
    {
        if (!CRU_CLKGATE_CON31)
        {
            CRU_CLKGATE_CON31 = ioremap(0xFF760000 + 0x037c, 4);
            GRF_GPIO2D_IOMUX  = ioremap(0xFF770000 + 0x0e00c, 4);
            GPIO2_SWPORTA_DDR = ioremap(0xFF780000 + 0x0004, 4);
            GPIO2_SWPORTA_DR  = ioremap(0xFF780000 + 0x0000, 4);
        }
            
        /* rk3399 GPIO2_D3 */
        /* a. 使能GPIO2
         * set CRU to enable GPIO2
         * CRU_CLKGATE_CON31 0xFF760000 + 0x037c
         * (1<<(3+16)) | (0<<3)
         */
        *CRU_CLKGATE_CON31 = (1<<(3+16)) | (0<<3);

        /* b. 设置GPIO2_D3用于GPIO
         * set PMU/GRF to configure GPIO2_D3 as GPIO
         * GRF_GPIO2D_IOMUX 0xFF770000 + 0x0e00c
         * bit[7:6] = 0b00
         * (3<<(6+16)) | (0<<6)
         */
        *GRF_GPIO2D_IOMUX = (3<<(6+16)) | (0<<6);

        /* c. 设置GPIO2_D3作为output引脚
         * set GPIO_SWPORTA_DDR to configure GPIO2_D3 as output
         * GPIO_SWPORTA_DDR 0xFF780000 + 0x0004
         * bit[27] = 0b1
         */ 
        *GPIO2_SWPORTA_DDR |= (1<<27);
    }
    return 0;
}

static int board_demo_led_ctl (int which, char status) /* 控制LED, which-哪个LED, status:1-亮, 0-灭*/
{
    //printk("%s %s line %d, led %d, %s\n", __FILE__, __FUNCTION__, __LINE__, which, status ? "on" : "off");
    if (which == 0)
    {
        if (status) /* on: output 1 */
        {   
            /* d. 设置GPIO2_D3输出高电平
             * set GPIO_SWPORTA_DR to configure GPIO2_D3 output 1
             * GPIO_SWPORTA_DR 0xFF780000 + 0x0000
             * bit[27] = 0b1
             */ 
            *GPIO2_SWPORTA_DR |= (1<<27);
        }
        else /* off : output 0 */
        {
            /* e. 设置GPIO2_D3输出低电平
             * set GPIO_SWPORTA_DR to configure GPIO2_D3 output 0
             * GPIO_SWPORTA_DR 0xFF780000 + 0x0000
             * bit[27] = 0b0
             */
            *GPIO2_SWPORTA_DR &= ~(1<<27);
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

