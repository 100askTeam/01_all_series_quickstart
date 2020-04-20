#include "regs.h"
#include "pll.h"
#include "clkroot.h"

struct ccm_regs *ccm = (struct ccm_regs *)CCM_BASE_ADDR;

#define LOOPS 1000000
static void busy_wait(void)
{
    for(u32 i = 0; i < LOOPS; i++); /* 忙等待进行延时 */
}

/* LED灯接口函数 */
extern void led_init(void);
extern void led_toggle(void);
void led_on(void);

/* PLL1时钟路径及其PODF分频参数设置 */
extern void setup_arm_podf(u32 podf);
extern void sel_pll1_sw_clk(int sel_pll1);

void main(void)
{
    /* 首先初始化并点亮LED灯 */
    int blinks = 0;
    led_init();
    led_on();

    sel_pll1_sw_clk(0);     /* 将ARM_ROOT时钟切换至OSC */
    setup_arm_podf(8);      /* ARM_ROOT的分频参数设置为8 */
    set_pll(ARM_PLL, 54);   /* 设置ARM_PLL: 24*54/2 = 648MHZ, ARM_ROOT: 81MHZ */
    sel_pll1_sw_clk(1);     /* 将ARM_ROOT切换回ARM_PLL，此时CPU工作频率为81MHZ */

    /* 循环点亮/熄灭LED共10次，观察LED闪烁频率 */
    for (blinks = 10; blinks > 0; blinks--)
    {
        busy_wait();
        led_toggle();
    }

    sel_pll1_sw_clk(0);     /* 将ARM_ROOT时钟切换至OSC */
    setup_arm_podf(2);      /* ARM_ROOT的分频参数设置为2 */
    set_pll(ARM_PLL, 108);  /* 设置ARM_PLL: 24*108/2 = 1296MHZ, ARM_ROOT: 648MHZ */
    sel_pll1_sw_clk(1);     /* 将ARM_ROOT切换回ARM_PLL，此时CPU工作频率为648MHZ */

    /* 无限循环点亮/熄灭LED，观察此时LED闪烁频率明显变快 */
    while(1)
    {
        busy_wait();
        led_toggle();
    }
}

/* 本程序的除法运算使用了GCC提供的函数，需要提供raise函数以正常编译 */
void raise(void)
{
}
