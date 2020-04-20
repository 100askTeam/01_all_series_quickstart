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
	spi_test_rw(ESCPI3_BASE);
	return 0;
}



