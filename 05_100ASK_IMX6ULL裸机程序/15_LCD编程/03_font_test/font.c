#include "lcd_manager.h"
#include "framebuffer.h"

extern const unsigned char fontdata_8x16[];

static unsigned int fb_base;
static int xres, yres, bpp;
		  			 		  						  					  				 	   		  	  	 	  
/**********************************************************************
 * 函数名称： font_init
 * 功能描述： 获取LCD参数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/02/26	     V1.0	  zh(angenao)	      创建
 ***********************************************************************/                

void font_init(void)
{
	get_lcd_params(&fb_base, &xres, &yres, &bpp);
}
/**********************************************************************
 * 函数名称： fb_print_char
 * 功能描述： 在lcd上打印单个字符
 * 输入参数： x坐标，y坐标，字符，颜色
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/02/26	     V1.0	  zh(angenao)	      创建
 ***********************************************************************/                

void fb_print_char(int x, int y, char c, unsigned int color)
{
	int i, j;
	
	/* 根据c的ascii码在fontdata_8x16中得到点阵数据 */
	const unsigned char *dots = &fontdata_8x16[c * 16];

	unsigned char data;
	int bit;

	/* 根据点阵来设置对应象素的颜色 */
	for (j = y; j < y+16; j++)
	{
		data = *dots++;
		bit = 7;
		for (i = x; i < x+8; i++)
		{
			/* 根据点阵的某位决定是否描颜色 */
			if (data & (1<<bit))
				fb_put_pixel(i, j, color);
			bit--;
		}
	}
}

/**********************************************************************
 * 函数名称： fb_print_string
 * 功能描述： 在lcd上打印字符串
 * 输入参数： x坐标，y坐标，字符串，颜色
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/02/26	     V1.0	  zh(angenao)	      创建
 ***********************************************************************/                

void fb_print_string(int x, int y, char* str, unsigned int color)
{
	int i = 0, j;
	
	while (str[i])
	{
		if (str[i] == '\n')
			y = y+16;
		else if (str[i] == '\r')
			x = 0;
		  			 		  						  					  				 	   		  	  	 	  
		else
		{
			fb_print_char(x, y, str[i], color);
			x = x+8;
			if (x >= xres) /* 换行 */
			{
				x = 0;
				y = y+16;
			}
		}
		i++;
	}
}


