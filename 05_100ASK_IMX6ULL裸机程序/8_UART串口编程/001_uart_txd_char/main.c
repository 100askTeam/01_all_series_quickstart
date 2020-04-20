#include "uart.h"

int  main()
{
	unsigned char cTestData = 'A'; /*用于测试发送的数据*/
 	Uart_Init()	 ;

	while(1)
	{	
		PutChar(cTestData);
	}
					
	return 0;
}


