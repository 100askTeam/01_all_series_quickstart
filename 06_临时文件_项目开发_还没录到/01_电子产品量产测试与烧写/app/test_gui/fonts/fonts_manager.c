#include <config.h>
#include <fonts_manager.h>
#include <string.h>

static PT_FontOpr g_ptFontOprHead = NULL;
static int g_dwFontSize;

/**********************************************************************
 * �������ƣ� RegisterFontOpr
 * ���������� ע��"����ģ��", ��ν����ģ�����ȡ���ַ�λͼ�ķ���
 * ��������� ptFontOpr - һ���ṹ��,�ں�"ȡ���ַ�λͼ"�Ĳ�������
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int RegisterFontOpr(PT_FontOpr ptFontOpr)
{
	PT_FontOpr ptTmp;

	if (!g_ptFontOprHead)
	{
		g_ptFontOprHead   = ptFontOpr;
		ptFontOpr->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptFontOprHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext     = ptFontOpr;
		ptFontOpr->ptNext = NULL;
	}

	return 0;
}


/**********************************************************************
 * �������ƣ� ShowFontOpr
 * ���������� ��ʾ��������֧�ֵ�"����ģ��"
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
void ShowFontOpr(void)
{
	int i = 0;
	PT_FontOpr ptTmp = g_ptFontOprHead;

	while (ptTmp)
	{
		printf("%02d %s\n", i++, ptTmp->name);
		ptTmp = ptTmp->ptNext;
	}
}

/**********************************************************************
 * �������ƣ� GetFontOpr
 * ���������� ��������ȡ��ָ����"����ģ��"
 * ��������� pcName - ����
 * ��������� ��
 * �� �� ֵ�� NULL   - ʧ��,û��ָ����ģ��, 
 *            ��NULL - ����ģ���PT_FontOpr�ṹ��ָ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
PT_FontOpr GetFontOpr(char *pcName)
{
	PT_FontOpr ptTmp = g_ptFontOprHead;
	
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
 * �������ƣ� SetFontSize
 * ���������� �����ַ��ĳߴ�(��λ:ɫ��)
 * ��������� dwFontSize - �ַ��ĳߴ�(��λ:ɫ��)
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
void SetFontSize(unsigned int dwFontSize)
{
	PT_FontOpr ptTmp = g_ptFontOprHead;
	
	g_dwFontSize = dwFontSize;

	while (ptTmp)
	{
		if (ptTmp->SetFontSize)
		{
			ptTmp->SetFontSize(dwFontSize);
		}
		ptTmp = ptTmp->ptNext;
	}
}

/**********************************************************************
 * �������ƣ� GetFontSize
 * ���������� ����ַ��ĳߴ�(��λ:ɫ��)
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� �ַ��ĳߴ�(��λ:ɫ��)
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
unsigned int GetFontSize(void)
{
	return g_dwFontSize;
}

/**********************************************************************
 * �������ƣ� GetFontBitmap
 * ���������� ����ַ���λͼ
 * ��������� dwCode       - �ַ��ı���ֵ(������ASCII/GBK/UNICODE)
 * ��������� ptFontBitMap - �ں�λͼ��Ϣ
 * �� �� ֵ�� 0  - �ɹ�
 *            -1 - ʧ�� 
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int GetFontBitmap(unsigned int dwCode, PT_FontBitMap ptFontBitMap)
{
	int iError = 0;
	PT_FontOpr ptTmp = g_ptFontOprHead;

	while (ptTmp)
	{
		iError = ptTmp->GetFontBitmap(dwCode, ptFontBitMap);
		if (0 == iError)
		{
			return 0;
		}
		ptTmp = ptTmp->ptNext;
	}
	return -1;
}


/**********************************************************************
 * �������ƣ� SetFontsDetail
 * ���������� ��������ģ�����Ϣ,����ָ���ֿ��ļ�,ָ���ַ��ߴ�
 * ��������� pcFontsName - Ҫ���õ�����ģ�������
 *            pcFontsFile - �ֿ��ļ�
 *            dwFontSize  - �ַ��ĳߴ�(��λ:ɫ��), ��������ʱ����SetFontSize�޸�
 * ��������� ��
 * �� �� ֵ�� 0      - �ɹ�
 *            ����ֵ - ʧ�� 
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int SetFontsDetail(char *pcFontsName, char *pcFontsFile, unsigned int dwFontSize)
{
	int iError = 0;
	PT_FontOpr ptFontOpr;

	ptFontOpr = GetFontOpr(pcFontsName);
	if (NULL == ptFontOpr)
	{
		return -1;
	}

	g_dwFontSize = dwFontSize;

	iError = ptFontOpr->FontInit(pcFontsFile, dwFontSize);
	
	return iError;
}


/**********************************************************************
 * �������ƣ� FontsInit
 * ���������� ���ø�������ģ��ĳ�ʼ������
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int FontsInit(void)
{

#if 0	
	int iError;

	iError = ASCIIInit();
	if (iError)
	{
		DBG_PRINTF("ASCIIInit error!\n");
		return -1;
	}

	iError = GBKInit();
	if (iError)
	{
		DBG_PRINTF("GBKInit error!\n");
		return -1;
	}
	iError = FreeTypeInit();
	if (iError)
	{
		DBG_PRINTF("FreeTypeInit error!\n");
		return -1;
	}
#endif	
	int iError;

	iError = FreeTypeInit();
	if (iError)
	{
		DBG_PRINTF("FreeTypeInit error!\n");
		return -1;
	}

	return 0;
}

