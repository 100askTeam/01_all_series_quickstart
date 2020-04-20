#ifndef   __UART_H__
#define   __UART_H__

#include "common.h"

void uart1_init();
void putchar(int c);
int getchar(void);
void puts(const char *s);

#endif
