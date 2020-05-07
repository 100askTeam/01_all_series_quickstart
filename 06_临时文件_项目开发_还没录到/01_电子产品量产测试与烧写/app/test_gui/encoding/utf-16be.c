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
 * �������ƣ� isUtf16beCoding
 * ���������� �жϻ����������ݵı��뷽ʽ�Ƿ�ΪUTF16BE
 * ��������� pucBufHead - �������׵�ַ,һ�����ı��ļ�����mmap�õ��Ļ�������ַ
 * ��������� ��
 * �� �� ֵ�� 0 - ����
 *            1 - ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
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
 * �������ƣ� Utf16beGetCodeFrmBuf
 * ���������� �ӻ�������ȡ����1���ַ���UTF16BE���벢��ת��ΪUNICODEֵ
 * ��������� pucBufStart - ��������ʼ��ַ
 *            pucBufEnd   - ������������ַ(���λ�õ��ַ�������)
 * ��������� pdwCode     - ��ȡ���ı���ֵ��������
 * �� �� ֵ�� 0      - �������������,û�еõ�����ֵ
 *            ����ֵ - ��pucBufStart��ʼ�õ��˶��ٸ��ַ���ȡ���������ֵ
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
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
		/* �ļ����� */
		return 0;
	}
}

/**********************************************************************
 * �������ƣ� Utf16beEncodingInit
 * ���������� �����ַ���UNICODE����ֵ,��freetype/ascii��2�ַ�����������ַ�λͼ
 *            Utf16beEncodingInit�ȸ�g_tUtf16beEncodingOpr���freetype/ascii��2��PT_FontOpr�ṹ��;
 *            ���ע��g_tUtf16beEncodingOpr
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0      - �ɹ�
 *            ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int  Utf16beEncodingInit(void)
{
	AddFontOprForEncoding(&g_tUtf16beEncodingOpr, GetFontOpr("freetype"));
	AddFontOprForEncoding(&g_tUtf16beEncodingOpr, GetFontOpr("ascii"));
	return RegisterEncodingOpr(&g_tUtf16beEncodingOpr);
}



