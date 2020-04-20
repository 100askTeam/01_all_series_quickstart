#include "timer.h"
#include "ds18b20.h"
#include "my_printf.h"
#include "uart.h"

unsigned int temp_negative_flag;

int  main()
{	
	Uart_Init();
	while(1)
	{
		if(ds18b20_data_read() != 0)
			printf("ds18b20 read failed!\n\r");
		gpt2_chan1_delay_us(2000000);
	}
					
	return 0;
}


