#include "uart.h"

int  main()
{
	unsigned char cTestData ;       /*用于测试发送的数据*/
 	Uart_Init()	 ;

	while(1)
	{	
		cTestData = GetChar() ;				/*等待从串口获取数据*/
		
			if (cTestData == '\r')  		/*添加回车换行\n\r*/
			{ 
				PutChar('\n');
			}

			if (cTestData == '\n')
			{
				PutChar('\r');
			}
				
		PutChar(cTestData)    ;				/*从串口发送数据*/
	}
					
	return 0;
}
		  			 		  						  					  				 	   		  	  	 	  