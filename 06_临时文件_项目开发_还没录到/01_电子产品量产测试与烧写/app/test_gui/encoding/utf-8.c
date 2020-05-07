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
 * �������ƣ� isUtf8Coding
 * ���������� �жϻ����������ݵı��뷽ʽ�Ƿ�ΪUTF8
 * ��������� pucBufHead - �������׵�ַ,һ�����ı��ļ�����mmap�õ��Ļ�������ַ
 * ��������� ��
 * �� �� ֵ�� 0 - ����
 *            1 - ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
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
 * �������ƣ� GetPreOneBits
 * ���������� ���"��bit7��ʼΪ1��λ�ĸ���"
 *            ����������� 11001111 ��2λ
 *                         11100001 ��3λ
 *                         01100001 ��0λ
 * ��������� ucVal - Ҫ��������
 * ��������� ��
 * �� �� ֵ�� "��bit7��ʼΪ1��λ�ĸ���"
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
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
 * �������ƣ� Utf8GetCodeFrmBuf
 * ���������� �ӻ�������ȡ����1���ַ���UTF8���벢��ת��ΪUNICODEֵ
 * ��������� pucBufStart - ��������ʼ��ַ
 *            pucBufEnd   - ������������ַ(���λ�õ��ַ�������)
 * ��������� pdwCode     - ��ȡ���ı���ֵ��������
 * �� �� ֵ�� 0      - �������������,û�еõ�����ֵ
 *            ����ֵ - ��pucBufStart��ʼ�õ��˶��ٸ��ַ���ȡ���������ֵ
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static int Utf8GetCodeFrmBuf(unsigned char *pucBufStart, unsigned char *pucBufEnd, unsigned int *pdwCode)
{
#if 0
    ����UTF-8�����е������ֽ�B�����B�ĵ�һλΪ0����BΪASCII�룬����B�����ı�ʾһ���ַ�;
    ���B�ĵ�һλΪ1���ڶ�λΪ0����BΪһ����ASCII�ַ������ַ��ɶ���ֽڱ�ʾ���е�һ���ֽڣ����Ҳ�Ϊ�ַ��ĵ�һ���ֽڱ���;
    ���B��ǰ��λΪ1������λΪ0����BΪһ����ASCII�ַ������ַ��ɶ���ֽڱ�ʾ���еĵ�һ���ֽڣ����Ҹ��ַ��������ֽڱ�ʾ;
    ���B��ǰ��λΪ1������λΪ0����BΪһ����ASCII�ַ������ַ��ɶ���ֽڱ�ʾ���еĵ�һ���ֽڣ����Ҹ��ַ��������ֽڱ�ʾ;
    ���B��ǰ��λΪ1������λΪ0����BΪһ����ASCII�ַ������ַ��ɶ���ֽڱ�ʾ���еĵ�һ���ֽڣ����Ҹ��ַ����ĸ��ֽڱ�ʾ;

    ��ˣ���UTF-8�����е������ֽڣ����ݵ�һλ�����ж��Ƿ�ΪASCII�ַ�;
    ����ǰ��λ�����жϸ��ֽ��Ƿ�Ϊһ���ַ�����ĵ�һ���ֽ�; 
    ����ǰ��λ�����ǰ��λ��Ϊ1������ȷ�����ֽ�Ϊ�ַ�����ĵ�һ���ֽڣ����ҿ��ж϶�Ӧ���ַ��ɼ����ֽڱ�ʾ;
    ����ǰ��λ�����ǰ��λΪ1�������жϱ����Ƿ��д�������ݴ���������Ƿ��д���
#endif

	int i;	
	int iNum;
	unsigned char ucVal;
	unsigned int dwSum = 0;

	if (pucBufStart >= pucBufEnd)
	{
		/* �ļ����� */
		return 0;
	}

	ucVal = pucBufStart[0];
	iNum  = GetPreOneBits(pucBufStart[0]);

	if ((pucBufStart + iNum) > pucBufEnd)
	{
		/* �ļ����� */
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
 * �������ƣ� Utf8EncodingInit
 * ���������� �����ַ���UNICODE����ֵ,��freetype/ascii��2�ַ�����������ַ�λͼ
 *            Utf8EncodingInit�ȸ�g_tUtf8EncodingOpr���freetype/ascii��2��PT_FontOpr�ṹ��;
 *            ���ע��g_tUtf8EncodingOpr
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0      - �ɹ�
 *            ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int  Utf8EncodingInit(void)
{
	AddFontOprForEncoding(&g_tUtf8EncodingOpr, GetFontOpr("freetype"));
	AddFontOprForEncoding(&g_tUtf8EncodingOpr, GetFontOpr("ascii"));
	return RegisterEncodingOpr(&g_tUtf8EncodingOpr);
}

