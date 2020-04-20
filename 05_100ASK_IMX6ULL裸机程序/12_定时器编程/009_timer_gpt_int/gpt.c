#include "gpt.h"

/* assume use ipc clk which is 66MHz, 1us against to 66 count */
#define USEC_TO_COUNT(us) (us * 66 - 1)

void gpt_init(GPT_Type *base, enum gpt_comp_channel chan, int us)
{
	/* bit15 SWR, Software reset*/
	base->CR |= (1 << 15);
	/* Wait reset finished. */
	while((base->CR >> 15) & 0x1) {
	}
		  			 		  						  					  				 	   		  	  	 	  
	/*
	 *bit10: Enable 24 MHz clock input from crystal
	 *bit9: 0 restart mode, 1 free-run mode:set 0
	 *bit8-6: Clock Source select :001 Peripheral Clock (ipg_clk)
	 *bit5: GPT Stop Mode enable
	 *bit3: GPT Wait Mode enable.
	 *bit1: GPT Enable Mode
	 */	
	base->CR = (1 << 6) | (1 << 5) | (1 << 3) | (1 << 1);

	/*
	 *bit15-bit12:PRESCALER24M
	 *bit11-0:PRESCALER
	 */
	base->PR = 0;

	/* GPTx_OCR1  bit31-0: Compare Value
	 * When the counter value equals the COMP bit field value, 
	 * a compare event is generated on Output Compare Channel 1.
	 */
	base->OCR[chan] = USEC_TO_COUNT(us);
}
		  			 		  						  					  				 	   		  	  	 	  
void gpt_run(GPT_Type *base, int on)
{
	/* bit0: GPT Enable */
	if (on)
		base->CR |= (1 << 0);
	else
		base->CR &= ~(1 << 0);
}

/* 
 * bit5: Rollover Interrupt Enable
 * bit4: IF2 Input capture Interrupt Enable
 * bit3: IF1IE Input capture Interrupt Enable
 * bit2: OF3IE Output Compare 3 Interrupt Enable
 * bit1: OF2IE Output Compare 2 Interrupt Enable
 * bit0: OF1IE Output Compare 1 Interrupt Enable
 */
void gpt_enable_interrupt(GPT_Type *base, enum gpt_interrupt_bit bit, int on)
{
	if (on)
		base->IR |= (1 << bit);
	else
		base->IR &= ~(1 << bit);
}

void gpt_clear_int_status(GPT_Type *base, enum gpt_interrupt_bit bit)
{
		base->SR |= (1 << bit);
}

