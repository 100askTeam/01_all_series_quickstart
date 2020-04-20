#ifndef _TYPE_H__
#define _TYPE_H__

typedef   signed          char int8_t;
typedef   signed short     int int16_t;
typedef   signed           int int32_t;
typedef unsigned          char uint8_t;
typedef unsigned short     int uint16_t;
typedef unsigned           int uint32_t;
typedef unsigned long     long uint64_t;
typedef	  signed char  	 	   s8;		
typedef	  signed short 	  int  s16;
typedef	  signed int 		   s32;
typedef	  signed long long int s64;
typedef	unsigned char 		   u8;
typedef	unsigned short int     u16;
typedef	unsigned int 		   u32;
typedef	unsigned long long int u64;

#define IOMUXC_UART4_TX_DATA_I2C1_SCL                        0x020E00B4U, 0x2U, 0x020E05A4U, 0x1U, 0x020E0340U
#define IOMUXC_UART4_RX_DATA_I2C1_SDA                        0x020E00B8U, 0x2U, 0x020E05A8U, 0x2U, 0x020E0344U

/*! @name SW_MUX_CTL_PAD - SW_MUX_CTL_PAD_JTAG_MOD SW MUX Control Register..SW_MUX_CTL_PAD_CSI_DATA07 SW MUX Control Register */
#define IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_MASK      (0xFU)  /* Merged from fields with different position or width, of widths (3, 4), largest definition used */
#define IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_SHIFT     (0U)
#define IOMUXC_SW_MUX_CTL_PAD_MUX_MODE(x)        (((uint32_t)(((uint32_t)(x)) << IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_SHIFT)) & IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_MASK)  /* Merged from fields with different position or width, of widths (3, 4), largest definition used */
#define IOMUXC_SW_MUX_CTL_PAD_SION_MASK          (0x10U)
#define IOMUXC_SW_MUX_CTL_PAD_SION_SHIFT         (4U)
#define IOMUXC_SW_MUX_CTL_PAD_SION(x)            (((uint32_t)(((uint32_t)(x)) << IOMUXC_SW_MUX_CTL_PAD_SION_SHIFT)) & IOMUXC_SW_MUX_CTL_PAD_SION_MASK)
/*! @name SELECT_INPUT - USB_OTG1_ID_SELECT_INPUT DAISY Register..USDHC2_WP_SELECT_INPUT DAISY Register */
#define IOMUXC_SELECT_INPUT_DAISY_MASK           (0x7U)  /* Merged from fields with different position or width, of widths (1, 2, 3), largest definition used */
#define IOMUXC_SELECT_INPUT_DAISY_SHIFT          (0U)
#define IOMUXC_SELECT_INPUT_DAISY(x)             (((uint32_t)(((uint32_t)(x)) << IOMUXC_SELECT_INPUT_DAISY_SHIFT)) & IOMUXC_SELECT_INPUT_DAISY_MASK)  /* Merged from fields with different position or width, of widths (1, 2, 3), largest definition used */

static inline void IOMUXC_SetPinMux(uint32_t muxRegister,
                                    uint32_t muxMode,
                                    uint32_t inputRegister,
                                    uint32_t inputDaisy,
                                    uint32_t configRegister,
                                    uint32_t inputOnfield)
{
    *((volatile uint32_t *)muxRegister) =
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE(muxMode) | IOMUXC_SW_MUX_CTL_PAD_SION(inputOnfield);

    if (inputRegister)
    {
        *((volatile uint32_t *)inputRegister) = IOMUXC_SELECT_INPUT_DAISY(inputDaisy);
    }
}
static inline void IOMUXC_SetPinConfig(uint32_t muxRegister,
									   uint32_t muxMode,
									   uint32_t inputRegister,
									   uint32_t inputDaisy,
									   uint32_t configRegister,
									   uint32_t configValue)
{
	if (configRegister)
	{
		*((volatile uint32_t *)configRegister) = configValue;
	}
}

#endif