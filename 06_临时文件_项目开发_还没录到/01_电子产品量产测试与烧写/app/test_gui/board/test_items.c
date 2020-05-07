#include <config.h>
#include <test_items.h>
#include <stdio.h>
#include <string.h>

static T_ItemCfg g_tItemsCfg[100];
static int g_iItemsNum = 0;

/**********************************************************************
 * 函数名称： ParseConfig
 * 功能描述： 解析配置文件/etc/test_gui/gui.conf
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0: 成功 , -1: 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/03/16	     V1.0	  韦东山	      创建
 ***********************************************************************/
int  ParseConfig(void)
{
	/* gui.conf格式
	 * # name   can_be_pressed  command
	 *   led      1
	 *   button1  0
	 *   usb_otg  0
     */
	FILE *tFp;
	char line[1024];
	char *p;
	int i = 0;

	/* 打开文件 */
	tFp = fopen(CFG_FILE, "r+");
	if (tFp == NULL)
	{
		DBG_PRINTF("can't open %s\n", CFG_FILE);
		return -1;
	}

	while (fgets(line, 1024, tFp))
	{
		/* 1. 吃掉开头的空格 */
		p = line;
		while (*p == ' ' || *p == '\t')p++;

		/* 2. 忽略注释 */		
		if (*p == '#')
			continue;

		g_tItemsCfg[i].command[0] = '\0';
		g_tItemsCfg[i].index = i;
		sscanf(p, "%s %d %s", g_tItemsCfg[i].name, &g_tItemsCfg[i].can_be_pressed, g_tItemsCfg[i].command);
		i++;
	}

	g_iItemsNum = i;
	return 0;
}

/**********************************************************************
 * 函数名称： GetConfigNumber
 * 功能描述： 返回配置项个数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 个数
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/03/16	     V1.0	  韦东山	      创建
 ***********************************************************************/
int  GetConfigNumber(void)
{
	return g_iItemsNum;
}


/**********************************************************************
 * 函数名称： GetConfigByIndex
 * 功能描述： 返回指定配置项
 * 输入参数： index - 哪个配置项
 * 输出参数： 无
 * 返 回 值： PT_ItemCfg - 配置项指针, 若失败则返回NULL
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/03/16	     V1.0	  韦东山	      创建
 ***********************************************************************/
PT_ItemCfg  GetConfigByIndex(int index)
{
	if (index > g_iItemsNum)
		return NULL;
	else
		return &g_tItemsCfg[index];
}

/**********************************************************************
 * 函数名称： GetConfigByName
 * 功能描述： 根据名字返回指定配置项
 * 输入参数： name - 名字
 * 输出参数： 无
 * 返 回 值： PT_ItemCfg - 配置项指针, 若失败则返回NULL
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/03/16	     V1.0	  韦东山	      创建
 ***********************************************************************/
PT_ItemCfg  GetConfigByName(char *name)
{
	int i;
	
	for (i = 0; i < g_iItemsNum; i++)
		if (!strcmp(g_tItemsCfg[i].name, name))
			return &g_tItemsCfg[i];
	return NULL;
}

/**********************************************************************
 * 函数名称： SetProcessForItem
 * 功能描述： 给指定配置项设置处理函数
 * 输入参数： name - 配置项名字, Process - 处理函数
 * 输出参数： 无
 * 返 回 值： 0: 成功 , -1: 失败
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/03/16	     V1.0	  韦东山	      创建
 ***********************************************************************/
int  SetProcessForItem(char *name, void (*Process)(char *))
{
	PT_ItemCfg ptItemCfg;
	
	ptItemCfg = GetConfigByName(name);
	if (!ptItemCfg)
		return -1;
	else
		ptItemCfg->Process = Process;
	return 0;
}

const char *GetItemStatus(E_TestStatus status)
{
	static const char *status_str[] = {"err", "ok", "processing", "percent"};
	int i = (int)status;
	if (i >=0 && i < sizeof(status_str) / sizeof(status_str[0]))
		return status_str[i];
	else
		return NULL;
}


