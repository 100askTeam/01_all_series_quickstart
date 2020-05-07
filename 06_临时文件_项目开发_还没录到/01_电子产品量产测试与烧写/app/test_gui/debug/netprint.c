#include <config.h>
#include <debug_manager.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <pthread.h>

#define SERVER_PORT 5678
#define PRINT_BUF_SIZE   (16*1024)

static int g_iSocketServer;
static struct sockaddr_in g_tSocketServerAddr;
static struct sockaddr_in g_tSocketClientAddr;
static int g_iHaveConnected = 0;
static char *g_pcNetPrintBuf;
static int g_iReadPos  = 0;
static int g_iWritePos = 0;

static pthread_t g_tSendTreadID;
static pthread_t g_tRecvTreadID;

static pthread_mutex_t g_tNetDbgSendMutex  = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  g_tNetDbgSendConVar = PTHREAD_COND_INITIALIZER;

/* Ҫͨ�������ӡ����Ϣ���ڻ��ͻ�������,
 * ���пͻ��������ϱ�����ʱ, �����̲߳Ŵӻ��ͻ�������ȡ�����ݷ��ͳ�ȥ
 */
 
/**********************************************************************
 * �������ƣ� isFull
 * ���������� �жϻ��ͻ������Ƿ���
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - δ��, 1 - ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static int isFull(void)
{
	return (((g_iWritePos + 1) % PRINT_BUF_SIZE) == g_iReadPos);
}

/**********************************************************************
 * �������ƣ� isEmpty
 * ���������� �жϻ��ͻ������Ƿ��
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - ����������, 1 - ������
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static int isEmpty(void)
{
	return (g_iWritePos == g_iReadPos);
}


/**********************************************************************
 * �������ƣ� PutData
 * ���������� �����ͻ������з�������
 * ��������� cVal - ����
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��(��������)
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static int PutData(char cVal)
{
	if (isFull())
		return -1;
	else
	{
		g_pcNetPrintBuf[g_iWritePos] = cVal;
		g_iWritePos = (g_iWritePos + 1) % PRINT_BUF_SIZE;
		return 0;
	}	
}

/**********************************************************************
 * �������ƣ� GetData
 * ���������� �ӻ��ͻ������л������
 * ��������� ��
 * ��������� pcVal - �����������õ�����
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��(������)
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static int GetData(char *pcVal)
{
	if (isEmpty())
		return -1;
	else
	{
		*pcVal = g_pcNetPrintBuf[g_iReadPos];
		g_iReadPos = (g_iReadPos + 1) % PRINT_BUF_SIZE;
		return 0;
	}
}


/**********************************************************************
 * �������ƣ� NetDbgSendTreadFunction
 * ���������� "�����������ͨ��"�ķ����̺߳���
 *            ���߳�ƽʱ����, ������Ҫ����ʱ������, Ȼ��ͨ����������ݷ��ͳ�ȥ
 * ��������� pVoid - δ��
 * ��������� ��
 * �� �� ֵ�� NULL - �߳��˳�
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static void *NetDbgSendTreadFunction(void *pVoid)
{
	char strTmpBuf[512];
	char cVal;
	int i;
	int iAddrLen;
	int iSendLen;
	
	while (1)
	{
		/* ƽʱ����, �����̵߳���DBG_PRINTF����ʱ, ������õ�g_tNetDbgOpr.DebugPrint, ���ỽ�ѱ��߳� */
		pthread_mutex_lock(&g_tNetDbgSendMutex);
		pthread_cond_wait(&g_tNetDbgSendConVar, &g_tNetDbgSendMutex);	
		pthread_mutex_unlock(&g_tNetDbgSendMutex);

		while (g_iHaveConnected && !isEmpty())
		{
			i = 0;

			/* �ѻ��λ�����������ȡ����, ���ȡ512�ֽ� */
			while ((i < 512) && (0 == GetData(&cVal)))
			{
				strTmpBuf[i] = cVal;
				i++;
			}
			
			/* ִ�е�����, ��ʾ������ */
			/* ��sendto�������ʹ�ӡ��Ϣ���ͻ��� */
			iAddrLen = sizeof(struct sockaddr);
			iSendLen = sendto(g_iSocketServer, strTmpBuf, i, 0,
			                      (const struct sockaddr *)&g_tSocketClientAddr, iAddrLen);

		}

	}

	iSendLen = iSendLen;
	return NULL;
}

/**********************************************************************
 * �������ƣ� NetDbgRecvTreadFunction
 * ���������� "�����������ͨ��"�Ľ����̺߳���
 *            ���տͻ��˷���������,��������"������Ϣ�����ĸ��ͻ���"/��ӡ����/�����ĳ������ͨ��
 * ��������� pVoid - δ��
 * ��������� ��
 * �� �� ֵ�� NULL - �߳��˳�
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static void *NetDbgRecvTreadFunction(void *pVoid)
{
	socklen_t iAddrLen;
	int iRecvLen;
	char ucRecvBuf[1000];
	struct sockaddr_in tSocketClientAddr;

	while (1)
	{
		/* ��ȡ�ͻ��˷��������� */
		iAddrLen = sizeof(struct sockaddr);
		DBG_PRINTF("in NetDbgRecvTreadFunction\n");
		iRecvLen = recvfrom(g_iSocketServer, ucRecvBuf, 999, 0, (struct sockaddr *)&tSocketClientAddr, &iAddrLen);
		
		if (iRecvLen > 0)	
		{
			ucRecvBuf[iRecvLen] = '\0';
			DBG_PRINTF("netprint.c get msg: %s\n", ucRecvBuf);
			
			/* ��������:
			 * setclient            : ���ý��մ�ӡ��Ϣ�Ŀͻ���
			 * dbglevel=0,1,2...    : �޸Ĵ�ӡ����
			 * stdout=0             : �ر�stdout��ӡ
			 * stdout=1             : ��stdout��ӡ
			 * netprint=0           : �ر�netprint��ӡ
			 * netprint=1           : ��netprint��ӡ
			 */
			if (strcmp(ucRecvBuf, "setclient")  == 0)
			{
				/* ������"setclient"�Ľ������ڽ��յ�����Ϣ */
				g_tSocketClientAddr = tSocketClientAddr;
				g_iHaveConnected = 1;
			}
			else if (strncmp(ucRecvBuf, "dbglevel=", 9) == 0)
			{
				/* ���ô�ӡ���� */
				SetDbgLevel(ucRecvBuf);
			}
			else
			{
				/* ��/�ش�ӡͨ�� */
				SetDbgChanel(ucRecvBuf);
			}
		}
		
	}
	return NULL;
}

/**********************************************************************
 * �������ƣ� NetDbgInit
 * ���������� "�����������ͨ��"�ĳ�ʼ������
 *            1. ���ö˿���Ϣ
 *            2. ����2�����߳�, һ���������տ�������, �����/�ر�ĳ����ӡͨ��, ���ô�ӡ����
 *               (��������������ӡͨ��: ��׼���,�����ӡ)
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static int NetDbgInit(void)
{
	/* socket��ʼ�� */
	int iRet;
	
	g_iSocketServer = socket(AF_INET, SOCK_DGRAM, 0);
	if (-1 == g_iSocketServer)
	{
		printf("socket error!\n");
		return -1;
	}

	g_tSocketServerAddr.sin_family      = AF_INET;
	g_tSocketServerAddr.sin_port        = htons(SERVER_PORT);  /* host to net, short */
 	g_tSocketServerAddr.sin_addr.s_addr = INADDR_ANY;
	memset(g_tSocketServerAddr.sin_zero, 0, 8);
	
	iRet = bind(g_iSocketServer, (const struct sockaddr *)&g_tSocketServerAddr, sizeof(struct sockaddr));
	if (-1 == iRet)
	{
		printf("bind error!\n");
		return -1;
	}

	g_pcNetPrintBuf = malloc(PRINT_BUF_SIZE);
	if (NULL == g_pcNetPrintBuf)
	{
		close(g_iSocketServer);
		return -1;
	}


	/* ����netprint�����߳�: ���������ʹ�ӡ��Ϣ���ͻ��� */
	pthread_create(&g_tSendTreadID, NULL, NetDbgSendTreadFunction, NULL);			
	
	/* ����netprint�����߷�: �������տ�����Ϣ,�����޸Ĵ�ӡ����,��/�رմ�ӡ */
	pthread_create(&g_tRecvTreadID, NULL, NetDbgRecvTreadFunction, NULL);			

	return 0;	
}

/**********************************************************************
 * �������ƣ� NetDbgExit
 * ���������� "�����������ͨ��"���˳�����
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static int NetDbgExit(void)
{
	/* �ر�socket,... */
	close(g_iSocketServer);
	free(g_pcNetPrintBuf);
	return 0;
}

/**********************************************************************
 * �������ƣ� StdoutDebugPrint
 * ���������� "�����������ͨ��"���������, ��ֻ�ǰ�Ҫ�������Ϣ���뻺����,
 *            Ȼ��������߳�
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� �����Ϣ�ĳ���
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static int NetDbgPrint(char *strData)
{
	/* �����ݷ��뻷�λ����� */
	int i;
	
	for (i = 0; i < strlen(strData); i++)
	{
		if (0 != PutData(strData[i]))
			break;
	}
	
	/* ����Ѿ��пͻ���������, �Ͱ�����ͨ�����緢�͸��ͻ��� */
	/* ����netprint�ķ����߳� */
	pthread_mutex_lock(&g_tNetDbgSendMutex);
	pthread_cond_signal(&g_tNetDbgSendConVar);
	pthread_mutex_unlock(&g_tNetDbgSendMutex);

	return i;
	
}


static T_DebugOpr g_tNetDbgOpr = {
	.name       = "netprint",
	.isCanUse   = 1,
	.DebugInit  = NetDbgInit,
	.DebugExit  = NetDbgExit,
	.DebugPrint = NetDbgPrint,
};

/**********************************************************************
 * �������ƣ� StdoutInit
 * ���������� ע��"�����������ͨ��", ��g_tNetDbgOpr�ṹ�����������
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int NetPrintInit(void)
{
	return RegisterDebugOpr(&g_tNetDbgOpr);
}

