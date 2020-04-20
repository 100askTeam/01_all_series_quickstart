#include "timer.h"
		  			 		  						  					  				 	   		  	  	 	  
void gpt_init(GPT_Type *base)
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
	base->OCR[0] = 66000000;

	/* 
	 * bit2: OF3IE Output Compare 3 Interrupt Enable
	 * bit1: OF2IE Output Compare 2 Interrupt Enable
	 * bit0: OF1IE Output Compare 1 Interrupt Enable
	 */
	/* Enable GPT Output Compare1 interrupt */
	base->IR = 1;

}
		  			 		  						  					  				 	   		  	  	 	  
void gpt_run(GPT_Type *base)
{
	/* bit0: GPT Enable */
	base->CR |= 1;
}		  			 		  						  					  				 	   		  	  	 	  

void gpt2_chan1_delay_us(u32 num_us)
{
	u32 ocr1 = 66 * num_us;

	/* bit15 SWR, Software reset*/
	GPT2->CR |= (1 << 15);
	/* Wait reset finished. */
	while((GPT2->CR >> 15) & 0x1) {
	}

	/*
	 *bit10: Enable 24 MHz clock input from crystal
	 *bit9: 0 restart mode, 1 free-run mode:set 0
	 *bit8-6: Clock Source select :001 Peripheral Clock (ipg_clk)
	 *bit5: GPT Stop Mode enable
	 *bit3: GPT Wait Mode enable.
	 *bit1: GPT Enable Mode
	 */	
	GPT2->CR = (1 << 6) | (1 << 5) | (1 << 3) | (1 << 1);

	/*
	 *bit15-bit12:PRESCALER24M
	 *bit11-0:PRESCALER
	 */
	GPT2->PR = 0;

	/* GPTx_OCR1  bit31-0: Compare Value
	 * When the counter value equals the COMP bit field value, 
	 * a compare event is generated on Output Compare Channel 1.
	 */
	GPT2->OCR[0] = ocr1;

	/* 
	 * bit2: OF3IE Output Compare 3 Interrupt Enable
	 * bit1: OF2IE Output Compare 2 Interrupt Enable
	 * bit0: OF1IE Output Compare 1 Interrupt Enable
	 */
	/* Enable GPT Output Compare1 interrupt */
	GPT2->IR = 1;

	/* bit0: GPT Enable */
	GPT2->CR |= 1;

	while(!(GPT2->SR & 0x1)) {
	}

	/* bit0: 0 GPT disable */
	GPT2->CR &= ~1;
	GPT2->SR = 0x1;
}



void timer1_init(unsigned int frac, unsigned int value)
{
	frac = frac & 0XFFF;
	EPIT1->CR = 0;	/* 先清零CR寄存器 */
	EPIT1->CR = (1<<24 | frac << 4 | 1<<3 | 1<<2 | 1<<1);
	EPIT1->LR = value;	/* 阈值 */
	EPIT1->CMPR	= 0;
	// EPIT1->CR |= 1<<0;	/* 使能EPIT1 */
}
