#ifndef IMX6ULL_REGS_H
#define IMX6ULL_REGS_H

#include "common.h"

#define CCM_BASE_ADDR   0x020C4000

struct ccm_regs {
	u32 ccr;	/* 0x0000 */
	u32 ccdr;
	u32 csr;
	u32 ccsr;
	u32 cacrr;	/* 0x0010*/
	u32 cbcdr;
	u32 cbcmr;
	u32 cscmr1;
	u32 cscmr2;	/* 0x0020 */
	u32 cscdr1;
	u32 cs1cdr;
	u32 cs2cdr;
	u32 cdcdr;	/* 0x0030 */
	u32 chsccdr;
	u32 cscdr2;
	u32 cscdr3;
	u32 cscdr4;	/* 0x0040 */
	u32 resv0;
	u32 cdhipr;
	u32 cdcr;
	u32 ctor;	/* 0x0050 */
	u32 clpcr;
	u32 cisr;
	u32 cimr;
	u32 ccosr;	/* 0x0060 */
	u32 cgpr;
	u32 CCGR0;
	u32 CCGR1;
	u32 CCGR2;	/* 0x0070 */
	u32 CCGR3;
	u32 CCGR4;
	u32 CCGR5;
	u32 CCGR6;	/* 0x0080 */
	u32 CCGR7;
	u32 cmeor;
};

#define ANADIG_BASE_ADDR   0x020C8000
struct anadig_regs {
	u32 analog_pll_arm;			/* 0x8000 */
	u32 analog_pll_arm_set;
	u32 analog_pll_arm_clr;
	u32 analog_pll_arm_tog;
	u32 analog_pll_usb1;		/* 0x8010 */
	u32 analog_pll_usb1_set;
	u32 analog_pll_usb1_clr;
	u32 analog_pll_usb1_tog;
	u32 analog_pll_usb2;		/* 0x8020 */
	u32 analog_pll_usb2_set;
	u32 analog_pll_usb2_clr;
	u32 analog_pll_usb2_tog;
	u32 analog_pll_sys;			/* 0x8030 */
	u32 analog_pll_sys_set;
	u32 analog_pll_sys_clr;
	u32 analog_pll_sys_tog;
	u32 analog_pll_sys_ss;	    /* 0x8040 */
	u32 analog_reserved1[3];
	u32 analog_pll_sys_num;		/* 0x8050 */
	u32 analog_reserved2[3];
	u32 analog_pll_sys_denom;	/* 0x8060 */
	u32 analog_reserved3[3];
	u32 analog_pll_audio;			/* 0x8070 */
	u32 analog_pll_audio_set;
	u32 analog_pll_audio_clr;
	u32 analog_pll_audio_tog;
	u32 analog_pll_audio_num;		/* 0x8080*/
	u32 analog_reserved4[3];
	u32 analog_pll_audio_denom;		/* 0x8090 */
	u32 analog_reserved5[3];
	u32 analog_pll_video;			/* 0x80a0 */
	u32 analog_pll_video_set;
	u32 analog_pll_video_clr;
	u32 analog_pll_video_tog;
	u32 analog_pll_video_num;		/* 0x80b0 */
	u32 analog_reserved6[3];
	u32 analog_pll_video_denom;		/* 0x80c0 */
	u32 analog_reserved7[7];
	u32 analog_pll_enet;			/* 0x80e0 */
	u32 analog_pll_enet_set;
	u32 analog_pll_enet_clr;
	u32 analog_pll_enet_tog;
	u32 analog_pfd_480;			/* 0x80f0 */
	u32 analog_pfd_480_set;
	u32 analog_pfd_480_clr;
	u32 analog_pfd_480_tog;
	u32 analog_pfd_528;			/* 0x8100 */
	u32 analog_pfd_528_set;
	u32 analog_pfd_528_clr;
	u32 analog_pfd_528_tog;
	/* PMU Memory Map/Register Definition */
	u32 pmu_reg_1p1;
	u32 pmu_reg_1p1_set;
	u32 pmu_reg_1p1_clr;
	u32 pmu_reg_1p1_tog;
	u32 pmu_reg_3p0;
	u32 pmu_reg_3p0_set;
	u32 pmu_reg_3p0_clr;
	u32 pmu_reg_3p0_tog;
	u32 pmu_reg_2p5;
	u32 pmu_reg_2p5_set;
	u32 pmu_reg_2p5_clr;
	u32 pmu_reg_2p5_tog;
	u32 pmu_reg_core;
	u32 pmu_reg_core_set;
	u32 pmu_reg_core_clr;
	u32 pmu_reg_core_tog;
	u32 pmu_misc0;
	u32 pmu_misc0_set;
	u32 pmu_misc0_clr;
	u32 pmu_misc0_tog;
	u32 pmu_misc1;
	u32 pmu_misc1_set;
	u32 pmu_misc1_clr;
	u32 pmu_misc1_tog;
	u32 pmu_misc2;
	u32 pmu_misc2_set;
	u32 pmu_misc2_clr;
	u32 pmu_misc2_tog;
};


#endif /* IMX6ULL_REGS_H */
