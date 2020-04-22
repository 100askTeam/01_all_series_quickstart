#ifndef   __EPIT_H__
#define   __EPIT_H__

#include "common.h"

void epit_init(EPIT_Type *base, unsigned int us);
void epit_enable_interrupt(EPIT_Type *base, int on);
void epit_run(EPIT_Type *base, int on);

void epit_poll_init(EPIT_Type *base);
void epit_poll_restart(EPIT_Type *base, unsigned int us);

#endif

