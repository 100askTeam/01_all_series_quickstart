#include "common.h"
#include "pll.h"
#include "regs.h"
#include "clkroot.h"

extern struct ccm_regs *ccm;

/**********************************************************************
 * 函数名称： setup_arm_podf
 * 功能描述： 设置ARM_CLK_ROOT的分频参数
 * 输入参数： 取值范围为1-8，ARM_CLK_ROOT = PLL1_SW_CLK / PODF
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人       修改内容
 * -----------------------------------------------
 * 2020/03/08     V1.0      今朝        创建
 ***********************************************************************/
void setup_arm_podf(u32 podf)
{
    if (podf < 1 || podf > 8) return;   /* ARM_PODF分频范围是1到8 */
    write32(podf-1, &ccm->cacrr);
}

/**********************************************************************
 * 函数名称： get_arm_clk_root
 * 功能描述： 获取ARM_CLK_ROOT的频率
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： ARM_CLK_ROOT的频率
 * 修改日期        版本号     修改人       修改内容
 * -----------------------------------------------
 * 2020/03/08     V1.0      今朝        创建
 ***********************************************************************/
u32 get_arm_clk_root(void)
{
    u32 reg, freq;

    reg = read32(&ccm->cacrr);
    reg = (reg & 0x7) + 1;              /* 获取ARM_PODF的分频范围 */
    freq = get_pll(ARM_PLL);

    return freq / reg;
}

/**********************************************************************
 * 函数名称： get_periph_clk
 * 功能描述： 获取PERIPH_CLK的频率
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： PERIPH_CLK的频率
 * 修改日期        版本号     修改人       修改内容
 * -----------------------------------------------
 * 2020/03/08     V1.0      今朝        创建
 ***********************************************************************/
static u32 get_periph_clk(void)
{
    u32 reg, per_clk2_podf = 0, freq = 0;

    reg = read32(&ccm->cbcdr);

    /* PERIPH_CLK_SEL选择periph_clk的时钟源：1表示periph_clk2, 0表示pre_periph_clk */
    if (reg & (1u << 25)) {         /* 选择periph_clk2 */
        per_clk2_podf = (reg >> 27) & 0x7;    /* 获取PERIPH_CLK2_PODF */
        reg = read32(&ccm->cbcmr);
        reg = (reg >> 12) & 0x3;    /* PERIPH_CLK2_SEL */

        /* PERIPH_CLK2_SEL: 0表示pll3_sw_clk，1表示osc_clk，2表示pll2_bypass_clk（即osc_clk） */
        switch (reg) {
        case 0:
            freq = get_pll(USB1_PLL);
            break;
        case 1:
        case 2:
            freq = CKIH;
            break;
        default:
            break;
        }

        freq /= (per_clk2_podf + 1);
    } else {                        /* 选择pre_periph_clk */
        reg = read32(&ccm->cbcmr);
        reg = (reg >> 18) & 0x3;    /* PRE_PERIPH_CLK_SEL */

        /* PRE_PERIPH_CLK_SEL：0表示PLL2输出，1表示PLL2 PFD2输出，2表示PLL2 PFD0输出，3表示PLL2 PFD2输出频率的一半 */
        switch (reg) {
        case 0:
            freq = get_pll(SYS_PLL);
            break;
        case 1:
            freq = get_pll_pfd(SYS_PLL, PFD2);
            break;
        case 2:
            freq = get_pll_pfd(SYS_PLL, PFD0);
            break;
        case 3:     /* static / 2 divider */
            freq = get_pll_pfd(SYS_PLL, PFD2) / 2;
            break;
        default:
            break;
        }
    }

    return freq;
}

/**********************************************************************
 * 函数名称： get_ahb_clk_root
 * 功能描述： 获取AHB_CLK_ROOT的频率
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： AHB_CLK_ROOT的频率
 * 修改日期        版本号     修改人       修改内容
 * -----------------------------------------------
 * 2020/03/08     V1.0      今朝        创建
 ***********************************************************************/
u32 get_ahb_clk_root(void)
{
    u32 reg, ahb_podf;

    reg = read32(&ccm->cbcdr);
    ahb_podf = (reg >> 10) & 0x7;   /* 获取AHB_PODF的分频设置 */

    return get_periph_clk() / (ahb_podf + 1);
}

/**********************************************************************
 * 函数名称： get_ipg_clk_root
 * 功能描述： 获取IPG_CLK_ROOT的频率
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： IPG_CLK_ROOT的频率
 * 修改日期        版本号     修改人       修改内容
 * -----------------------------------------------
 * 2020/03/08     V1.0      今朝        创建
 ***********************************************************************/
u32 get_ipg_clk_root(void)
{
    u32 reg, ipg_podf;

    reg = read32(&ccm->cbcdr);
    ipg_podf = (reg >> 8) & 0x3;    /* 获取IPG_PODF的分频设置 */

    return get_ahb_clk_root() / (ipg_podf + 1);
}

/**********************************************************************
 * 函数名称： get_axi_clk_root
 * 功能描述： 获取AXI_CLK_ROOT的频率
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： AXI_CLK_ROOT的频率
 * 修改日期        版本号     修改人       修改内容
 * -----------------------------------------------
 * 2020/03/08     V1.0      今朝        创建
 ***********************************************************************/
u32 get_axi_clk_root(void)
{
    u32 root_freq, axi_podf;
    u32 reg =  read32(&ccm->cbcdr);

    axi_podf = (reg >> 16) & 0x7;   /* 获取AXI_PODF的分频设置 */

    if (reg & (1u << 6)) {          /* AXI_SEL: 1表示axi_alt_clk，0表示periph_clk */
        if (reg & (1u << 7))        /* AXI_ALT_SEL：1表示PLL3 PFD1的输出，0表示PLL2 PFD2的输出 */
            root_freq = get_pll_pfd(USB1_PLL, PFD1);
        else
            root_freq = get_pll_pfd(SYS_PLL, PFD2);
    } else
        root_freq = get_periph_clk();   /* periph_clk */

    return  root_freq / (axi_podf + 1);
}

/**********************************************************************
 * 函数名称： get_fabric_mmdc_clk_root
 * 功能描述： 获取FABRIC_MMDC_CLK_ROOT的频率
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： FABRIC_MMDC_CLK_ROOT的频率
 * 修改日期        版本号     修改人       修改内容
 * -----------------------------------------------
 * 2020/03/08     V1.0      今朝        创建
 ***********************************************************************/
u32 get_fabric_mmdc_clk_root(void)
{
    u32 cbcmr = read32(&ccm->cbcmr);
    u32 cbcdr = read32(&ccm->cbcdr);

    u32 freq, podf, per2_clk2_podf;

    podf = (cbcdr >> 3) & 0x7;  /* 获取FABRIC_MMDC_PODF的分频设置 */

    if (cbcdr & (1u << 26)) {   /* PERIPH2_CLK_SEL: 1选择periph2_clk2，0选择pre_periph2_clk */
        per2_clk2_podf = cbcdr & 0x7;   /* 获取PERIPH2_CLK2_PODF的分频设置 */
        if (cbcmr & (1u << 20)) /* PERIPH2_CLK2_SEL：1选择osc_clk，0选择pll3_sw_clk */
            freq = CKIH;
        else
            freq = get_pll(USB1_PLL);

        freq /= (per2_clk2_podf + 1);
    } else {                    /* pre_periph2_clk */
        extern u32 get_pll4_main_clk(void);
        /* PRE_PERIPH2_CLK_SEL：0表示PLL2输出，1表示PLL2 PFD2输出，2表示PLL2 PFD0输出，3表示pll4_main_clk */
        switch ((cbcmr >> 21) & 0x3) {
            case 0:
                freq = get_pll(SYS_PLL);
                break;
            case 1:
                freq = get_pll_pfd(SYS_PLL, PFD2);
                break;
            case 2:
                freq = get_pll_pfd(SYS_PLL, PFD0);
                break;
            case 3:
                freq = get_pll4_main_clk();
                break;
        }
    }

    return freq / (podf + 1);
}
