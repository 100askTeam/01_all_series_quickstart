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

/* 从CLKO1和CLKO2引脚输出时钟信号进行测试 */
extern void setup_clock_output(void);
#include "uart.h"
#include "my_printf.h"

/**********************************************************************
 * 函数名称： show_clocks
 * 功能描述： 打印PLL输出时钟频率和总线根时钟频率
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人       修改内容
 * -----------------------------------------------
 * 2020/03/08     V1.0      今朝        创建
 ***********************************************************************/
void show_clocks(void)
{
    u32 freq;

    printf("Show IMX6ULL Clocks: \r\n");
    freq = get_pll(ARM_PLL);
    printf("ARM_PLL     %8d MHz\r\n", freq / 1000000);

    freq = get_pll(SYS_PLL);
    printf("SYS_PLL     %8d MHz\r\n", freq / 1000000);
    freq = get_pll_pfd(SYS_PLL, PFD0);
    printf("|-SYS_PLL_PFD0  %8d MHz\r\n", freq / 1000000);
    freq = get_pll_pfd(SYS_PLL, PFD1);
    printf("|-SYS_PLL_PFD1  %8d MHz\r\n", freq / 1000000);
    freq = get_pll_pfd(SYS_PLL, PFD2);
    printf("|-SYS_PLL_PFD2  %8d MHz\r\n", freq / 1000000);
    freq = get_pll_pfd(SYS_PLL, PFD3);
    printf("|-SYS_PLL_PFD3  %8d MHz\r\n", freq / 1000000);

    freq = get_pll(USB1_PLL);
    printf("USB1_PLL    %8d MHz\r\n", freq / 1000000);
    freq = get_pll_pfd(USB1_PLL, PFD0);
    printf("|-USB1_PLL_PFD0 %8d MHz\r\n", freq / 1000000);
    freq = get_pll_pfd(USB1_PLL, PFD1);
    printf("|-USB1_PLL_PFD1 %8d MHz\r\n", freq / 1000000);
    freq = get_pll_pfd(USB1_PLL, PFD2);
    printf("|-USB1_PLL_PFD2 %8d MHz\r\n", freq / 1000000);
    freq = get_pll_pfd(USB1_PLL, PFD3);
    printf("|-USB1_PLL_PFD3 %8d MHz\r\n", freq / 1000000);

    freq = get_pll(USB2_PLL);
    printf("USB2_PLL    %8d MHz\r\n", freq / 1000000);
    freq = get_pll(AUDIO_PLL);
    printf("AUDIO_PLL   %8d MHz\r\n", freq / 1000000);
    freq = get_pll(VIDEO_PLL);
    printf("VIDEO_PLL   %8d MHz\r\n", freq / 1000000);

    printf("\r\n");
    freq = get_arm_clk_root();
    printf("ARM_CLK_ROOT    %8d KHZ\r\n", freq / 1000);
    freq = get_ahb_clk_root();
    printf("AHB_CLK_ROOT    %8d KHZ\r\n", freq / 1000);
    freq = get_ipg_clk_root();
    printf("IPG_CLK_ROOT    %8d KHZ\r\n", freq / 1000);
    freq = get_axi_clk_root();
    printf("AXI_CLK_ROOT    %8d KHZ\r\n", freq / 1000);
    freq = get_fabric_mmdc_clk_root();
    printf("FABRIC_MMDC_CLK_ROOT    %8d KHZ\r\n", freq / 1000);
    printf("\r\n");
}

void main(void)
{
    /* 首先初始化并点亮LED灯 */
    int blinks = 0;
    led_init();
    led_on();
    Uart_Init();
    /* 默认不开启输出时钟信号，有条件的同学可以尝试输出并测量 */
    //setup_clock_output();

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

    /* 打印时钟信号 */
    show_clocks();

    /* 无限循环点亮/熄灭LED，观察此时LED闪烁频率明显变快 */
    while(1)
    {
        busy_wait();
        led_toggle();
    }
}
