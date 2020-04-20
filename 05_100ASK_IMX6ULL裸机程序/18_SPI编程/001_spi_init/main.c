#include "common.h"
#include "uart.h"
#include "my_printf.h"
#include "spi.h"
void delay(volatile unsigned int d)
{
	while(d--);
}
void system_init()
{
	
	boot_clk_gate_init();
	boot_clk_init();
	uart1_init();
	puts("init ok\r\n");
	
}
int  main()
{	
	unsigned char uc_cnt;
	spi_init(ESCPI3_BASE);
	while(1)
	{
		spi_writeread(ESCPI3_BASE,0x55);
		delay(1000);
	}
	return 0;
}



