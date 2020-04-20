#include "uart.h"
#include "spi.h"
#include "my_printf.h"
#include "tlc5615.h"

void delay_x(unsigned short uc_x)
{
	unsigned short x,y;
	for(x=0;x<uc_x;x++)
	{
		for(y=0;y<1000;y++);
	}
}

int  main()
{	
	unsigned short uc_cnt=0;
	unsigned short key_read_x=0;
	unsigned short key_read_his=0;
    Uart_Init();
    tlc5615_init();//初始化TLC5615
	while(1)
	{
		for(uc_cnt=0;uc_cnt<1024;uc_cnt+=50)
		{
			printf("write data %d\n\r",uc_cnt);
			tlc5615_write_addr(uc_cnt<<2);		
			delay_x(1000);
		}
	}
	return 0;
}


