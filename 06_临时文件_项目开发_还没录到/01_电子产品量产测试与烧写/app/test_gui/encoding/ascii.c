
#include <config.h>
#include <encoding_manager.h>
#include <string.h>

static int isAsciiCoding(unsigned char *pucBufHead);
static int AsciiGetCodeFrmBuf(unsigned char *pucBufStart, unsigned char *pucBufEnd, unsigned int *pdwCode);

static T_EncodingOpr g_tAsciiEncodingOpr = {
	.name          = "ascii",
	.iHeadLen      = 0,
	.isSupport     = isAsciiCoding,
	.GetCodeFrmBuf = AsciiGetCodeFrmBuf,
};

/**********************************************************************
 * 函数名称： isAsciiCoding
 * 功能描述： 判断缓冲区中数据的编码方式是否为ANSI(英文字母用ASCII,汉字用GBK)
 * 输入参数： pucBufHead - 缓冲区首地址,一般是文本文件经过mmap得到的缓冲区地址
 * 输出参数： 无
 * 返 回 值： 0 - 不是
 *            1 - 是
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int isAsciiCoding(unsigned char *pucBufHead)
{
	const char aStrUtf8[]    = {0xEF, 0xBB, 0xBF, 0};
	const char aStrUtf16le[] = {0xFF, 0xFE, 0};
	const char aStrUtf16be[] = {0xFE, 0xFF, 0};
	
	if (strncmp((const char*)pucBufHead, aStrUtf8, 3) == 0)
	{
		/* UTF-8 */
		return 0;
	}
	else if (strncmp((const char*)pucBufHead, aStrUtf16le, 2) == 0)
	{
		/* UTF-16 little endian */
		return 0;
	}
	else if (strncmp((const char*)pucBufHead, aStrUtf16be, 2) == 0)
	{
		/* UTF-16 big endian */
		return 0;
	}
	else
	{
		return 1;
	}
}

/**********************************************************************
 * 函数名称： AsciiGetCodeFrmBuf
 * 功能描述： 从缓冲区中取出第1个字符的ANSI编码值(英文字母用ASCII,汉字用GBK)
 * 输入参数： pucBufStart - 缓冲区起始地址
 *            pucBufEnd   - 缓冲区结束地址(这个位置的字符不处理)
 * 输出参数： pdwCode     - 所取出的编码值存在这里
 * 返 回 值： 0      - 缓冲区处理完毕,没有得到编码值
 *            其他值 - 从pucBufStart开始用到了多少个字符才取得这个编码值
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int AsciiGetCodeFrmBuf(unsigned char *pucBufStart, unsigned char *pucBufEnd, unsigned int *pdwCode)
{
	unsigned char *pucBuf = pucBufStart;
	unsigned char c = *pucBuf;
	
	if ((pucBuf < pucBufEnd) && (c < (unsigned char)0x80))
	{
		/* 返回ASCII码 */
		*pdwCode = (unsigned int)c;
		return 1;
	}

	if (((pucBuf + 1) < pucBufEnd) && (c >= (unsigned char)0x80))
	{
		/* 返回GBK码 */
		*pdwCode = pucBuf[0] + (((unsigned int)pucBuf[1])<<8);
		return 2;
	}

	if (pucBuf < pucBufEnd)
	{
		/* 可能文件有损坏, 但是还是返回一个码, 即使它是错误的 */
		*pdwCode = (unsigned int)c;
		return 1;
	}
	else
	{
		/* 文件处理完毕 */
		return 0;
	}
}

/**********************************************************************
 * 函数名称： AsciiEncodingInit
 * 功能描述： 根据字符的ANSI编码值,有freetype/ascii/gbk这三种方法获得它的字符位图
 *            AsciiEncodingInit先给g_tAsciiEncodingOpr添加freetype/ascii/gbk这三种PT_FontOpr结构体;
 *            最后注册g_tAsciiEncodingOpr
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0      - 成功
 *            其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int AsciiEncodingInit(void)
{
	AddFontOprForEncoding(&g_tAsciiEncodingOpr, GetFontOpr("freetype"));
	AddFontOprForEncoding(&g_tAsciiEncodingOpr, GetFontOpr("ascii"));
	AddFontOprForEncoding(&g_tAsciiEncodingOpr, GetFontOpr("gbk"));
	return RegisterEncodingOpr(&g_tAsciiEncodingOpr);
}

