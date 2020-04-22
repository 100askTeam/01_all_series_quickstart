#include "epit.h"

/* assume use ipc clk which is 66MHz, 1us against to 66 count */
#define USEC_TO_COUNT(us) (us * 66)

void epit_init(EPIT_Type *base, unsigned int us)
{
	base->CR = 0;

	/* software reset  
	 * bit16
	 */
	base->CR |= (1 << 16);
	/* wait for software reset self clear*/
	while((base->CR) & (1 << 16))
		;

	/*
	 * EPIT_CR
	 * bit21 stopen; bit19 waiten; bit18 debugen
	 * bit17 overwrite enable; bit3 reload
	 * bit2 compare interrupt enable; bit1 enable mode
	 */
	base->CR |= (1 << 21) | (1 << 19) | (1 << 3) | (1 << 1);

	/*
	 * EPIT_CR
	 * bit25-24: 00 off, 01 peripheral clock(ipg clk), 10 high, 11 low
	 * bit15-4: prescaler value, divide by n+1
	 */
	base->CR &= ~((0x3 << 24) | (0xFFF << 4));
	base->CR |= (1 << 24);

	/* EPIT_CMPR: compare register */
	base->CMPR = 0;
	/* EPIT_LR: load register , assue use ipc clk 66MHz*/
	base->LR = USEC_TO_COUNT(us);
}

void epit_enable_interrupt(EPIT_Type *base, int on)
{
	/*  EPIT_CR bit2 OCIEN compare interrupt enable */
	if (on)
		base->CR |= (1 << 2);
	else
		base->CR &= ~(1 << 2);
}

void epit_run(EPIT_Type *base, int on)
{
	/*  EPIT_CR bit0 EN */
	if (on)
		base->CR |= (1 << 0);
	else
		base->CR &= ~(1 << 0);
}


void epit_poll_init(EPIT_Type *base)
{
	base->CR = 0;

	/* software reset  
	 * bit16
	 */
	base->CR |= (1 << 16);
	/* wait for software reset self clear*/
	while((base->CR) & (1 << 16))
		;

	/*
	 * EPIT_CR
	 * bit21 stopen; bit19 waiten; bit18 debugen
	 * bit17 overwrite enable; bit3 reload
	 * bit2 compare interrupt enable; bit1 enable mode
	 */
	base->CR |= (1 << 21) | (1 << 19) | (1 << 17) | (1 << 3) | (1 << 1);

	/*
	 * EPIT_CR
	 * bit25-24: 00 off, 01 peripheral clock(ipg clk), 10 high, 11 low
	 * bit15-4: prescaler value, divide by n+1
	 */
	base->CR &= ~((0x3 << 24) | (0xFFF << 4));
	base->CR |= (1 << 24);

	/* EPIT_CMPR: compare register */
	base->CMPR = 0;
	/* EPIT_LR: load register , assue use ipc clk 66MHz*/
	//base->LR = USEC_TO_COUNT(us);

	/*	EPIT_CR bit2 OCIEN compare interrupt enable */
	base->CR |= (1 << 2);
}

void epit_poll_restart(EPIT_Type *base, unsigned int us)
{
	epit_run(base, 0);
	/* EPIT_LR: load register , assue use ipc clk 66MHz*/
	base->LR = USEC_TO_COUNT(us);
	/* write 1 clear it, avoid it happened before */
	EPIT1->SR |= (1 << 0);
	epit_run(base, 1);
	/* wait compare event happened*/
	while(!(EPIT1->SR & 0x1))
		;
	/* write 1 clear it */
	EPIT1->SR |= (1 << 0);
}


