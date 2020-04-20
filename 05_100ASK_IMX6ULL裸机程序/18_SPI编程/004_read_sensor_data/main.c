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
	icm20608g_init();//初始化传感器ICM-20608-G	
	for(uc_cnt=0;uc_cnt<1;uc_cnt++)
	{
		icm20608g_read_acc();
		icm20608g_read_gyro();
		icm20608g_read_temp();
	}
	return 0;
}



