#include  "my_printf.h"
#include  "uart.h"
		  			 		  						  					  				 	   		  	  	 	  

//==================================================================================================
typedef char *  va_list;
#define _INTSIZEOF(n)   ( (sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1) )

#define va_start(ap,v)  ( ap = (va_list)&v + _INTSIZEOF(v) )
//#define va_arg(ap,t)    ( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )
#define va_arg(ap,t)    ( *(t *)( ap=ap + _INTSIZEOF(t), ap- _INTSIZEOF(t)) )
#define va_end(ap)      ( ap = (va_list)0 )

//==================================================================================================


unsigned char hex_tab[]={'0','1','2','3','4','5','6','7',\
		                 '8','9','a','b','c','d','e','f'};
		  			 		  						  					  				 	   		  	  	 	  

static int outc(int c) 
{
	PutChar(c);
	return 0;
}

static int outs (const char *s)
{
	while (*s != '\0')	
		PutChar(*s++);
	return 0;
}

static int out_num(long n, int base,char lead,int maxwidth) 
{
	unsigned long m=0;
	char buf[MAX_NUMBER_BYTES], *s = buf + sizeof(buf);
	int count=0,i=0;
			

	*--s = '\0';
	
	if (n < 0){
		m = -n;
	}
	else{
		m = n;
	}
	
	do{
		*--s = hex_tab[m%base];
		count++;
	}while ((m /= base) != 0);
	
	if( maxwidth && count < maxwidth){
		for (i=maxwidth - count; i; i--)	
			*--s = lead;
}

	if (n < 0)
		*--s = '-';
	
	return outs(s);
}
   

/*reference :   int vprintf(const char *format, va_list ap); */
static int my_vprintf(const char *fmt, va_list ap) 
{
	char lead=' ';
	int  maxwidth=0;
	
	 for(; *fmt != '\0'; fmt++)		
	 {
		if (*fmt != '%') {			
			outc(*fmt);				
			continue;
		}

		 lead=' ';
		 maxwidth=0;
		 
		//format : %08d, %8d,%d,%u,%x,%f,%c,%s 
		    fmt++;
		if(*fmt == '0'){
			lead = '0';
			fmt++;	
		}

		
		while(*fmt >= '0' && *fmt <= '9'){
			maxwidth *=10;
			maxwidth += (*fmt - '0');
			fmt++;
		}
		
		switch (*fmt) {
		case 'd': out_num(va_arg(ap, int),          10,lead,maxwidth); break;
		case 'o': out_num(va_arg(ap, unsigned int),  8,lead,maxwidth); break;				
		case 'u': out_num(va_arg(ap, unsigned int), 10,lead,maxwidth); break;
		case 'x': out_num(va_arg(ap, unsigned int), 16,lead,maxwidth); break;
		case 'c': outc(va_arg(ap, int   )); break;		
		case 's': outs(va_arg(ap, char *)); break;		  		
				
		default:  
			outc(*fmt);
			break;
		}
	}
	return 0;
}
/**********************************************************************
 * 函数名称： printf
 * 功能描述： 自定义串口格式化函数，实现printf打印功能
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 返回0，表示函数正常返回
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/02/16	     V1.0	  zh(angenao)	      创建
 ***********************************************************************/


//reference :  int printf(const char *format, ...); 
int printf(const char *fmt, ...) 
{
	va_list ap;

	va_start(ap, fmt);
	my_vprintf(fmt, ap);	
	va_end(ap);
	return 0;
}

/**********************************************************************
 * 函数名称： my_printf_test
 * 功能描述： printf测试函数，测试自定义printf函数打印是否正常
 * 输入参数： 无
 * 输出参数：无
 * 返 回 值：  返回0，表示函数正常返回
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/02/16	     V1.0	  zh(angenao)	      创建
 ***********************************************************************/

int my_printf_test(void)
{
	printf("This is www.100ask.org   my_printf test\n\r") ;	
	printf("test char           =%c,%c\n\r", 'A','a') ;	
	printf("test decimal number =%d\n\r",    123456) ;
	printf("test decimal number =%d\n\r",    -123456) ;	
	printf("test hex     number =0x%x\n\r",  0x55aa55aa) ;	
	printf("test string         =%s\n\r",    "www.100ask.org") ;	
	printf("num=%08d\n\r",   12345);
	printf("num=%8d\n\r",    12345);
	printf("num=0x%08x\n\r", 0x12345);
	printf("num=0x%8x\n\r",  0x12345);
	printf("num=0x%02x\n\r", 0x1);
	printf("num=0x%2x\n\r",  0x1);

	printf("num=%05d\n\r", 0x1);
	printf("num=%5d\n\r",  0x1);

	return 0;
}



