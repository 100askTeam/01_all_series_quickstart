#include "uart.h"

/*static void uart1_pins_int(void)
  {
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

  }
  */

/* This UART instantiation uses a slightly different baud rate calculation.
 * Baud Rate = Ref Freq / (16 * (UBMR + 1)/(UBIR+1)).
 * To get a baud rate, three register need to be writen, UFCR,UBMR and UBIR
 * At first, find the approximately maximum divisor of src_Clock and baudRate_Bps.
 * If the numerator and denominator are larger then register maximum value(0xFFFF),
 * both of numerator and denominator will be divided by the same value, which
 * will ensure numerator and denominator range from 0~maximum value(0xFFFF).
 * Then calculate UFCR and UBIR value from numerator, and get UBMR value from denominator.
 */
static void UART_SetBaudRate(uint32_t baudRate_Bps, uint32_t srcClock_Hz)
{
	uint32_t numerator = 0u;
	uint32_t denominator = 0U;
	uint32_t divisor = 0U;
	uint32_t refFreqDiv = 0U;
	uint32_t divider = 1U;
	uint64_t baudDiff = 0U;
	uint64_t tempNumerator = 0U;
	uint32_t tempDenominator = 0u;

	/* get the approximately maximum divisor */
	numerator = srcClock_Hz;
	denominator = baudRate_Bps << 4;
	divisor = 1;

	while (denominator != 0)
	{
		divisor = denominator;
		denominator = numerator % denominator;
		numerator = divisor;
	}

	numerator = srcClock_Hz / divisor;
	denominator = (baudRate_Bps << 4) / divisor;

	/* numerator ranges from 1 ~ 7 * 64k */
	/* denominator ranges from 1 ~ 64k */
	if ((numerator > (UART_UBIR_INC_MASK * 7)) || (denominator > UART_UBIR_INC_MASK))
	{
		uint32_t m = (numerator - 1) / (UART_UBIR_INC_MASK * 7) + 1;
		uint32_t n = (denominator - 1) / UART_UBIR_INC_MASK + 1;
		uint32_t max = m > n ? m : n;
		numerator /= max;
		denominator /= max;
		if (0 == numerator)
		{
			numerator = 1;
		}
		if (0 == denominator)
		{
			denominator = 1;
		}
	}
	divider = (numerator - 1) / UART_UBIR_INC_MASK + 1;

	switch (divider)
	{
		case 1:
			refFreqDiv = 0x05;
			break;
		case 2:
			refFreqDiv = 0x04;
			break;
		case 3:
			refFreqDiv = 0x03;
			break;
		case 4:
			refFreqDiv = 0x02;
			break;
		case 5:
			refFreqDiv = 0x01;
			break;
		case 6:
			refFreqDiv = 0x00;
			break;
		case 7:
			refFreqDiv = 0x06;
			break;
		default:
			refFreqDiv = 0x05;
			break;
	}
	/* Compare the difference between baudRate_Bps and calculated baud rate.
	 * Baud Rate = Ref Freq / (16 * (UBMR + 1)/(UBIR+1)).
	 * baudDiff = (srcClock_Hz/divider)/( 16 * ((numerator / divider)/ denominator).
	 */
	tempNumerator = srcClock_Hz;
	tempDenominator = (numerator << 4);
	divisor = 1;
	/* get the approximately maximum divisor */
	while (tempDenominator != 0)
	{
		divisor = tempDenominator;
		tempDenominator = tempNumerator % tempDenominator;
		tempNumerator = divisor;
	}
	tempNumerator = srcClock_Hz / divisor;
	tempDenominator = (numerator << 4) / divisor;
	baudDiff = (tempNumerator * denominator) / tempDenominator;
	baudDiff = (baudDiff >= baudRate_Bps) ? (baudDiff - baudRate_Bps) : (baudRate_Bps - baudDiff);

	//should satisfy (baudDiff < (baudRate_Bps / 100) * 3)
	UART1->UFCR &= ~UART_UFCR_RFDIV_MASK;
	UART1->UFCR |= UART_UFCR_RFDIV(refFreqDiv);
	UART1->UBIR = UART_UBIR_INC(denominator - 1);
	UART1->UBMR = UART_UBMR_MOD(numerator / divider - 1);
	UART1->ONEMS = UART_ONEMS_ONEMS(srcClock_Hz / (1000 * divider));
}

void uart1_init(void)
{
	/* Disable the UART Module */
	UART1->UCR1 &= ~UART_UCR1_UARTEN_MASK;

	/* This function resets the transmit and receive state machines, all FIFOs and register
	 * USR1, USR2, UBIR, UBMR, UBRC , URXD, UTXD and UTS[6-3]
	 */
	UART1->UCR2 &= ~UART_UCR2_SRST_MASK;
	while ((UART1->UCR2 & UART_UCR2_SRST_MASK) == 0)
	{
	}

	/* Set UART Module Register content to default value */
	UART1->UCR1 = 0x0;
	UART1->UCR2 = UART_UCR2_SRST_MASK;
	UART1->UCR3 = UART_UCR3_DSR_MASK | UART_UCR3_DCD_MASK | UART_UCR3_RI_MASK;
	UART1->UCR4 = UART_UCR4_CTSTL(32);
	UART1->UFCR = UART_UFCR_TXTL(2) | UART_UFCR_RXTL(1);
	UART1->UESC = UART_UESC_ESC_CHAR(0x2B);
	UART1->UTIM = 0x0;
	UART1->ONEMS = 0x0;
	UART1->UTS = UART_UTS_TXEMPTY_MASK | UART_UTS_RXEMPTY_MASK;
	UART1->UMCR = 0x0;

	/* Set UART data word length, stop bit count, parity mode and communication
	 * direction according to uart init struct, disable RTS hardware flow control.
	 */
	UART1->UCR2 |=
		((uint32_t)UART_UCR2_WS(1) | (uint32_t)UART_UCR2_STPB(1) |
		 (uint32_t)(((0) << UART_UCR2_PROE_SHIFT) & (UART_UCR2_PREN_MASK | UART_UCR2_PROE_MASK)) |
		 (uint32_t)UART_UCR2_TXEN(1) | (uint32_t)UART_UCR2_RXEN(1) | UART_UCR2_IRTS_MASK);

	/* For imx family device, UARTs are used in MUXED mode, so that this bit should always be set.*/
	UART1->UCR3 |= UART_UCR3_RXDMUXSEL_MASK;

	/* Disable automatic baud rate detection */
	UART1->UCR1 &= ~UART_UCR1_ADBR_MASK;

#if 0
	/* directly set baudrate to 115200 */
	//UART1->UFCR &= ~ (7 << 7);  /* clear RFDIV */
	//UART1->UFCR = 5 << 7;       /* set divide input clock by 1, uart_clk=80MHz */
	//UART1->UBIR = 0x47;//71;
	//UART1->UBMR = 0xC34;//3124;
#else
	/* Set BaudRate according to uart initialize struct. Baud Rate = Ref Freq / (16 * (UBMR + 1)/(UBIR+1)) */
	UART_SetBaudRate(115200, 80000000);
#endif

	/* Enable UART module */
	UART1->UCR1 |= UART_UCR1_UARTEN_MASK;
}
void putchar(int c)
{
	while (!(UART1->USR2 & UART_USR2_TXDC_MASK))
	{
	}
	UART1->UTXD = c & UART_UTXD_TX_DATA_MASK;
}
int getchar(void)
{
	while (!(UART1->USR2 & UART_USR2_RDR_MASK))
	{
	}

	return (unsigned char)((UART1->URXD & UART_URXD_RX_DATA_MASK) >> UART_URXD_RX_DATA_SHIFT);

}

void puts(const char *s)
{
	while (*s)
	{
		putchar(*s);
		s++;
	}
}

