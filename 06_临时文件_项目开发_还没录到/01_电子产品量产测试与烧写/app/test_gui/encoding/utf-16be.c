#include <config.h>
#include <encoding_manager.h>
#include <string.h>

static int isUtf16beCoding(unsigned char *pucBufHead);
static int Utf16beGetCodeFrmBuf(unsigned char *pucBufStart, unsigned char *pucBufEnd, unsigned int *pdwCode);

static T_EncodingOpr g_tUtf16beEncodingOpr = {
	.name          = "utf-16be",
	.iHeadLen	   = 2,
	.isSupport     = isUtf16beCoding,
	.GetCodeFrmBuf = Utf16beGetCodeFrmBuf,
};

/**********************************************************************
 * 函数名称： isUtf16beCoding
 * 功能描述： 判断缓冲区中数据的编码方式是否为UTF16BE
 * 输入参数： pucBufHead - 缓冲区首地址,一般是文本文件经过mmap得到的缓冲区地址
 * 输出参数： 无
 * 返 回 值： 0 - 不是
 *            1 - 是
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int isUtf16beCoding(unsigned char *pucBufHead)
{
	const char aStrUtf16be[] = {0xFE, 0xFF, 0};
	
	if (strncmp((const char*)pucBufHead, aStrUtf16be, 2) == 0)
	{
		/* UTF-16 big endian */
		return 1;
	}
	else
	{
		return 0;
	}
}

/**********************************************************************
 * 函数名称： Utf16beGetCodeFrmBuf
 * 功能描述： 从缓冲区中取出第1个字符的UTF16BE编码并且转换为UNICODE值
 * 输入参数： pucBufStart - 缓冲区起始地址
 *            pucBufEnd   - 缓冲区结束地址(这个位置的字符不处理)
 * 输出参数： pdwCode     - 所取出的编码值存在这里
 * 返 回 值： 0      - 缓冲区处理完毕,没有得到编码值
 *            其他值 - 从pucBufStart开始用到了多少个字符才取得这个编码值
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int Utf16beGetCodeFrmBuf(unsigned char *pucBufStart, unsigned char *pucBufEnd, unsigned int *pdwCode)
{
	if (pucBufStart + 1 < pucBufEnd)
	{
		*pdwCode = (((unsigned int)pucBufStart[0])<<8) + pucBufStart[1];
		return 2;
	}
	else
	{
		/* 文件结束 */
		return 0;
	}
}

/**********************************************************************
 * 函数名称： Utf16beEncodingInit
 * 功能描述： 根据字符的UNICODE编码值,有freetype/ascii这2种方法获得它的字符位图
 *            Utf16beEncodingInit先给g_tUtf16beEncodingOpr添加freetype/ascii这2种PT_FontOpr结构体;
 *            最后注册g_tUtf16beEncodingOpr
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0      - 成功
 *            其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int  Utf16beEncodingInit(void)
{
	AddFontOprForEncoding(&g_tUtf16beEncodingOpr, GetFontOpr("freetype"));
	AddFontOprForEncoding(&g_tUtf16beEncodingOpr, GetFontOpr("ascii"));
	return RegisterEncodingOpr(&g_tUtf16beEncodingOpr);
}



