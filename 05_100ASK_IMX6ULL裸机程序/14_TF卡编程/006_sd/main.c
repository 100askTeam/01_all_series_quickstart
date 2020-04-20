
#include "led.h"
#include "common.h"
#include "uart.h"
#include "sd.h"
#include "led.imx.h"

//static u8 sd_align_buf[1024] __attribute__ ((aligned(1024)));
static u8 sd_read_buf[4096];

void init_pins(void)
{
	/* led pins start */
    IOMUXC_SetPinMux(IOMUXC_SNVS_SNVS_TAMPER3_GPIO5_IO03, 0U);
    IOMUXC_SetPinConfig(IOMUXC_SNVS_SNVS_TAMPER3_GPIO5_IO03, 
                        IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_DSE(1U) |
                        IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
	/* led pins end */

	/* uart1 pins start*/	
	IOMUXC_SetPinMux(IOMUXC_UART1_RX_DATA_UART1_RX, 0U);
	IOMUXC_SetPinConfig(IOMUXC_UART1_RX_DATA_UART1_RX, 
						IOMUXC_SW_PAD_CTL_PAD_DSE(2U) |
						IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
						IOMUXC_SW_PAD_CTL_PAD_PKE_MASK);
	IOMUXC_SetPinMux(IOMUXC_UART1_TX_DATA_UART1_TX, 0U);
	IOMUXC_SetPinConfig(IOMUXC_UART1_TX_DATA_UART1_TX, 
						IOMUXC_SW_PAD_CTL_PAD_DSE(2U) |
						IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
						IOMUXC_SW_PAD_CTL_PAD_PKE_MASK);
	/* uart1 pins end*/

	/* uSDHC1 pins start*/
	IOMUXC_SetPinMux(IOMUXC_UART1_RTS_B_USDHC1_CD_B, 0U);
    IOMUXC_SetPinConfig(IOMUXC_UART1_RTS_B_USDHC1_CD_B, 
                        IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |
                        IOMUXC_SW_PAD_CTL_PAD_DSE(1U) |
                        IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
	IOMUXC_SetPinMux(IOMUXC_SD1_CLK_USDHC1_CLK, 0U);
	IOMUXC_SetPinConfig(IOMUXC_SD1_CLK_USDHC1_CLK, 
						IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_DSE(1U) |
						IOMUXC_SW_PAD_CTL_PAD_SPEED(1U) |
						IOMUXC_SW_PAD_CTL_PAD_PKE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_PUS(1U) |
						IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
	IOMUXC_SetPinMux(IOMUXC_SD1_CMD_USDHC1_CMD, 0U);
	IOMUXC_SetPinConfig(IOMUXC_SD1_CMD_USDHC1_CMD, 
						IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_DSE(1U) |
						IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
						IOMUXC_SW_PAD_CTL_PAD_PKE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_PUS(1U) |
						IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
	IOMUXC_SetPinMux(IOMUXC_SD1_DATA0_USDHC1_DATA0, 0U);
	IOMUXC_SetPinConfig(IOMUXC_SD1_DATA0_USDHC1_DATA0, 
						IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_DSE(1U) |
						IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
						IOMUXC_SW_PAD_CTL_PAD_PKE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_PUS(1U) |
						IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
	IOMUXC_SetPinMux(IOMUXC_SD1_DATA1_USDHC1_DATA1, 0U);
	IOMUXC_SetPinConfig(IOMUXC_SD1_DATA1_USDHC1_DATA1, 
						IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_DSE(1U) |
						IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
						IOMUXC_SW_PAD_CTL_PAD_PKE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_PUS(1U) |
						IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
	IOMUXC_SetPinMux(IOMUXC_SD1_DATA2_USDHC1_DATA2, 0U);
	IOMUXC_SetPinConfig(IOMUXC_SD1_DATA2_USDHC1_DATA2, 
						IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_DSE(1U) |
						IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
						IOMUXC_SW_PAD_CTL_PAD_PKE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_PUS(1U) |
						IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
	IOMUXC_SetPinMux(IOMUXC_SD1_DATA3_USDHC1_DATA3, 0U);
	IOMUXC_SetPinConfig(IOMUXC_SD1_DATA3_USDHC1_DATA3, 
						IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_DSE(1U) |
						IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
						IOMUXC_SW_PAD_CTL_PAD_PKE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_PUS(1U) |
						IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
	/* uSDHC1 pins end*/

	/* uSDHC2 pins start*/
	IOMUXC_SetPinMux(IOMUXC_NAND_WE_B_USDHC2_CMD, 0U);
	IOMUXC_SetPinConfig(IOMUXC_NAND_WE_B_USDHC2_CMD, 
						IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_DSE(7U) |
						IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
						IOMUXC_SW_PAD_CTL_PAD_PKE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_PUS(1U) |
						IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
	IOMUXC_SetPinMux(IOMUXC_NAND_RE_B_USDHC2_CLK, 0U);
	IOMUXC_SetPinConfig(IOMUXC_NAND_RE_B_USDHC2_CLK, 
						IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_DSE(1U) |
						IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
						IOMUXC_SW_PAD_CTL_PAD_PKE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_PUS(1U) |
						IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
	IOMUXC_SetPinMux(IOMUXC_NAND_ALE_USDHC2_RESET_B, 0U);
	IOMUXC_SetPinConfig(IOMUXC_NAND_ALE_USDHC2_RESET_B, 
						IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_DSE(1U) |
						IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
						IOMUXC_SW_PAD_CTL_PAD_PKE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_PUS(1U) |
						IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
	IOMUXC_SetPinMux(IOMUXC_NAND_DATA00_USDHC2_DATA0, 0U);
	IOMUXC_SetPinConfig(IOMUXC_NAND_DATA00_USDHC2_DATA0, 
						IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_DSE(7U) |
						IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
						IOMUXC_SW_PAD_CTL_PAD_PKE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_PUS(1U) |
						IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
	IOMUXC_SetPinMux(IOMUXC_NAND_DATA01_USDHC2_DATA1, 0U);
	IOMUXC_SetPinConfig(IOMUXC_NAND_DATA01_USDHC2_DATA1, 
						IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_DSE(7U) |
						IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
						IOMUXC_SW_PAD_CTL_PAD_PKE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_PUS(1U) |
						IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
	IOMUXC_SetPinMux(IOMUXC_NAND_DATA02_USDHC2_DATA2, 0U);
	IOMUXC_SetPinConfig(IOMUXC_NAND_DATA02_USDHC2_DATA2, 
						IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_DSE(7U) |
						IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
						IOMUXC_SW_PAD_CTL_PAD_PKE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_PUS(1U) |
						IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
	IOMUXC_SetPinMux(IOMUXC_NAND_DATA03_USDHC2_DATA3, 0U);
	IOMUXC_SetPinConfig(IOMUXC_NAND_DATA03_USDHC2_DATA3, 
						IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_DSE(7U) |
						IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
						IOMUXC_SW_PAD_CTL_PAD_PKE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_PUS(1U) |
						IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
	IOMUXC_SetPinMux(IOMUXC_NAND_DATA04_USDHC2_DATA4, 0U);
	IOMUXC_SetPinConfig(IOMUXC_NAND_DATA04_USDHC2_DATA4, 
						IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_DSE(7U) |
						IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
						IOMUXC_SW_PAD_CTL_PAD_PKE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_PUS(1U) |
						IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
	IOMUXC_SetPinMux(IOMUXC_NAND_DATA05_USDHC2_DATA5, 0U);
	IOMUXC_SetPinConfig(IOMUXC_NAND_DATA05_USDHC2_DATA5, 
						IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_DSE(7U) |
						IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
						IOMUXC_SW_PAD_CTL_PAD_PKE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_PUS(1U) |
						IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
	IOMUXC_SetPinMux(IOMUXC_NAND_DATA06_USDHC2_DATA6, 0U);
	IOMUXC_SetPinConfig(IOMUXC_NAND_DATA06_USDHC2_DATA6, 
						IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_DSE(7U) |
						IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
						IOMUXC_SW_PAD_CTL_PAD_PKE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_PUS(7U) |
						IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);
	IOMUXC_SetPinMux(IOMUXC_NAND_DATA07_USDHC2_DATA7, 0U);
	IOMUXC_SetPinConfig(IOMUXC_NAND_DATA07_USDHC2_DATA7, 
						IOMUXC_SW_PAD_CTL_PAD_SRE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_DSE(7U) |
						IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
						IOMUXC_SW_PAD_CTL_PAD_PKE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
						IOMUXC_SW_PAD_CTL_PAD_PUS(1U) |
						IOMUXC_SW_PAD_CTL_PAD_HYS_MASK);


	/* uSDHC2 pins end*/

}

void delay(volatile unsigned int d)
{
	while(d--);
}

void boot_clk_gate_init(void)
{
    CCM->CCGR0 = 0xFFFFFFFFU;
    CCM->CCGR1 = 0xFFFFFFFFU;
    CCM->CCGR2 = 0xFFFFFFFFU;
    CCM->CCGR3 = 0xFFFFFFFFU;
    CCM->CCGR4 = 0xFFFFFFFFU;
    CCM->CCGR5 = 0xFFFFFFFFU;
    CCM->CCGR6 = 0xFFFFFFFFU;
}

void boot_clk_init(void)
{
	if ((CCM->CCSR & (1 << 2)) == 0 ) /* CPU runs on ARM PLL : bit2 pll1_sw_clk_sel*/
	{
		CCM->CCSR &= ~(1 << 8);/* Set Step MUX to OSC : bit8 STEP_SEL */
		CCM->CCSR |= (1 << 2);/* Let CPU run on Step MUX : bit2 pll1_sw_clk_sel*/
	}

	CCM_ANALOG->PLL_ARM = (1 << 13) | (88 << 0); /*Configure ARM PLL to 1056M:bit13 Enable, bit0~6 DIV_SELECT*/
	CCM->CCSR &= ~(1 << 2); /* Now CPU runs again on ARM PLL*/
	CCM->CACRR = 1; /* Configure ARM clock root with divide 2 */

#if 0
	/* Set periph2_clk2 and periph_clk2 MUX to OSC */
	CCM->CBCMR &= ~((3 << 20) | ( 3 << 12));
	CCM->CBCMR |= (1 << 20) | ( 1 << 12);
	/* Let BUS and mmdc clock run on OSC */
	CCM->CBCDR &= ~((1 << 6) | ( 1 << 25) | ( 1 << 26));
	CCM->CBCDR |= (0 << 6) | ( 1 << 25) | ( 1 << 26);
	/* Wait handshake process */
	while (CCM->CDHIPR & ((1 << 5) | (1 << 3)))
	{
	}

	/* Configure SYS PLL to 528M */
	CCM_ANALOG->PLL_SYS_SS &= (1 << 15);
	CCM_ANALOG->PLL_SYS_NUM = 0; 
	CCM_ANALOG->PLL_SYS = (1 << 13) | (1 << 0);
	/* wait for PLL is locked */
	while ((CCM_ANALOG->PLL_SYS & (1 << 31)) == 0)
	{
	}
#endif

	 /* Configure PFD_528
	 * PFD0     : Enable, set divider 27 352M
	 * PFD1     : Enable, set divider 16 594M
	 * PFD2     : Enable, set divider 24 396M
	 * PFD3     : Enable, set divider 48 198M
	 * freq=528*18/PFDn_FRAC
	 */
	 /* Disable all clock output first. */
	//CCM_ANALOG->PFD_528 |= (1 << 31) | ( 1 << 23) | (1 << 15) | ( 1 << 7);
	/* Clear first, then Set default divide value for all PFD. */
	CCM_ANALOG->PFD_528 &= ~((0x3F << 24) | (0x3F << 16) | (0x3F << 8) | (0x3F << 0));
	CCM_ANALOG->PFD_528 |= (0x30 << 24) | (0x18 << 16) | (0x10 << 8) | (0x1B << 0);
	/* Enable PFD_528 PFD0~3*/
	//CCM_ANALOG->PFD_528 &= ~((1 << 31) | ( 1 << 23) | (1 << 15) | ( 1 << 7));
	

	/* Configure PFD_480
	 * PFD0     : Enable, set divider 12 720M
	 * PFD1     : Enable, set divider 16 540M
	 * PFD2     : Enable, set divider 17 508.2M
	 * PFD3     : Enable, set divider 19 454.7M
	 * freq=480*18/PFDn_FRAC
	 */
	/* Disable all clock output. */
	//CCM_ANALOG->PFD_480 |= (1 << 31) | ( 1 << 23) | (1 << 15) | ( 1 << 7);
	/* Clear first, then Set default divide value for all PFD. */
	CCM_ANALOG->PFD_480 &= ~((0x3F << 24) | (0x3F << 16) | (0x3F << 8) | (0x3F << 0));
	CCM_ANALOG->PFD_480 |= (0x13 << 24) | (0x11 << 16) | (0x10 << 8) | (0xC << 0);
	/* Enable PFD_528 PFD0~3*/
	//CCM_ANALOG->PFD_480 &= ~((1 << 31) | ( 1 << 23) | (1 << 15) | ( 1 << 7));

    /* Configure BUS clcok
     * AHB     : Sourced from SYS PLL PFD2 396M with divider 3 132M
     * IPG     : Sourced from AHB with divider 2 66M
     * AXI     : Sourced from SYS PLL PFD2 396M with divider 2 198M
     * MMDC    : Sourced from SYS PLL PFD2 396M with divider 1 396M
     */
     /*
      *AHB_CLK_ROOT<= PLL2 PFD2(pre_periph_clk=1 periph_clk=0 ahb_podf=3)
      *IPG_CLK_ROOT<=AHB_CLK_ROOT(ipg_pdof=2)
      *AXI_CLK_ROOT<= PLL2 PFD2(AXI_pdof=2)
      */

    /* Set pre_periph2_clk and pre_periph_clk MUX to SYS PLL PFD2*/
    CCM->CBCMR &= ~((3 << 18) | ( 3 << 21));
	CCM->CBCMR |= (1 << 18) | ( 1 << 21);

    /* Set periph2_clk and periph_clk MUX to PLL2*/
    CCM->CBCDR &= ~((1 << 26) | ( 1 << 25));
	CCM->CBCDR |= (0 << 26) | ( 0 << 25);//no use, just for tidy

	/* Config AXI divide by 2, AHB divide by 3, IPG divide by 2 */
	//CCM->CBCDR &= ~((7 << 16) | ( 7 << 10) | ( 3 << 8));
	//CCM->CBCDR |= (1 << 16) | ( 2 << 10) | ( 1 << 8);
	/* Config AXI divide by 2, IPG divide by 2 */
	CCM->CBCDR &= ~((7 << 16) | ( 3 << 8));
	CCM->CBCDR |= (1 << 16) | ( 1 << 8);

	/* Config other filed in CBCDR to certain value */
	/* AXI root clock from PLL2 PFD2, PERIPH_CLK2_PODF=0,PERIPH2_CLK2_PODF=0*/
	CCM->CBCDR &= ~((1 << 7) | ( 7 << 27) | ( 7 << 0));
	CCM->CBCDR |= (0 << 7) | ( 0 << 27) | ( 0 << 0);

	/* Wait handshake process */
	while (CCM->CDHIPR & ((1 << 5) | (1 << 3) | (1 << 2) | (1 << 1) | (1 << 0)))
	{
	}

	/* Select PLL3 to generate pll3_sw_clk from pll3_main_clk */
    CCM->CCSR &= ~(1 << 0);

    CCM->CSCDR1 &= ~(1 << 6); /* Set UART source to PLL3 80M */
    CCM->CSCDR1 &= ~(0x3F << 0); /* Set UART divider to 1 */

	/* Set PERCLK_CLK_ROOT (I2C/EPIT source) to IPG 66M */
	CCM->CSCMR1 &= ~(1 << 6); /* Set I2C/EPIT source to IPG 66M */
    CCM->CSCMR1 &= ~(0x3F << 0); /* Set I2C/EPIT divider to 1 */

}

int  main()
{
	unsigned char c;
	int i;

	init_pins();
	led_gpio_init();
	led_ctl(0);//turn off led
	boot_clk_gate_init();
	boot_clk_init();
	uart1_init();
	puts("hello world\r\n");
	//my_printf_test();
	//printf("UART1->UFCR is 0x%x\n\r",    UART1->UFCR);
	//printf("UART1->UBIR is 0x%x\n\r",    UART1->UBIR);
	//printf("UART1->UBMR is 0x%x\n\r",    UART1->UBMR);
	//printf("CCM->CSCDR1 is 0x%x\n\r", CCM->CSCDR1);

	printf("1:test sd\r\n");
	printf("2:test eMMC\r\n");
	printf("please select:(1 or 2)\r\n");
	c = getchar();
	if(c == '1' ) {
		//make sure sd card inserted first
		printf("please insert sd card first, then enter 'c'\r\nEnter:");
		c = getchar();
		if (c == 'c') {
			printf("start test sd\r\n");
			sd_init(USDHC1);

			memset(sd_read_buf, 0, sizeof(sd_read_buf));
			sd_read_blocks(USDHC1, sd_read_buf, 2, 2);

			printf("read 2 sectors from 2nd sector\r\n");
			for(i = 0; i < 1024; i++) {
				printf("%02x ", sd_read_buf[i]);
				if((i + 1)%16 == 0)
					printf("\r\n");
			}

			printf("burn led.imx to sd\r\n");
			sd_write_blocks(USDHC1, led_imx_image + 1024, 2, (sizeof(led_imx_image) - 1024) >> 9);
			printf("please set dip switch to SD boot, and push reset, then green led blink \r\n");

			memset(sd_read_buf, 0, sizeof(sd_read_buf));
			sd_read_blocks(USDHC1, sd_read_buf, 2, 2);

			printf("read 2 sectors from 2nd sector after burn led_imx_image:\r\n");
			for(i = 0; i < 1024; i++) {
				printf("%02x ", sd_read_buf[i]);
				if((i + 1)%16 == 0)
					printf("\r\n");
			}			
			printf("please set dip switch to SD boot, and push reset, then green led blink \r\n");			
		} else {
		printf("input wrong number failed\r\n");
		}
	}else if (c == '2') {
		mmc_init(USDHC2);
		MMC_SelectBoot1Partition(USDHC2);

		memset(sd_read_buf, 0, sizeof(sd_read_buf));
		sd_read_blocks(USDHC2, sd_read_buf, 2, 2);

		printf("read 2 sectors from eMMC boot1 partition 2nd sector\r\n");
		for(i = 0; i < 1024; i++) {
			printf("%02x ", sd_read_buf[i]);
			if((i + 1)%16 == 0)
				printf("\r\n");
		}

		/* wirte led_imx_image to eMMC boot1 partition */
		sd_write_blocks(USDHC2, led_imx_image + 1024, 2, (sizeof(led_imx_image) - 1024) >> 9);
		printf("please set dip switch to eMMC boot, and push reset, then green led blink\r\n");
	
		printf("read 2 sectors from eMMC boot1 part 2nd sector after burn led_imx_image\r\n");
		memset(sd_read_buf, 0, sizeof(sd_read_buf));
		sd_read_blocks(USDHC2, sd_read_buf, 2, 2);
		MMC_ExitBoot1Partition(USDHC2);

		for(i = 0; i < 1024; i++) {
			printf("%02x ", sd_read_buf[i]);
			if((i + 1)%16 == 0)
				printf("\r\n");
		}	
	} else {
		printf("input wrong number failed\r\n");
	}


#if 0
/*****/
//test read and write
	mmc_init(USDHC2);

	memset(sd_read_buf, 0, sizeof(sd_read_buf));
	sd_read_blocks(USDHC2, sd_read_buf, 1000, 4);
	for(i = 0; i <2048; i++) {
		printf("%02x ", sd_read_buf[i]);
		if((i + 1)%16 == 0)
			printf("\r\n");

	memset(sd_read_buf, 0x55, sizeof(sd_read_buf));
	sd_write_blocks(USDHC2, sd_read_buf, 1000, 2);
	memset(sd_read_buf, 0xAA, sizeof(sd_read_buf));
	sd_write_blocks(USDHC2, sd_read_buf, 1002, 2);

	memset(sd_read_buf, 0, sizeof(sd_read_buf));
	sd_read_blocks(USDHC2, sd_read_buf, 1000, 4);
	for(i = 0; i <2048; i++) {
		printf("%02x ", sd_read_buf[i]);
		if((i + 1)%16 == 0)
			printf("\r\n");
	}
/*****/

/*****/
	//burn image to eMMC boot1 partition
	MMC_SelectBoot1Partition(USDHC2);
	sd_write_blocks(USDHC2, led_imx_image + 1024, 2, (sizeof(led_imx_image) - 1024) >> 9);
	MMC_ExitBoot1Partition(USDHC2);
/*****/	

//#else
	printf("please insert sd card first, then enter 'c'\r\n");
	c = getchar();
	if(c == 'c') {	
		printf("start test sd\r\n");
		sd_init(USDHC1);
/*****/
//test sd read and write
	memset(sd_read_buf, 0, sizeof(sd_read_buf));
	sd_read_blocks(USDHC1, sd_read_buf, 1000, 4);
	for(i = 0; i <2048; i++) {
		printf("%02x ", sd_read_buf[i]);
		if((i + 1)%16 == 0)
			printf("\r\n");

	memset(sd_read_buf, 0x55, sizeof(sd_read_buf));
	sd_write_blocks(USDHC1, sd_read_buf, 1000, 2);
	memset(sd_read_buf, 0xAA, sizeof(sd_read_buf));
	sd_write_blocks(USDHC2, sd_read_buf, 1002, 2);

	memset(sd_read_buf, 0, sizeof(sd_read_buf));
	sd_read_blocks(USDHC1, sd_read_buf, 1000, 4);
	for(i = 0; i <2048; i++) {
		printf("%02x ", sd_read_buf[i]);
		if((i + 1)%16 == 0)
			printf("\r\n");
	}
/*****/

/*****/
//burn led_imx_image to sd card
	sd_write_blocks(USDHC1, led_imx_image, 2, (sizeof(led_imx_image) - 1024) << 9);
/*****/

}
#endif
	while(1)
	{
		c = getchar();
		if (c == '\r')
		{
			putchar('\n');
		}

		if (c == '\n')
		{
			putchar('\r');
		}

		putchar(c);

	}
					
	return 0;
}


