#include "common.h"
#include "regs.h"

#define IOMUXC_SW_MUX_CTL_PAD_JTAG_TDO  0x020E004C
#define IOMUXC_JTAG_TDO_MODE_CCM_CLKO2  3   /* JTAG_TDO(ALT3) as CLKO2 */

#define IOMUXC_SW_MUX_CTL_PAD_JTAG_TMS  0x020E0048
#define IOMUXC_JTAG_TMS_MODE_CCM_CLKO1  3   /* JTAG_TMS(ALT3) as CLKO1 */

#define CLKO2_EN            (1u<<24)
#define CLKO2_DIV_SHIFT     21
#define CLKO2_DIV_MASK      (0x7<<CLKO2_DIV_SHIFT)
#define CLKO2_DIV(div)      ((div<<CLKO2_DIV_SHIFT) & CLKO2_DIV_MASK)
#define CLKO2_SEL_SHIFT     16
#define CLKO2_SEL_MASK      (0x1F<<CLKO2_SEL_SHIFT)
#define CLKO2_SEL(sel)      ((sel<<CLKO2_SEL_SHIFT) & CLKO2_SEL_MASK)

#define CLKO1_EN            (1u<<7)
#define CLKO1_DIV_SHIFT     4
#define CLKO1_DIV_MASK      (0x7<<CLKO1_DIV_SHIFT)
#define CLKO1_DIV(div)      ((div<<CLKO1_DIV_SHIFT) & CLKO1_DIV_MASK)
#define CLKO1_SEL_SHIFT     0
#define CLKO1_SEL_MASK      (0xF<<CLKO1_SEL_SHIFT)
#define CLKO1_SEL(sel)      ((sel<<CLKO1_SEL_SHIFT) & CLKO1_SEL_MASK)

#define CLKO1_TO_CLKO2      (1u<<8)

extern struct ccm_regs *ccm;

void setup_clock_output(void)
{
    /* setup the CLKO1 and CLKO2 pads */
    write32(IOMUXC_JTAG_TMS_MODE_CCM_CLKO1, (u32 *)IOMUXC_SW_MUX_CTL_PAD_JTAG_TMS);
    write32(IOMUXC_JTAG_TDO_MODE_CCM_CLKO2, (u32 *)IOMUXC_SW_MUX_CTL_PAD_JTAG_TDO);

    /* select the clocks, CLKO2: /8, ARM_ROOT; CLKO1: /8, AXI_ROOT */
    u32 value = CLKO2_EN | CLKO2_DIV(7) | CLKO2_SEL(10) |
         CLKO1_EN | CLKO1_DIV(7) | CLKO1_SEL(5);
    write32(value, &ccm->ccosr);
}
