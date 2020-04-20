#ifndef   __TIMER_H__
#define   __TIMER_H__

#include "common.h"

void gpt_init(GPT_Type *base);
void gpt_run(GPT_Type *base);
void gpt2_chan1_delay_us(u32 num_us);


void delay_us(unsigned int n);
void delay_ms(unsigned int n);

void gpt2_start(void);
void gpt2_stop(void);

#endif
