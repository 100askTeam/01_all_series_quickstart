#include "lcd_controller_manager.h"
#include "lcd_manager.h"
#include "imx6ull_con.h"

#define LCD_CONTROLLER_NUM 10

static p_lcd_controller p_array_lcd_controller[LCD_CONTROLLER_NUM];
static p_lcd_controller g_p_lcd_controller_selected;
/**********************************************************************
 * 函数名称： strcmp
 * 功能描述： 比较字符串函数
 * 输入参数： 1:字符串，2:字符串
 * 输出参数： 无
 * 返 回 值： 相同返回0
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/02/26	     V1.0	  zh(angenao)	      创建
 ***********************************************************************/

int strcmp(const char * cs,const char * ct)
{
	register signed char __res;

	while (1) {
		if ((__res = *cs - *ct++) != 0 || !*cs++)
			break;
	}

	return __res;
}
/**********************************************************************
 * 函数名称： register_lcd_controller
 * 功能描述： 注册LCD控制器结构体，提供给上层调用
 * 输入参数： LCD控制器结构体
 * 输出参数： 无
 * 返 回 值： 正常返回0
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/02/26	     V1.0	  zh(angenao)	      创建
 ***********************************************************************/

int register_lcd_controller(p_lcd_controller plcdcon)
{
	int i;
	for (i = 0; i < LCD_CONTROLLER_NUM; i++)
	{
		if (!p_array_lcd_controller[i])
		{
			p_array_lcd_controller[i] = plcdcon;
			return i;
		}
	}
	return -1;		
}
/**********************************************************************
 * 函数名称： select_lcd_controller
 * 功能描述： 选中指定LCD控制器结构体，提供给上层调用
 * 输入参数： lcd控制器的名称
 * 输出参数： 无
 * 返 回 值： 正常返回0
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/02/26	     V1.0	  zh(angenao)	      创建
 ***********************************************************************/

int select_lcd_controller(char *name)
{
	int i;
	for (i = 0; i < LCD_CONTROLLER_NUM; i++)
	{
		if (p_array_lcd_controller[i] && !strcmp(p_array_lcd_controller[i]->name, name))
		{
			g_p_lcd_controller_selected = p_array_lcd_controller[i];
			return i;
		}
	}
	return -1;		
}
/**********************************************************************
 * 函数名称： lcd_controller_init
 * 功能描述： 调用下层的LCD控制器初始化函数，提供给上层使用
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/02/26	     V1.0	  zh(angenao)	      创建
 ***********************************************************************/

int lcd_controller_init(p_lcd_params plcdparams)
{
	if (g_p_lcd_controller_selected)
	{
		g_p_lcd_controller_selected->init(plcdparams);
		return 0;
	}
	return -1;
}
/**********************************************************************
 * 函数名称： lcd_controller_enable
 * 功能描述： 调用下层的LCD控制器使能函数，提供给上层使用
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/02/26	     V1.0	  zh(angenao)	      创建
 ***********************************************************************/

void lcd_controller_enable(void)
{
	if (g_p_lcd_controller_selected)
	{
		g_p_lcd_controller_selected->enable();
	}
}
/**********************************************************************
 * 函数名称： lcd_controller_disable
 * 功能描述： 调用下层的LCD控制器失能函数，提供给上层使用
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/02/26	     V1.0	  zh(angenao)	      创建
 ***********************************************************************/

void lcd_controller_disable(void)
{
	if (g_p_lcd_controller_selected)
	{
		g_p_lcd_controller_selected->disable();
	}
}
/**********************************************************************
 * 函数名称： lcd_contoller_add
 * 功能描述： 添加lcd控制器结构体，提供给上层使用
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/02/26	     V1.0	  zh(angenao)	      创建
 ***********************************************************************/

void lcd_contoller_add(void)
{
	Imx6ull_lcd_controller_add();
}

