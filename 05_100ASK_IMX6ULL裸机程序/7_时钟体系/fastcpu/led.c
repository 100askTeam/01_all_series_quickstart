#include "common.h"

static int led_state = 0;

#define IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER5 0x0229001C
#define IOMUXC_SNVS_SW_PAD_CTL_PAD_SNVS_TAMPER5 0x02290060

#define IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 0x02290014
#define IOMUXC_SNVS_SW_PAD_CTL_PAD_SNVS_TAMPER3 0x02290058

#define GPIO5_DR    0x020AC000
#define GPIO5_GDIR  0x020AC004
#define GPIO5_PSR   0x020AC008

#if 0
volatile u32 *mux_led = (volatile u32 *)IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER5;
volatile u32 *pad_led = (volatile u32 *)IOMUXC_SNVS_SW_PAD_CTL_PAD_SNVS_TAMPER5;
#define LED_OFFSET  5   /* GPIO5_5 */
#else
volatile u32 *mux_led = (volatile u32 *)IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3;
volatile u32 *pad_led = (volatile u32 *)IOMUXC_SNVS_SW_PAD_CTL_PAD_SNVS_TAMPER3;
#define LED_OFFSET  3   /* GPIO5_3 */
#endif

volatile u32 *led_gdir  = (volatile u32 *)GPIO5_GDIR;
volatile u32 *led_dr    = (volatile u32 *)GPIO5_DR;

void led_init(void)
{
    write32(5, mux_led);    /* ALT5 as GPIO */
    set_bit(led_gdir, LED_OFFSET);  /* output */
}

void led_on(void)
{
    clr_bit(led_dr, LED_OFFSET);
    led_state = 1;
}

void led_off(void)
{
    set_bit(led_dr, LED_OFFSET);
    led_state = 0;
}

void led_toggle(void)
{
    if (led_state)
        led_off();
    else
        led_on();
}
