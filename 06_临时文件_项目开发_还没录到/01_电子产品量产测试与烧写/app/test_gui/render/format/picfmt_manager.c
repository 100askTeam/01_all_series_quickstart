
#include <config.h>
#include <pic_operation.h>
#include <picfmt_manager.h>
#include <string.h>


static PT_PicFileParser g_ptPicFileParserHead;

/**********************************************************************
 * 函数名称： RegisterPicFileParser
 * 功能描述： 注册"图片文件解析模块", "图片文件解析模块"就是怎么从BMP/JPG等图片文件中解析出象素数据
 * 输入参数： ptPicFileParser - 一个结构体,内含"图片文件解析模块"的操作函数
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int RegisterPicFileParser(PT_PicFileParser ptPicFileParser)
{
	PT_PicFileParser ptTmp;

	if (!g_ptPicFileParserHead)
	{
		g_ptPicFileParserHead   = ptPicFileParser;
		ptPicFileParser->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptPicFileParserHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = ptPicFileParser;
		ptPicFileParser->ptNext = NULL;
	}

	return 0;
}


/**********************************************************************
 * 函数名称： ShowPicFmts
 * 功能描述： 显示本程序能支持的"图片文件解析模块"
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
void ShowPicFmts(void)
{
	int i = 0;
	PT_PicFileParser ptTmp = g_ptPicFileParserHead;

	while (ptTmp)
	{
		printf("%02d %s\n", i++, ptTmp->name);
		ptTmp = ptTmp->ptNext;
	}
}

/**********************************************************************
 * 函数名称： Parser
 * 功能描述： 根据名字取出指定的"图片文件解析模块"
 * 输入参数： pcName - 名字
 * 输出参数： 无
 * 返 回 值： NULL   - 失败,没有指定的模块, 
 *            非NULL - "图片文件解析模块"的PT_PicFileParser结构体指针
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
PT_PicFileParser Parser(char *pcName)
{
	PT_PicFileParser ptTmp = g_ptPicFileParserHead;
	
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
 * 函数名称： GetParser
 * 功能描述： 找到能支持指定文件的"图片文件解析模块"
 * 输入参数： ptFileMap - 内含文件信息
 * 输出参数： 无
 * 返 回 值： NULL   - 失败,没有指定的模块, 
 *            非NULL - 支持该文件的"图片文件解析模块"的PT_PicFileParser结构体指针
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
PT_PicFileParser GetParser(PT_FileMap ptFileMap)
{
	PT_PicFileParser ptTmp = g_ptPicFileParserHead;
	
	while (ptTmp)
	{
		if (ptTmp->isSupport(ptFileMap))
		{
			return ptTmp;
		}
		ptTmp = ptTmp->ptNext;
	}
	return NULL;
}

/**********************************************************************
 * 函数名称： PicFmtsInit
 * 功能描述： 调用各个"图片文件解析模块"的初始化函数,就是注册它们
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int PicFmtsInit(void)
{
	int iError;

	iError = BMPParserInit();
//	iError |= JPGParserInit();
		
	return iError;
}




