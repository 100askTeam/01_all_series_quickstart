#ifndef   __GIC_H__
#define   __GIC_H__

#include "common.h"

void gic_init(void);
void gic_enable_irq(IRQn_Type nr);
void gic_disable_irq(IRQn_Type nr);


#endif

