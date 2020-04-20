
/**********************************************************************
 * 函数名称： copy_data
 * 功能描述： 将整个程序(.text, .rodata, .data)从DDR3重定位到片内RAM
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号        修改人        修改内容
 * -------------------------------------------------
 * 2020/02/20	    V1.0         阿和            创建
 ***********************************************************************/
void copy_data (void)
{
	/* 从链接脚本中获得参数 _start, __bss_start, */
	extern int _load_addr, _start, __bss_start;

	volatile unsigned int *dest = (volatile unsigned int *)&_start;			//_start = 0x900000
	volatile unsigned int *end = (volatile unsigned int *)&__bss_start;		//__bss_start = 0x9xxxxx
	volatile unsigned int *src = (volatile unsigned int *)&_load_addr;		//_load_addr = 0x80100000

	/* 重定位数据 */	
	while (dest < end)
	{
		*dest++ = *src++;
	}
}
		  			 		  						  					  				 	   		  	  	 	  
/**********************************************************************
 * 函数名称： clean_bss
 * 功能描述： 清除.bss段
 * 输入参数： start, end
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号        修改人        修改内容
 * -------------------------------------------------
 * 2020/02/20	    V1.0         阿和            创建
 ***********************************************************************/
void clean_bss(void)
{
	/* 从lds文件中获得 __bss_start, __bss_end */
	extern int __bss_end, __bss_start;

	volatile unsigned int *start = (volatile unsigned int *)&__bss_start;
	volatile unsigned int *end = (volatile unsigned int *)&__bss_end;

	while (start <= end)
	{
		*start++ = 0;
	}
}
		  			 		  						  					  				 	   		  	  	 	  
