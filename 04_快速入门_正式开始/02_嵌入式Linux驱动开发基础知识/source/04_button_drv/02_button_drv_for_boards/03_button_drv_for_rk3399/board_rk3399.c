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

static volatile unsigned int *PMUCRU_CLKGATE_CON1;
static volatile unsigned int *GRF_GPIO0B_IOMUX ;
static volatile unsigned int *GPIO0_SWPORTA_DDR;
static volatile unsigned int *GPIO0_EXT_PORTA ;

static void board_rk3399_button_init (int which) /* 初始化button, which-哪个button */       
{
    if (which == 0)
    {
        if (!PMUCRU_CLKGATE_CON1)
        {
            PMUCRU_CLKGATE_CON1 = ioremap(0xFF760000+ 0x0104, 4);
            GRF_GPIO0B_IOMUX  = ioremap(0xFF310000+0x0004, 4);
            GPIO0_SWPORTA_DDR = ioremap(0xFF720000 + 0x0004, 4);
            GPIO0_EXT_PORTA   = ioremap(0xFF720000 + 0x0050, 4);
        }
        
        /* rk3399 GPIO0_B1 */
        /* a. 使能GPIO0
         * set CRU to enable GPIO0
         * PMUCRU_CLKGATE_CON1 0xFF760000+ 0x0104
         * (1<<(3+16)) | (0<<3)
         */
        *PMUCRU_CLKGATE_CON1 = (1<<(3+16)) | (0<<3);
        
        /* b. 设置GPIO0_B1用于GPIO
         * set PMU/GRF to configure GPIO0_B1 as GPIO
         * GRF_GPIO0B_IOMUX 0xFF310000+0x0004
         * bit[3:2] = 0b00
         * (3<<(2+16)) | (0<<2)
         */
        *GRF_GPIO0B_IOMUX =(3<<(2+16)) | (0<<2);
        
        /* c. 设置GPIO0_B1作为input引脚
         * set GPIO_SWPORTA_DDR to configure GPIO0_B1 as input
         * GPIO_SWPORTA_DDR 0xFF720000 + 0x0004
         * bit[9] = 0b0
         */
        *GPIO0_SWPORTA_DDR &= ~(1<<9);
    }
    
}

static int board_rk3399_button_read (int which) /* 读button, which-哪个 */
{
    //printk("%s %s line %d, button %d, 0x%x\n", __FILE__, __FUNCTION__, __LINE__, which, *GPIO1_DATAIN);
    if (which == 0)
        return (*GPIO0_EXT_PORTA & (1<<9)) ? 1 : 0;
    else
        return 0;
}
    
static struct button_operations my_buttons_ops = {
    .count = 1,
    .init = board_rk3399_button_init,
    .read = board_rk3399_button_read,
};

int board_rk3399_button_drv_init(void)
{
    register_button_operations(&my_buttons_ops);
    return 0;
}

void board_rk3399_button_drv_exit(void)
{
    unregister_button_operations();
}

module_init(board_rk3399_button_drv_init);
module_exit(board_rk3399_button_drv_exit);

MODULE_LICENSE("GPL");

