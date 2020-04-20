#ifndef PLL_H
#define PLL_H

#include "common.h"

#define CKIH    24000000

typedef enum {
    ARM_PLL,
    USB1_PLL,
    USB2_PLL,
    SYS_PLL,
    AUDIO_PLL,
    VIDEO_PLL,
    ENET_PLL
} pll_e;

typedef enum {
    PFD0 = 0,
    PFD1,
    PFD2,
    PFD3,
} pfd_e;

#define PFD_SHIFT(pfd)          (pfd*8)
#define PFD_MASK(pfd)           (0xFF<<PFD_SHIFT(pfd))
#define PFD_GATE_MASK(pfd)      (0x80<<PFD_SHIFT(pfd))
#define PFD_STABLE_MASK(pfd)    (0x40<<PFD_SHIFT(pfd))
#define PFD_FRAC_MASK(pfd)      (0x3F<<PFD_SHIFT(pfd))
#define PFD_FRAC_VALUE(value, pfd)      ((value>>PFD_SHIFT(pfd)) & 0x3F)

/* general definations */
#define LOCK_MASK       (1u<<31)
#define BYPASS_MASK     (1u<<16)
#define ENABLE_MASK     (1u<<13)
#define POWERDOWN_MASK  (1u<<12)

void set_pll(pll_e pll, u32 div);
u32 get_pll(pll_e pll);
void set_pll_pfd(pll_e pll, pfd_e pfd, int gate, u32 frac);
u32 get_pll_pfd(pll_e pll, pfd_e pfd);

#endif  /* PLL_H */
