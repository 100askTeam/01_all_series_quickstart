
#ifndef _MY_PRINTF_H
#define _MY_PRINTF_H

#include "uart.h"
#define  __out_putchar  putchar


#define  MAX_NUMBER_BYTES  64

extern int my_printf_test(void);
int printf(const char *fmt, ...);

#endif /* _MY_PRINTF_H */

