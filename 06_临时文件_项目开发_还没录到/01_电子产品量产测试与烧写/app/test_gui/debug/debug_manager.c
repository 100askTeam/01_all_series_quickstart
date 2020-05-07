#include <config.h>
#include <debug_manager.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

static PT_DebugOpr g_ptDebugOprHead;
static int g_iDbgLevelLimit = 8;

/**********************************************************************
 * �������ƣ� RegisterDispOpr
 * ���������� ע��"����ͨ��", ��PT_DebugOpr�ṹ�����������
 * ��������� ptDebugOpr - һ���ṹ��,��ʾ����ͨ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int RegisterDebugOpr(PT_DebugOpr ptDebugOpr)
{
	PT_DebugOpr ptTmp;

	if (!g_ptDebugOprHead)
	{
		g_ptDebugOprHead   = ptDebugOpr;
		ptDebugOpr->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptDebugOprHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = ptDebugOpr;
		ptDebugOpr->ptNext = NULL;
	}

	return 0;
}

/**********************************************************************
 * �������ƣ� ShowDebugOpr
 * ���������� ��ʾ��������֧�ֵ�"����ģ��"
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
void ShowDebugOpr(void)
{
	int i = 0;
	PT_DebugOpr ptTmp = g_ptDebugOprHead;

	while (ptTmp)
	{
		DBG_PRINTF("%02d %s\n", i++, ptTmp->name);
		ptTmp = ptTmp->ptNext;
	}
}

/**********************************************************************
 * �������ƣ� GetDebugOpr
 * ���������� ��������ȡ��ָ����"����ģ��"
 * ��������� pcName - ����
 * ��������� ��
 * �� �� ֵ�� NULL   - ʧ��,û��ָ����ģ��, 
 *            ��NULL - ��ʾģ���PT_DebugOpr�ṹ��ָ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
PT_DebugOpr GetDebugOpr(char *pcName)
{
	PT_DebugOpr ptTmp = g_ptDebugOprHead;
	
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
 * �������ƣ� SetDbgLevel
 * ���������� ���ô�ӡ����g_iDbgLevelLimit: ����Χ0~7, ����ԽС����Խ��
 *            ���ڻ����g_iDbgLevelLimit�ĵ�����Ϣ�Ż��ӡ����
 * ��������� strBuf - ����"dbglevel=3"
 * ��������� ��
 * �� �� ֵ�� 0   - �ɹ�
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int SetDbgLevel(char *strBuf)
{
	g_iDbgLevelLimit = strBuf[9] - '0';
	return 0;
}

/**********************************************************************
 * �������ƣ� SetDbgChanel
 * ���������� ��/��ĳ������ͨ��
 * ��������� strBuf - �����������ַ���
 *                     stdout=0			   : �ر�stdout��ӡ
 *                     stdout=1			   : ��stdout��ӡ
 *                     netprint=0		   : �ر�netprint��ӡ
 *                     netprint=1		   : ��netprint��ӡ
 * ��������� ��
 * �� �� ֵ�� 0   - �ɹ�
 *            -1  - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int SetDbgChanel(char *strBuf)
{
	char *pStrTmp;
	char strName[100];
	PT_DebugOpr ptTmp;

	pStrTmp = strchr(strBuf, '=');
	if (!pStrTmp)
	{
		return -1;
	}
	else
	{
		/* �Ƚ�ȡ����, Ȼ����������ҵ�����ģ�� */
		strncpy(strName, strBuf, pStrTmp-strBuf);
		strName[pStrTmp-strBuf] = '\0';
		ptTmp = GetDebugOpr(strName);
		if (!ptTmp)
			return -1;

		if (pStrTmp[1] == '0')
			ptTmp->isCanUse = 0;
		else
			ptTmp->isCanUse = 1;
		return 0;
	}
	
}

/**********************************************************************
 * �������ƣ� DebugPrint
 * ���������� ��ӡ��Ϣ������ں���
 *            ��������DBG_PRINTF����ӡ, ������DebugPrint
 *            ��config.h���������ĺ궨��: #define DBG_PRINTF DebugPrint
 * ��������� �ɱ����,�÷���printf��ȫһ��
 * ��������� ��
 * �� �� ֵ�� 0   - �ɹ�
 *            -1  - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int DebugPrint(const char *pcFormat, ...)
{
	char strTmpBuf[1000];
	char *pcTmp;
	va_list tArg;
	int iNum;
	PT_DebugOpr ptTmp = g_ptDebugOprHead;
	int dbglevel = DEFAULT_DBGLEVEL;

	/* �ɱ�����Ĵ���, ����glibc��printf���� */
	va_start (tArg, pcFormat);
	iNum = vsprintf (strTmpBuf, pcFormat, tArg);
	va_end (tArg);
	strTmpBuf[iNum] = '\0';


	pcTmp = strTmpBuf;
	
	/* ���ݴ�ӡ��������Ƿ��ӡ */
	if ((strTmpBuf[0] == '<') && (strTmpBuf[2] == '>'))
	{
		dbglevel = strTmpBuf[1] - '0';
		if (dbglevel >= 0 && dbglevel <= 9)
		{
			pcTmp = strTmpBuf + 3;
		}
		else
		{
			dbglevel = DEFAULT_DBGLEVEL;
		}
	}

	if (dbglevel > g_iDbgLevelLimit)
	{
		return -1;
	}

	/* ��������������isCanUseΪ1�Ľṹ���DebugPrint���� 
	 * �������������Ϣ
	 */
	while (ptTmp)
	{
		if (ptTmp->isCanUse)
		{
			ptTmp->DebugPrint(pcTmp);
		}
		ptTmp = ptTmp->ptNext;
	}

	return 0;
	
}

/**********************************************************************
 * �������ƣ� InitDebugChanel
 * ���������� ��Щ��ӡͨ����Ҫ����һЩ��ʼ��, ���������ӡ��Ҫ�󶨶˿ڵȵ�
 *            ����������ִ����Щ��ʼ��
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int InitDebugChanel(void)
{
	PT_DebugOpr ptTmp = g_ptDebugOprHead;
	while (ptTmp)
	{
		if (ptTmp->isCanUse && ptTmp->DebugInit)
		{
			ptTmp->DebugInit();
		}
		ptTmp = ptTmp->ptNext;
	}

	return 0;
}


/**********************************************************************
 * �������ƣ� DebugInit
 * ���������� ע�����ͨ��,���ǰ�PT_DebugOprע�ᵽ������
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int DebugInit(void)
{
	int iError;

	iError = StdoutInit();
//	iError |= NetPrintInit();
	return iError;
}

