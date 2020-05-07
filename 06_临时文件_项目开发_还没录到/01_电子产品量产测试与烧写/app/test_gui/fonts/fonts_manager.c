#include <config.h>
#include <fonts_manager.h>
#include <string.h>

static PT_FontOpr g_ptFontOprHead = NULL;
static int g_dwFontSize;

/**********************************************************************
 * 函数名称： RegisterFontOpr
 * 功能描述： 注册"字体模块", 所谓字体模块就是取出字符位图的方法
 * 输入参数： ptFontOpr - 一个结构体,内含"取出字符位图"的操作函数
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int RegisterFontOpr(PT_FontOpr ptFontOpr)
{
	PT_FontOpr ptTmp;

	if (!g_ptFontOprHead)
	{
		g_ptFontOprHead   = ptFontOpr;
		ptFontOpr->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptFontOprHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext     = ptFontOpr;
		ptFontOpr->ptNext = NULL;
	}

	return 0;
}


/**********************************************************************
 * 函数名称： ShowFontOpr
 * 功能描述： 显示本程序能支持的"字体模块"
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
void ShowFontOpr(void)
{
	int i = 0;
	PT_FontOpr ptTmp = g_ptFontOprHead;

	while (ptTmp)
	{
		printf("%02d %s\n", i++, ptTmp->name);
		ptTmp = ptTmp->ptNext;
	}
}

/**********************************************************************
 * 函数名称： GetFontOpr
 * 功能描述： 根据名字取出指定的"字体模块"
 * 输入参数： pcName - 名字
 * 输出参数： 无
 * 返 回 值： NULL   - 失败,没有指定的模块, 
 *            非NULL - 字体模块的PT_FontOpr结构体指针
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
PT_FontOpr GetFontOpr(char *pcName)
{
	PT_FontOpr ptTmp = g_ptFontOprHead;
	
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
 * 函数名称： SetFontSize
 * 功能描述： 设置字符的尺寸(单位:色素)
 * 输入参数： dwFontSize - 字符的尺寸(单位:色素)
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
void SetFontSize(unsigned int dwFontSize)
{
	PT_FontOpr ptTmp = g_ptFontOprHead;
	
	g_dwFontSize = dwFontSize;

	while (ptTmp)
	{
		if (ptTmp->SetFontSize)
		{
			ptTmp->SetFontSize(dwFontSize);
		}
		ptTmp = ptTmp->ptNext;
	}
}

/**********************************************************************
 * 函数名称： GetFontSize
 * 功能描述： 获得字符的尺寸(单位:色素)
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 字符的尺寸(单位:色素)
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
unsigned int GetFontSize(void)
{
	return g_dwFontSize;
}

/**********************************************************************
 * 函数名称： GetFontBitmap
 * 功能描述： 获得字符的位图
 * 输入参数： dwCode       - 字符的编码值(可能是ASCII/GBK/UNICODE)
 * 输出参数： ptFontBitMap - 内含位图信息
 * 返 回 值： 0  - 成功
 *            -1 - 失败 
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int GetFontBitmap(unsigned int dwCode, PT_FontBitMap ptFontBitMap)
{
	int iError = 0;
	PT_FontOpr ptTmp = g_ptFontOprHead;

	while (ptTmp)
	{
		iError = ptTmp->GetFontBitmap(dwCode, ptFontBitMap);
		if (0 == iError)
		{
			return 0;
		}
		ptTmp = ptTmp->ptNext;
	}
	return -1;
}


/**********************************************************************
 * 函数名称： SetFontsDetail
 * 功能描述： 设置字体模块的信息,比如指定字库文件,指定字符尺寸
 * 输入参数： pcFontsName - 要设置的字体模块的名字
 *            pcFontsFile - 字库文件
 *            dwFontSize  - 字符的尺寸(单位:色素), 程序运行时可用SetFontSize修改
 * 输出参数： 无
 * 返 回 值： 0      - 成功
 *            其他值 - 失败 
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int SetFontsDetail(char *pcFontsName, char *pcFontsFile, unsigned int dwFontSize)
{
	int iError = 0;
	PT_FontOpr ptFontOpr;

	ptFontOpr = GetFontOpr(pcFontsName);
	if (NULL == ptFontOpr)
	{
		return -1;
	}

	g_dwFontSize = dwFontSize;

	iError = ptFontOpr->FontInit(pcFontsFile, dwFontSize);
	
	return iError;
}


/**********************************************************************
 * 函数名称： FontsInit
 * 功能描述： 调用各个字体模块的初始化函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int FontsInit(void)
{

#if 0	
	int iError;

	iError = ASCIIInit();
	if (iError)
	{
		DBG_PRINTF("ASCIIInit error!\n");
		return -1;
	}

	iError = GBKInit();
	if (iError)
	{
		DBG_PRINTF("GBKInit error!\n");
		return -1;
	}
	iError = FreeTypeInit();
	if (iError)
	{
		DBG_PRINTF("FreeTypeInit error!\n");
		return -1;
	}
#endif	
	int iError;

	iError = FreeTypeInit();
	if (iError)
	{
		DBG_PRINTF("FreeTypeInit error!\n");
		return -1;
	}

	return 0;
}

