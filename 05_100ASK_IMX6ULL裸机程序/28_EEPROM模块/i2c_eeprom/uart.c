#include "uart.h"

// GPS 数据发送缓存
__attribute((aligned(8))) unsigned char GPS_TX_BUF[1000];
unsigned char GPS_RX_BUF[1000];
unsigned char USART1_TX_BUF[1000];
unsigned short GPS_RX_STA = 0;


static void UART_SetBaudRate(UART_Type *base, uint32_t baudRate_Bps, uint32_t srcClock_Hz)
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
    base->UFCR &= ~UART_UFCR_RFDIV_MASK;
    base->UFCR |= UART_UFCR_RFDIV(refFreqDiv);
    base->UBIR = UART_UBIR_INC(denominator - 1);
    base->UBMR = UART_UBMR_MOD(numerator / divider - 1);
    base->ONEMS = UART_ONEMS_ONEMS(srcClock_Hz / (1000 * divider));
}

void uart_init(UART_Type *base, uint32_t  baudRate, uint32_t clk)
{
	/* Disable the UART Module */
	base->UCR1 &= ~UART_UCR1_UARTEN_MASK;

	/* This function resets the transmit and receive state machines, all FIFOs and register
	* USR1, USR2, UBIR, UBMR, UBRC , URXD, UTXD and UTS[6-3]
	*/
	base->UCR2 &= ~UART_UCR2_SRST_MASK;
	while ((base->UCR2 & UART_UCR2_SRST_MASK) == 0);

    // 配置串口
    base->UCR1 = 0;
    base->UCR1 &= ~(1<<14);

    // 配置数据位、奇偶校验、停止位等等
    base->UCR2 = 0;
    base->UCR2 |= (1 << 1) | (1 << 2) | (1 << 5) | (1 << 14);
    base->UCR3 |= (1 << 2);
    base->UCR4 |= (1 << 0);
    UART_SetBaudRate(base, baudRate, clk);
	/* Enable UART module */
	base->UCR1 |= UART_UCR1_UARTEN_MASK;
}

void putchar(int c)
{
	while (!(UART1->USR2 & UART_USR2_TXDC_MASK));
	UART1->UTXD = c & UART_UTXD_TX_DATA_MASK;
}
int getchar(void)
{
	while (!(UART1->USR2 & UART_USR2_RDR_MASK));
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

