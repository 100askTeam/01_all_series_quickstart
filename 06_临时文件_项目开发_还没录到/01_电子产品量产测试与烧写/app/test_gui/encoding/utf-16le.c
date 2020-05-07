#include <config.h>
#include <encoding_manager.h>
#include <string.h>

static int isUtf16leCoding(unsigned char *pucBufHead);
static int Utf16leGetCodeFrmBuf(unsigned char *pucBufStart, unsigned char *pucBufEnd, unsigned int *pdwCode);

static T_EncodingOpr g_tUtf16leEncodingOpr = {
	.name          = "utf-16le",
	.iHeadLen	   = 2,
	.isSupport     = isUtf16leCoding,
	.GetCodeFrmBuf = Utf16leGetCodeFrmBuf,
};

/**********************************************************************
 * �������ƣ� isUtf16leCoding
 * ���������� �жϻ����������ݵı��뷽ʽ�Ƿ�ΪUTF16LE
 * ��������� pucBufHead - �������׵�ַ,һ�����ı��ļ�����mmap�õ��Ļ�������ַ
 * ��������� ��
 * �� �� ֵ�� 0 - ����
 *            1 - ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static int isUtf16leCoding(unsigned char *pucBufHead)
{
	const char aStrUtf16le[] = {0xFF, 0xFE, 0};
	if (strncmp((const char *)pucBufHead, aStrUtf16le, 2) == 0)
	{
		/* UTF-16 little endian */
		return 1;
	}
	else
	{
		return 0;
	}
}

/**********************************************************************
 * �������ƣ� Utf16leGetCodeFrmBuf
 * ���������� �ӻ�������ȡ����1���ַ���UTF16LE���벢��ת��ΪUNICODEֵ
 * ��������� pucBufStart - ��������ʼ��ַ
 *            pucBufEnd   - ������������ַ(���λ�õ��ַ�������)
 * ��������� pdwCode     - ��ȡ���ı���ֵ��������
 * �� �� ֵ�� 0      - �������������,û�еõ�����ֵ
 *            ����ֵ - ��pucBufStart��ʼ�õ��˶��ٸ��ַ���ȡ���������ֵ
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static int Utf16leGetCodeFrmBuf(unsigned char *pucBufStart, unsigned char *pucBufEnd, unsigned int *pdwCode)
{
	if (pucBufStart + 1 < pucBufEnd)
	{
		*pdwCode = (((unsigned int)pucBufStart[1])<<8) + pucBufStart[0];
		return 2;
	}
	else
	{
		/* �ļ����� */
		return 0;
	}
}

/**********************************************************************
 * �������ƣ� Utf16leEncodingInit
 * ���������� �����ַ���UNICODE����ֵ,��freetype/ascii��2�ַ�����������ַ�λͼ
 *            Utf16leEncodingInit�ȸ�g_tUtf16leEncodingOpr���freetype/ascii��2��PT_FontOpr�ṹ��;
 *            ���ע��g_tUtf16leEncodingOpr
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0      - �ɹ�
 *            ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int  Utf16leEncodingInit(void)
{
	AddFontOprForEncoding(&g_tUtf16leEncodingOpr, GetFontOpr("freetype"));
	AddFontOprForEncoding(&g_tUtf16leEncodingOpr, GetFontOpr("ascii"));
	return RegisterEncodingOpr(&g_tUtf16leEncodingOpr);
}


