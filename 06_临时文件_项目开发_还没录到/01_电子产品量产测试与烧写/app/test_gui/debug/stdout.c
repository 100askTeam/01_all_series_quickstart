
#include <config.h>
#include <debug_manager.h>
#include <stdio.h>
#include <string.h>


/**********************************************************************
 * 函数名称： StdoutDebugPrint
 * 功能描述： "标准输出调试通道"的输出函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 输出信息的长度
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int StdoutDebugPrint(char *strData)
{
	/* 直接把输出信息用printf打印出来 */
	printf("%s", strData);
	return strlen(strData);	
}

static T_DebugOpr g_tStdoutDbgOpr = {
	.name       = "stdout",
	.isCanUse   = 1,                 /* 1表示将使用它来输出调试信息 */
	.DebugPrint = StdoutDebugPrint,  /* 打印函数 */
};

/**********************************************************************
 * 函数名称： StdoutInit
 * 功能描述： 注册"标准输出调试通道", 把g_tStdoutDbgOpr结构体放入链表中
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int StdoutInit(void)
{
	return RegisterDebugOpr(&g_tStdoutDbgOpr);
}

