#include "timer.h"
#include "led.h"
#include "dht11.h"
#include "my_printf.h"
#include "uart.h"

void delay(volatile unsigned int d)
{
	while(d--);
}


int  main()
{
	Uart_Init();
	while(1)
	{
		dht11_data_read();				//读取dht11数据
		gpt2_chan1_delay_us(2000000);	//延时2s
	}
					
	return 0;
}


