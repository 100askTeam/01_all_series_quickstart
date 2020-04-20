#include "uart.h"
#include <string.h>
extern unsigned char flag_rec_ok;//从外部读取，表示读取到一帧GPS信息
extern  unsigned char buf[128];//缓存有当前读取到的GPS帧信息
int  main()
{
	unsigned char cTestDataX =0;		/*用于记录读取到的数据长度*/

 	Uart_Init(UART1,1)	 ;//初始化串口1，设置波特率为115200

	PutStr(UART1,"Hello, world!\n\r");	/*发送字符串*/
	Uart_Init(UART6,0)	 ;//初始化串口6，设置波特率为9600
	while(1)
	{
		GetStr(UART6,&cTestDataX);
		if(flag_rec_ok==1)
		{
			flag_rec_ok = 0;
			if(buf[4]=='G')//表示当前读取到的是GPGGA的相关内容，进行解析。如果需要其他GPGSA、GPGSV等语句的信息，按照相应格式进行提取即可
			{
		//		PutStr(UART1,buf);//打印得到的原始数据
			//	anglysysrawdata("$GPGGA,082559.00,4005.22599,N,11632.58234,E,1,04,3.08,14.6,M,-5.6,M,,*76");//用于测试固定内容的解析，
				anglysysrawdata(buf);	//进行实际数据内容的解析
			}				
		}
	}
					
	return 0;
}
		  			 		  						  					  				 	   		  	  	 	  

