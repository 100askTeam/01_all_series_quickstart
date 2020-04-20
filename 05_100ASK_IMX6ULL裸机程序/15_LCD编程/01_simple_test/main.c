#include "lcd_manager.h"

void lcd_test(void)
{
	unsigned int fb_base ;
	int xres, yres, bpp;
	int x, y;
	unsigned short *p;
	unsigned int *p2;
	
	/* 使能LCD */
	lcd_enable();

	/* 获得LCD的参数: fb_base, xres, yres, bpp */
	get_lcd_params(&fb_base, &xres, &yres, &bpp);

	/* 往framebuffer中写数据 */
	if (bpp == 16)
	{
		/* 让LCD输出整屏的红色 */

		/* ARGB555 */
		/*red*/
		p = (unsigned short *)fb_base;
		for (x = 0; x < xres; x++)
			for (y = 0; y < yres; y++)
				*p++ = 0x7c00;

		/* green */
		p = (unsigned short *)fb_base;
		for (x = 0; x < xres; x++)
			for (y = 0; y < yres; y++)
				*p++ = 0x3E0;

		/* blue */
		p = (unsigned short *)fb_base;
		for (x = 0; x < xres; x++)
			for (y = 0; y < yres; y++)
				*p++ = 0x1f;
			
	}
	else if (bpp == 32)
	{
		/* 让LCD输出整屏的红色 */

		/* ARGB888 */
		/*red*/
		p2 = (unsigned int *)fb_base;
		for (x = 0; x < xres; x++)
			for (y = 0; y < yres; y++)
				*p2++ = 0xff0000;

		/*green*/ 
		p2 = (unsigned int *)fb_base;
		for (x = 0; x < xres; x++)
			for (y = 0; y < yres; y++)
				*p2++ = 0x00ff00;

		/*blue*/ 
		p2 = (unsigned int *)fb_base;
		for (x = 0; x < xres; x++)
			for (y = 0; y < yres; y++)
				*p2++ = 0x0000ff;

	}
}



int main(void)
{
	/*初始化LCD相关*/
	lcd_init();

	/*LCD测试*/		
	lcd_test();

}

