#ifndef   __GIC_H__
#define   __GIC_H__

#include "common.h"

extern unsigned char write_state;
extern unsigned char read_state;

void gic_init(void);
void gic_enable_irq(IRQn_Type nr);
void gic_disable_irq(IRQn_Type nr);


#endif

