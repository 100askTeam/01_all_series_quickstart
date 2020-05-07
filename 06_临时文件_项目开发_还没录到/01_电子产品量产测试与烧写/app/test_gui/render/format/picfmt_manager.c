
#include <config.h>
#include <pic_operation.h>
#include <picfmt_manager.h>
#include <string.h>


static PT_PicFileParser g_ptPicFileParserHead;

/**********************************************************************
 * �������ƣ� RegisterPicFileParser
 * ���������� ע��"ͼƬ�ļ�����ģ��", "ͼƬ�ļ�����ģ��"������ô��BMP/JPG��ͼƬ�ļ��н�������������
 * ��������� ptPicFileParser - һ���ṹ��,�ں�"ͼƬ�ļ�����ģ��"�Ĳ�������
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int RegisterPicFileParser(PT_PicFileParser ptPicFileParser)
{
	PT_PicFileParser ptTmp;

	if (!g_ptPicFileParserHead)
	{
		g_ptPicFileParserHead   = ptPicFileParser;
		ptPicFileParser->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptPicFileParserHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = ptPicFileParser;
		ptPicFileParser->ptNext = NULL;
	}

	return 0;
}


/**********************************************************************
 * �������ƣ� ShowPicFmts
 * ���������� ��ʾ��������֧�ֵ�"ͼƬ�ļ�����ģ��"
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
void ShowPicFmts(void)
{
	int i = 0;
	PT_PicFileParser ptTmp = g_ptPicFileParserHead;

	while (ptTmp)
	{
		printf("%02d %s\n", i++, ptTmp->name);
		ptTmp = ptTmp->ptNext;
	}
}

/**********************************************************************
 * �������ƣ� Parser
 * ���������� ��������ȡ��ָ����"ͼƬ�ļ�����ģ��"
 * ��������� pcName - ����
 * ��������� ��
 * �� �� ֵ�� NULL   - ʧ��,û��ָ����ģ��, 
 *            ��NULL - "ͼƬ�ļ�����ģ��"��PT_PicFileParser�ṹ��ָ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
PT_PicFileParser Parser(char *pcName)
{
	PT_PicFileParser ptTmp = g_ptPicFileParserHead;
	
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
 * �������ƣ� GetParser
 * ���������� �ҵ���֧��ָ���ļ���"ͼƬ�ļ�����ģ��"
 * ��������� ptFileMap - �ں��ļ���Ϣ
 * ��������� ��
 * �� �� ֵ�� NULL   - ʧ��,û��ָ����ģ��, 
 *            ��NULL - ֧�ָ��ļ���"ͼƬ�ļ�����ģ��"��PT_PicFileParser�ṹ��ָ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
PT_PicFileParser GetParser(PT_FileMap ptFileMap)
{
	PT_PicFileParser ptTmp = g_ptPicFileParserHead;
	
	while (ptTmp)
	{
		if (ptTmp->isSupport(ptFileMap))
		{
			return ptTmp;
		}
		ptTmp = ptTmp->ptNext;
	}
	return NULL;
}

/**********************************************************************
 * �������ƣ� PicFmtsInit
 * ���������� ���ø���"ͼƬ�ļ�����ģ��"�ĳ�ʼ������,����ע������
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int PicFmtsInit(void)
{
	int iError;

	iError = BMPParserInit();
//	iError |= JPGParserInit();
		
	return iError;
}




