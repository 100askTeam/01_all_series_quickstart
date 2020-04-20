#ifndef   __TIMER_H__
#define   __TIMER_H__

#include "common.h"

void gpt_init(GPT_Type *base);
void gpt_run(GPT_Type *base);
void gpt2_chan1_delay_us(u32 num_us);

#endif
