
#include "lcd_manager.h"

#define LCD_FB_BASE   0x99000000 /*显存地址*/

lcd_params lcd_7_0_params = {
	.name = "lcd_7.0",
	.pins_pol = {
		.de    = INVERT,	/* normal: 低电平表示使能输出 */
		.vclk  = INVERT,	/* normal: 在上升降沿获取数据*/
		.hsync = NORMAL,    /* normal: 低脉冲*/
		.vsync = NORMAL, 	/* normal: 低脉冲*/
	},
	.time_seq = {
		/* 垂直方向 */
		.tvp=	3, /* vysnc脉冲宽度 */
		.tvb=	20,  /* 上边黑框, Vertical Back porch */
		.tvf=	12,  /* 下边黑框, Vertical Front porch */

		/* 水平方向 */
		.thp=	20, /* hsync脉冲宽度 */
		.thb=	140,  /* 左边黑框, Horizontal Back porch */
		.thf=	160,  /* 右边黑框, Horizontal Front porch */
		  			 		  						  					  				 	   		  	  	 	  
		.vclk=	51.2,  /* MHz */
	},
	.xres = 1024,
	.yres = 600,
	.bpp  = 16,       
	.fb_base = LCD_FB_BASE,
};

/**********************************************************************
 * 函数名称： lcd_7_0_add
 * 功能描述： 添加7寸屏幕的参数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/02/26	     V1.0	  zh(angenao)	      创建
 ***********************************************************************/
void lcd_7_0_add(void)
{
	register_lcd(&lcd_7_0_params);
}


