#ifndef   __GPT_H__
#define   __GPT_H__

#include "common.h"
		  			 		  						  					  				 	   		  	  	 	  
typedef enum gpt_interrupt_bit {
	IR_OF1IE,
	IR_OF2IE,
	IR_OF3IE,
	IR_IF1IE,
	IR_IF2IE,
	IR_ROVIE,
} gpt_interrupt_bit_t;

typedef enum gpt_comp_channel {
	OUT_COMP1,
	OUT_COMP2,
	OUT_COMP3,
} gpt_comp_channel_t;

typedef enum gpt_cap_channel {
	IN_CAP1,
	IN_CAP2,
}gpt_cap_channel_t;

void gpt_init(GPT_Type *base, enum gpt_comp_channel chan, int us);
void gpt_run(GPT_Type *base, int on);
void gpt_enable_interrupt(GPT_Type *base, enum gpt_interrupt_bit bit, int on);


#endif

