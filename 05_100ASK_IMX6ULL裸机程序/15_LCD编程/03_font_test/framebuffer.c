
#include "lcd_manager.h"

/* 实现画点 */

/* 获得LCD参数 */

static unsigned int fb_base;
static int xres, yres, bpp;
/**********************************************************************
 * 函数名称： fb_get_lcd_params
 * 功能描述： 获取lcd参数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/02/26	     V1.0	  zh(angenao)	      创建
 ***********************************************************************/
		  			 		  						  					  				 	   		  	  	 	  
void fb_get_lcd_params(void)
{
	get_lcd_params(&fb_base, &xres, &yres, &bpp);
}

/* rgb: 0x00RRGGBB */
unsigned short convert32bppto16bpp(unsigned int rgb)
{
	int r = (rgb >> 16)& 0xff;
	int g = (rgb >> 8) & 0xff;
	int b = rgb & 0xff;

	/* argb555 */
	r = r >> 3;
	g = g >> 3;
	b = b >> 3;

	return ((r<<10) | (g<<5) | (b));
}


/* color : 32bit, 0x00RRGGBB
 *
 */
 
/**********************************************************************
 * 函数名称： fb_put_pixel
 * 功能描述： 画点函数
 * 输入参数： x坐标，y坐标，颜色
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/02/26	     V1.0	  zh(angenao)	      创建
 ***********************************************************************/
void fb_put_pixel(int x, int y, unsigned int color)
{
	unsigned short *pw;  /* 16bpp */
	unsigned int   *pdw; /* 32bpp */

	unsigned int pixel_base = fb_base + (xres * bpp / 8) * y + x * bpp / 8;

	switch (bpp)
	{
		case 16:
			pw = (unsigned short *) pixel_base;
			*pw = convert32bppto16bpp(color);
			break;
		case 32:
			pdw = (unsigned int *) pixel_base;
			*pdw = color;
			break;
	}
}

