
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
 * �������ƣ� isAsciiCoding
 * ���������� �жϻ����������ݵı��뷽ʽ�Ƿ�ΪANSI(Ӣ����ĸ��ASCII,������GBK)
 * ��������� pucBufHead - �������׵�ַ,һ�����ı��ļ�����mmap�õ��Ļ�������ַ
 * ��������� ��
 * �� �� ֵ�� 0 - ����
 *            1 - ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
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
 * �������ƣ� AsciiGetCodeFrmBuf
 * ���������� �ӻ�������ȡ����1���ַ���ANSI����ֵ(Ӣ����ĸ��ASCII,������GBK)
 * ��������� pucBufStart - ��������ʼ��ַ
 *            pucBufEnd   - ������������ַ(���λ�õ��ַ�������)
 * ��������� pdwCode     - ��ȡ���ı���ֵ��������
 * �� �� ֵ�� 0      - �������������,û�еõ�����ֵ
 *            ����ֵ - ��pucBufStart��ʼ�õ��˶��ٸ��ַ���ȡ���������ֵ
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static int AsciiGetCodeFrmBuf(unsigned char *pucBufStart, unsigned char *pucBufEnd, unsigned int *pdwCode)
{
	unsigned char *pucBuf = pucBufStart;
	unsigned char c = *pucBuf;
	
	if ((pucBuf < pucBufEnd) && (c < (unsigned char)0x80))
	{
		/* ����ASCII�� */
		*pdwCode = (unsigned int)c;
		return 1;
	}

	if (((pucBuf + 1) < pucBufEnd) && (c >= (unsigned char)0x80))
	{
		/* ����GBK�� */
		*pdwCode = pucBuf[0] + (((unsigned int)pucBuf[1])<<8);
		return 2;
	}

	if (pucBuf < pucBufEnd)
	{
		/* �����ļ�����, ���ǻ��Ƿ���һ����, ��ʹ���Ǵ���� */
		*pdwCode = (unsigned int)c;
		return 1;
	}
	else
	{
		/* �ļ�������� */
		return 0;
	}
}

/**********************************************************************
 * �������ƣ� AsciiEncodingInit
 * ���������� �����ַ���ANSI����ֵ,��freetype/ascii/gbk�����ַ�����������ַ�λͼ
 *            AsciiEncodingInit�ȸ�g_tAsciiEncodingOpr���freetype/ascii/gbk������PT_FontOpr�ṹ��;
 *            ���ע��g_tAsciiEncodingOpr
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0      - �ɹ�
 *            ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int AsciiEncodingInit(void)
{
	AddFontOprForEncoding(&g_tAsciiEncodingOpr, GetFontOpr("freetype"));
	AddFontOprForEncoding(&g_tAsciiEncodingOpr, GetFontOpr("ascii"));
	AddFontOprForEncoding(&g_tAsciiEncodingOpr, GetFontOpr("gbk"));
	return RegisterEncodingOpr(&g_tAsciiEncodingOpr);
}

