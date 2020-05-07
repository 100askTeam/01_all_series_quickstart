#include <config.h>
#include <encoding_manager.h>
#include <string.h>
#include <stdlib.h>

static PT_EncodingOpr g_ptEncodingOprHead;

/**********************************************************************
 * �������ƣ� RegisterEncodingOpr
 * ���������� ע��"����ģ��", һ�����ֿ��ܲ���ASCII����,Ҳ������UTF8�ȱ���.
 *            ��ô��һ��������ȡ��һ����,��ͬ�ı���ȡ����һ��,
 *            ÿһ�ֱ��뷽ʽ������RegisterEncodingOpr����ؽṹ�����g_ptEncodingOprHead����
 * ��������� ptEncodingOpr - һ���ṹ��,�ں�ĳ�ֱ��뷽ʽ�Ĳ�������
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int RegisterEncodingOpr(PT_EncodingOpr ptEncodingOpr)
{
	PT_EncodingOpr ptTmp;

	if (!g_ptEncodingOprHead)
	{
		g_ptEncodingOprHead   = ptEncodingOpr;
		ptEncodingOpr->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptEncodingOprHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	      = ptEncodingOpr;
		ptEncodingOpr->ptNext = NULL;
	}

	return 0;
}


/**********************************************************************
 * �������ƣ� Encode
 * ���������� ��������ȡ��ָ����"����ģ��"
 * ��������� pcName - ����
 * ��������� ��
 * �� �� ֵ�� NULL   - ʧ��,û��ָ����ģ��, 
 *            ��NULL - "ҳ��ģ��"��PT_EncodingOpr�ṹ��ָ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
PT_EncodingOpr Encode(char *pcName)
{
	PT_EncodingOpr ptTmp = g_ptEncodingOprHead;
	
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
 * �������ƣ� ShowEncodingOpr
 * ���������� ��ʾ��������֧�ֵ�"���뷽ʽ"
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
void ShowEncodingOpr(void)
{
	int i = 0;
	PT_EncodingOpr ptTmp = g_ptEncodingOprHead;

	while (ptTmp)
	{
		printf("%02d %s\n", i++, ptTmp->name);
		ptTmp = ptTmp->ptNext;
	}
}

/**********************************************************************
 * �������ƣ� SelectEncodingOprForFile
 * ���������� ���ļ�ѡ��һ������ģ��
 * ��������� pucFileBufHead - �ļ�ͷ��, ������mmapӳ���ļ����õ��ڴ�ָ��
 * ��������� ��
 * �� �� ֵ�� NULL   - ���еı��뷽ʽ����֧�ִ��ļ�
 *            ��NULL - ֧�ִ��ļ��ı��뷽ʽ��Ӧ�Ľṹ��ָ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
PT_EncodingOpr SelectEncodingOprForFile(unsigned char *pucFileBufHead)
{
	PT_EncodingOpr ptTmp = g_ptEncodingOprHead;

	while (ptTmp)
	{	
		if (ptTmp->isSupport(pucFileBufHead))
			return ptTmp;
		else
			ptTmp = ptTmp->ptNext;
	}
	return NULL;
}

/**********************************************************************
 * �������ƣ� GetCodeFrmBuf
 * ���������� ���ַ�����ȡ��UTF8��ʽ���ַ�����UNICODE����ֵ
 * ��������� pucBufStart - �ַ�����ʼλ��
 *            pucBufEnd   - �ַ�������λ��(���λ�õ��ַ�������)
 * ��������� pdwCode     - ��ȡ���ı���ֵ��������
 * �� �� ֵ�� ʹ���˶��ٸ��ַ��ŵõ��������ֵ
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int GetCodeFrmBuf(unsigned char *pucBufStart, unsigned char *pucBufEnd, unsigned int *pdwCode)
{
    return Encode("utf-8")->GetCodeFrmBuf(pucBufStart, pucBufEnd, pdwCode);
}


/**********************************************************************
 * �������ƣ� AddFontOprForEncoding
 * ���������� Ҫ��ʾһ���ַ�, �漰��������:
 *            1. �ַ��ı���ֵ, ������ļ���ʲô��ֵ����ʾ, ����'A'��0x41����ʾ,
 *               '��'�ı��뷽ʽ�ж���, GBK���뷽ʽ��������ֵΪ0xD0D6, ����UNICODE
 *               ������������ֵΪ0x4E2D
 *            2. ����ֵ�ı�ʾ��ʽ, ����UTF8/UTF16LE/UTF16BE��UNICODE��������ֱ�ʾ��ʽ,����:
 *               'A'��ASCII��ʾΪ0x41;UTF16-LE��ʾΪ0x41,0x00;UTF16-BE��ʾΪ0x00,0x41;UTF8��ʾΪ0x41
 *               '��'��GBK��ʾΪ0xD6,0xD0;UTF16-LE��ʾΪ0x2D,0x4E;UTF16-BE��ʾΪ0x4E,0x2D;UTF8��ʾΪ0xE4,0xB8,0xAD
 *               ע��:UTF8/UTF16LE/UTF16BEֻ��UNICODE����Ĳ�ͬ��ʾ��ʽ,����ͬһ���ַ�,���Ǳ��ı���ֵ��һ����,ֻ�Ǳ�ʾ������ͬ
 *            3. �ַ���λͼ,���ַ�Ӧ����ô�軭
 *            ����: ʹ��freetypeʸ���ֿ�����ʾUTF8�����ʽ���ļ�����ʱ,
 *                  ����Ҫ��ȡ��UTF8��ֵ����������UNICODEֵ,Ȼ�����UNICODEֵȥ�ֿ��ļ���ȡ��λͼ,
 *                  �����LCD����ʾ����
 *            һ���ı��ļ��ı��뷽ʽ��ȷ����,
 *        ������Ҫô��ANSI(��ĸ��ASCII,������GBK),Ҫô��UTF8�ȵ�; �ļ����ַ���λͼ��Դ�����ж���,
 *        ����ASCII�ַ����Դ�ȷ���������л������λͼ, GBK�ַ����Դ�"HZK16"����ļ��л������λͼ,
 *        Ҳ���Դ�ĳ��freetypeʸ���ֿ��л������λͼ.
 *           AddFontOprForEncoding�������Ǹ�ĳ�����뷽ʽ(ptEncodingOpr)�ṩ�ַ��Ĵ�����(ptFontOpr),
 *        ptFontOpr��������"ptEncodingOpr->ptFontOprSupportedHead"������
 * ��������� ptEncodingOpr - ���뷽ʽ
 *            ptFontOpr     - �ַ�����ṹ��,�ں�"GetFontBitmap"����(�����ݱ���ֵ���λͼ)
 * ��������� ��
 * �� �� ֵ�� 0   - �ɹ�
 *            -1  - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int AddFontOprForEncoding(PT_EncodingOpr ptEncodingOpr, PT_FontOpr ptFontOpr)
{
	PT_FontOpr ptFontOprCpy;
	
	if (!ptEncodingOpr || !ptFontOpr)
	{
		return -1;
	}
	else
	{
        /* ����һ���µ�T_FontOpr�ṹ�� */
		ptFontOprCpy = malloc(sizeof(T_FontOpr));
		if (!ptFontOprCpy)
		{
			return -1;
		}
		else
		{
            /* ���ƽṹ��������� */
			memcpy(ptFontOprCpy, ptFontOpr, sizeof(T_FontOpr));

            /* ���½ṹ�����ptEncodingOpr->ptFontOprSupportedHead���� 
             * Ϊ�β�ֱ�Ӱ�ԭ����ptFontOpr��������?
             * ��Ϊԭ����ptFontOpr->ptNext�Ѿ���RegisterFontOpr�����б�ռ����
             */
			ptFontOprCpy->ptNext = ptEncodingOpr->ptFontOprSupportedHead;
			ptEncodingOpr->ptFontOprSupportedHead = ptFontOprCpy;
			return 0;
		}		
	}
}


/**********************************************************************
 * �������ƣ� DelFontOprFrmEncoding
 * ���������� AddFontOprForEncoding�ķ�����,��PT_EncodingOpr->ptFontOprSupportedHead�����а�ĳ��PT_FontOprȥ��
 * ��������� ptEncodingOpr - ���뷽ʽ
 *            ptFontOpr     - �ַ�����ṹ��,�ں�"GetFontBitmap"����(�����ݱ���ֵ���λͼ)
 * ��������� ��
 * �� �� ֵ�� 0  - �ɹ�
 *            -1 - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int DelFontOprFrmEncoding(PT_EncodingOpr ptEncodingOpr, PT_FontOpr ptFontOpr)
{
	PT_FontOpr ptTmp;
	PT_FontOpr ptPre;
		
	if (!ptEncodingOpr || !ptFontOpr)
	{
		return -1;
	}
	else
	{
		ptTmp = ptEncodingOpr->ptFontOprSupportedHead;
		if (strcmp(ptTmp->name, ptFontOpr->name) == 0)
		{
			/* ɾ��ͷ�ڵ� */
			ptEncodingOpr->ptFontOprSupportedHead = ptTmp->ptNext;
			free(ptTmp);
			return 0;
		}

		ptPre = ptEncodingOpr->ptFontOprSupportedHead;
		ptTmp = ptPre->ptNext;
		while (ptTmp)
		{
			if (strcmp(ptTmp->name, ptFontOpr->name) == 0)
			{
				/* ��������ȡ�����ͷ� */
				ptPre->ptNext = ptTmp->ptNext;
				free(ptTmp);
				return 0;
			}
			else
			{
				ptPre = ptTmp;
				ptTmp = ptTmp->ptNext;
			}
		}

		return -1;
	}
}

/**********************************************************************
 * �������ƣ� EncodingInit
 * ���������� ���ø��ֱ��뷽ʽ�ĳ�ʼ������
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int EncodingInit(void)
{
	int iError;

	iError = AsciiEncodingInit();
	if (iError)
	{
		DBG_PRINTF("AsciiEncodingInit error!\n");
		return -1;
	}

	iError = Utf16leEncodingInit();
	if (iError)
	{
		DBG_PRINTF("Utf16leEncodingInit error!\n");
		return -1;
	}
	
	iError = Utf16beEncodingInit();
	if (iError)
	{
		DBG_PRINTF("Utf16beEncodingInit error!\n");
		return -1;
	}
	
	iError = Utf8EncodingInit();
	if (iError)
	{
		DBG_PRINTF("Utf8EncodingInit error!\n");
		return -1;
	}

	return 0;
}

