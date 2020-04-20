#include "regs.h"
#include "pll.h"

extern struct ccm_regs *ccm;

/**********************************************************************
 * 函数名称： set_pll1_sw_clk
 * 功能描述： 设置PLL1_SW_CLK的时钟路径
 * 输入参数： sel_pll1: 0-选择XTALOSC24M的输出，1-选择PLL1的输出
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人       修改内容
 * -----------------------------------------------
 * 2020/03/08     V1.0      今朝        创建
 ***********************************************************************/
void sel_pll1_sw_clk(int sel_pll1)
{
    /* PLL1_SW_CLK_SEL: 0表示pll1_main_clk，1表示step_clk */
    if (sel_pll1)
        clr_bit(&ccm->ccsr, 2);     /* 选择pll1_main_clk */
    else {
        clr_bit(&ccm->ccsr, 8);     /* step_clk选择使用OSC的输出 */
        set_bit(&ccm->ccsr, 2);     /* 选择step_clk */
    }
}

/**********************************************************************
 * 函数名称： get_pll1_sw_clk
 * 功能描述： 获取PLL1_SW_CLK的时钟频率
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： PLL1_SW_CLK的时钟频率
 * 修改日期        版本号     修改人       修改内容
 * -----------------------------------------------
 * 2020/03/08     V1.0      今朝        创建
 ***********************************************************************/
u32 get_pll1_sw_clk(void)
{
    u32 reg = read32(&ccm->ccsr);
    
    if (reg & (1u<<2)) {        /* PLL1_SW_CLK_SEL：0表示pll1_main_clk，1表示step_clk */
        if (reg & (1u<<8)) {    /* STEP_SEL：1表示secondary_clk, 0表示OSC */
            if (reg & (1u<<3))  /* SECONDARY_CLK_SEL：1表示PLL2，0表示PLL2 PFD2 */
                return get_pll(SYS_PLL);
            else
                return get_pll_pfd(SYS_PLL, PFD2);
        } else
            return CKIH;        /* OSC输出 */
    } else
        return get_pll(ARM_PLL);
}

/**********************************************************************
 * 函数名称： get_pll3_sw_clk
 * 功能描述： 获取PLL3_SW_CLK的时钟频率
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： PLL3_SW_CLK的时钟频率
 * 修改日期        版本号     修改人       修改内容
 * -----------------------------------------------
 * 2020/03/08     V1.0      今朝        创建
 ***********************************************************************/
u32 get_pll3_sw_clk(void)
{
    u32 reg = read32(&ccm->ccsr);
    if (reg & 1)    /* PLL3_SW_CLK_SEL: 1表示pll3，0表示pll3_bypass（即OSC输出） */
        return get_pll(USB1_PLL);
    else
        return CKIH;            /* OSC输出 */
}

/**********************************************************************
 * 函数名称： get_pll4_main_clk
 * 功能描述： 获取PLL4_MAIN_CLK的时钟频率
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： PLL4_MAIN_CLK的时钟频率
 * 修改日期        版本号     修改人       修改内容
 * -----------------------------------------------
 * 2020/03/08     V1.0      今朝        创建
 ***********************************************************************/
extern struct anadig_regs *anadig;

u32 get_pll4_main_clk(void)
{
    u32 reg, audio_div;

    reg = read32(&anadig->pmu_misc2);
    /* AUDIO_DIV_MSB(23): AUDIO_DIV_LSB(15)
     * 00：除以1
     * 01：除以2
     * 10：除以1
     * 11：除以4
     */
    audio_div = reg & (1u<<15) ? (reg & (1u<<23) ? 4 : 2) : 1;

    return get_pll(AUDIO_PLL) / audio_div;
}

/**********************************************************************
 * 函数名称： get_pll5_main_clk
 * 功能描述： 获取PLL5_MAIN_CLK的时钟频率
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： PLL5_MAIN_CLK的时钟频率
 * 修改日期        版本号     修改人       修改内容
 * -----------------------------------------------
 * 2020/03/08     V1.0      今朝        创建
 ***********************************************************************/
u32 get_pll5_main_clk(void)
{
    u32 reg, video_div;

    reg = read32(&anadig->pmu_misc2);
    /* AUDIO_DIV_MSB(31): AUDIO_DIV_LSB(30)
     * 00：除以1
     * 01：除以2
     * 10：除以1
     * 11：除以4
     */
    video_div = reg & (1u<<30) ? (reg & (1u<<31) ? 4 : 2) : 1;

    return get_pll(VIDEO_PLL) / video_div;
}
