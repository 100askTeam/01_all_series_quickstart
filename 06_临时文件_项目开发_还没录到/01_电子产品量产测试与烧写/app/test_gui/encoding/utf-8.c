#include <config.h>
#include <encoding_manager.h>
#include <string.h>

static int isUtf8Coding(unsigned char *pucBufHead);
static int Utf8GetCodeFrmBuf(unsigned char *pucBufStart, unsigned char *pucBufEnd, unsigned int *pdwCode);

static T_EncodingOpr g_tUtf8EncodingOpr = {
	.name          = "utf-8",
	.iHeadLen	   = 3,
	.isSupport     = isUtf8Coding,
	.GetCodeFrmBuf = Utf8GetCodeFrmBuf,
};

/**********************************************************************
 * 函数名称： isUtf8Coding
 * 功能描述： 判断缓冲区中数据的编码方式是否为UTF8
 * 输入参数： pucBufHead - 缓冲区首地址,一般是文本文件经过mmap得到的缓冲区地址
 * 输出参数： 无
 * 返 回 值： 0 - 不是
 *            1 - 是
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int isUtf8Coding(unsigned char *pucBufHead)
{
	const char aStrUtf8[]    = {0xEF, 0xBB, 0xBF, 0};
	if (strncmp((const char*)pucBufHead, aStrUtf8, 3) == 0)
	{
		/* UTF-8 */
		return 1;
	}
	else
	{
		return 0;
	}
}

/**********************************************************************
 * 函数名称： GetPreOneBits
 * 功能描述： 获得"从bit7开始为1的位的个数"
 *            比如二进制数 11001111 有2位
 *                         11100001 有3位
 *                         01100001 有0位
 * 输入参数： ucVal - 要分析的数
 * 输出参数： 无
 * 返 回 值： "从bit7开始为1的位的个数"
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int GetPreOneBits(unsigned char ucVal)
{
	int i;
	int j = 0;
	
	for (i = 7; i >= 0; i--)
	{
		if (!(ucVal & (1<<i)))
			break;
		else
			j++;
	}
	return j;

}

/**********************************************************************
 * 函数名称： Utf8GetCodeFrmBuf
 * 功能描述： 从缓冲区中取出第1个字符的UTF8编码并且转换为UNICODE值
 * 输入参数： pucBufStart - 缓冲区起始地址
 *            pucBufEnd   - 缓冲区结束地址(这个位置的字符不处理)
 * 输出参数： pdwCode     - 所取出的编码值存在这里
 * 返 回 值： 0      - 缓冲区处理完毕,没有得到编码值
 *            其他值 - 从pucBufStart开始用到了多少个字符才取得这个编码值
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int Utf8GetCodeFrmBuf(unsigned char *pucBufStart, unsigned char *pucBufEnd, unsigned int *pdwCode)
{
#if 0
    对于UTF-8编码中的任意字节B，如果B的第一位为0，则B为ASCII码，并且B独立的表示一个字符;
    如果B的第一位为1，第二位为0，则B为一个非ASCII字符（该字符由多个字节表示）中的一个字节，并且不为字符的第一个字节编码;
    如果B的前两位为1，第三位为0，则B为一个非ASCII字符（该字符由多个字节表示）中的第一个字节，并且该字符由两个字节表示;
    如果B的前三位为1，第四位为0，则B为一个非ASCII字符（该字符由多个字节表示）中的第一个字节，并且该字符由三个字节表示;
    如果B的前四位为1，第五位为0，则B为一个非ASCII字符（该字符由多个字节表示）中的第一个字节，并且该字符由四个字节表示;

    因此，对UTF-8编码中的任意字节，根据第一位，可判断是否为ASCII字符;
    根据前二位，可判断该字节是否为一个字符编码的第一个字节; 
    根据前四位（如果前两位均为1），可确定该字节为字符编码的第一个字节，并且可判断对应的字符由几个字节表示;
    根据前五位（如果前四位为1），可判断编码是否有错误或数据传输过程中是否有错误。
#endif

	int i;	
	int iNum;
	unsigned char ucVal;
	unsigned int dwSum = 0;

	if (pucBufStart >= pucBufEnd)
	{
		/* 文件结束 */
		return 0;
	}

	ucVal = pucBufStart[0];
	iNum  = GetPreOneBits(pucBufStart[0]);

	if ((pucBufStart + iNum) > pucBufEnd)
	{
		/* 文件结束 */
		return 0;
	}

	if (iNum == 0)
	{
		/* ASCII */
		*pdwCode = pucBufStart[0];
		return 1;
	}
	else
	{
		ucVal = ucVal << iNum;
		ucVal = ucVal >> iNum;
		dwSum += ucVal;
		for (i = 1; i < iNum; i++)
		{
			ucVal = pucBufStart[i] & 0x3f;
			dwSum = dwSum << 6;
			dwSum += ucVal;			
		}
		*pdwCode = dwSum;
		return iNum;
	}
}

/**********************************************************************
 * 函数名称： Utf8EncodingInit
 * 功能描述： 根据字符的UNICODE编码值,有freetype/ascii这2种方法获得它的字符位图
 *            Utf8EncodingInit先给g_tUtf8EncodingOpr添加freetype/ascii这2种PT_FontOpr结构体;
 *            最后注册g_tUtf8EncodingOpr
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0      - 成功
 *            其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int  Utf8EncodingInit(void)
{
	AddFontOprForEncoding(&g_tUtf8EncodingOpr, GetFontOpr("freetype"));
	AddFontOprForEncoding(&g_tUtf8EncodingOpr, GetFontOpr("ascii"));
	return RegisterEncodingOpr(&g_tUtf8EncodingOpr);
}

