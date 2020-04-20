#ifndef   __UART_H__
#define   __UART_H__

#include "common.h"


// GPS 数据发送缓存 	
extern unsigned char GPS_TX_BUF[1000];
extern unsigned char GPS_RX_BUF[1000];
extern unsigned char USART1_TX_BUF[1000];
extern unsigned short GPS_RX_STA;



void uart_init();
void putchar(int c);
int getchar(void);
void puts(const char *s);

#endif
