#include <config.h>
#include <debug_manager.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

static PT_DebugOpr g_ptDebugOprHead;
static int g_iDbgLevelLimit = 8;

/**********************************************************************
 * 函数名称： RegisterDispOpr
 * 功能描述： 注册"调试通道", 把PT_DebugOpr结构体放入链表中
 * 输入参数： ptDebugOpr - 一个结构体,表示调试通道
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int RegisterDebugOpr(PT_DebugOpr ptDebugOpr)
{
	PT_DebugOpr ptTmp;

	if (!g_ptDebugOprHead)
	{
		g_ptDebugOprHead   = ptDebugOpr;
		ptDebugOpr->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptDebugOprHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = ptDebugOpr;
		ptDebugOpr->ptNext = NULL;
	}

	return 0;
}

/**********************************************************************
 * 函数名称： ShowDebugOpr
 * 功能描述： 显示本程序能支持的"调试模块"
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
void ShowDebugOpr(void)
{
	int i = 0;
	PT_DebugOpr ptTmp = g_ptDebugOprHead;

	while (ptTmp)
	{
		DBG_PRINTF("%02d %s\n", i++, ptTmp->name);
		ptTmp = ptTmp->ptNext;
	}
}

/**********************************************************************
 * 函数名称： GetDebugOpr
 * 功能描述： 根据名字取出指定的"调试模块"
 * 输入参数： pcName - 名字
 * 输出参数： 无
 * 返 回 值： NULL   - 失败,没有指定的模块, 
 *            非NULL - 显示模块的PT_DebugOpr结构体指针
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
PT_DebugOpr GetDebugOpr(char *pcName)
{
	PT_DebugOpr ptTmp = g_ptDebugOprHead;
	
	while (ptTmp)
	{
		if (strcmp(ptTmp->name, pcName) == 0)
		{
			return ptTmp;
		}
		ptTmp = ptTmp->ptNext;
	}
	return NULL;
}


/**********************************************************************
 * 函数名称： SetDbgLevel
 * 功能描述： 设置打印级别g_iDbgLevelLimit: 级别范围0~7, 数字越小级别越高
 *            高于或等于g_iDbgLevelLimit的调试信息才会打印出来
 * 输入参数： strBuf - 类似"dbglevel=3"
 * 输出参数： 无
 * 返 回 值： 0   - 成功
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int SetDbgLevel(char *strBuf)
{
	g_iDbgLevelLimit = strBuf[9] - '0';
	return 0;
}

/**********************************************************************
 * 函数名称： SetDbgChanel
 * 功能描述： 开/关某个调试通道
 * 输入参数： strBuf - 类似于以下字符串
 *                     stdout=0			   : 关闭stdout打印
 *                     stdout=1			   : 打开stdout打印
 *                     netprint=0		   : 关闭netprint打印
 *                     netprint=1		   : 打开netprint打印
 * 输出参数： 无
 * 返 回 值： 0   - 成功
 *            -1  - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int SetDbgChanel(char *strBuf)
{
	char *pStrTmp;
	char strName[100];
	PT_DebugOpr ptTmp;

	pStrTmp = strchr(strBuf, '=');
	if (!pStrTmp)
	{
		return -1;
	}
	else
	{
		/* 先截取名字, 然后根据名字找到调试模块 */
		strncpy(strName, strBuf, pStrTmp-strBuf);
		strName[pStrTmp-strBuf] = '\0';
		ptTmp = GetDebugOpr(strName);
		if (!ptTmp)
			return -1;

		if (pStrTmp[1] == '0')
			ptTmp->isCanUse = 0;
		else
			ptTmp->isCanUse = 1;
		return 0;
	}
	
}

/**********************************************************************
 * 函数名称： DebugPrint
 * 功能描述： 打印信息的总入口函数
 *            程序里用DBG_PRINTF来打印, 它就是DebugPrint
 *            在config.h里有这样的宏定义: #define DBG_PRINTF DebugPrint
 * 输入参数： 可变参数,用法和printf完全一样
 * 输出参数： 无
 * 返 回 值： 0   - 成功
 *            -1  - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int DebugPrint(const char *pcFormat, ...)
{
	char strTmpBuf[1000];
	char *pcTmp;
	va_list tArg;
	int iNum;
	PT_DebugOpr ptTmp = g_ptDebugOprHead;
	int dbglevel = DEFAULT_DBGLEVEL;

	/* 可变参数的处理, 抄自glibc的printf函数 */
	va_start (tArg, pcFormat);
	iNum = vsprintf (strTmpBuf, pcFormat, tArg);
	va_end (tArg);
	strTmpBuf[iNum] = '\0';


	pcTmp = strTmpBuf;
	
	/* 根据打印级别决定是否打印 */
	if ((strTmpBuf[0] == '<') && (strTmpBuf[2] == '>'))
	{
		dbglevel = strTmpBuf[1] - '0';
		if (dbglevel >= 0 && dbglevel <= 9)
		{
			pcTmp = strTmpBuf + 3;
		}
		else
		{
			dbglevel = DEFAULT_DBGLEVEL;
		}
	}

	if (dbglevel > g_iDbgLevelLimit)
	{
		return -1;
	}

	/* 调用链表中所有isCanUse为1的结构体的DebugPrint函数 
	 * 用来输出调试信息
	 */
	while (ptTmp)
	{
		if (ptTmp->isCanUse)
		{
			ptTmp->DebugPrint(pcTmp);
		}
		ptTmp = ptTmp->ptNext;
	}

	return 0;
	
}

/**********************************************************************
 * 函数名称： InitDebugChanel
 * 功能描述： 有些打印通道需要进行一些初始化, 比如网络打印需要绑定端口等等
 *            本函数用于执行这些初始化
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int InitDebugChanel(void)
{
	PT_DebugOpr ptTmp = g_ptDebugOprHead;
	while (ptTmp)
	{
		if (ptTmp->isCanUse && ptTmp->DebugInit)
		{
			ptTmp->DebugInit();
		}
		ptTmp = ptTmp->ptNext;
	}

	return 0;
}


/**********************************************************************
 * 函数名称： DebugInit
 * 功能描述： 注册调试通道,就是把PT_DebugOpr注册到链表中
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int DebugInit(void)
{
	int iError;

	iError = StdoutInit();
//	iError |= NetPrintInit();
	return iError;
}

