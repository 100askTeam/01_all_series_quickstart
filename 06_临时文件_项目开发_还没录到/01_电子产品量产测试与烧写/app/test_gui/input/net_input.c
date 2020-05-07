#include <config.h>
#include <input_manager.h>
#include <disp_manager.h>
#include <stdlib.h>
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

static int g_iSocketInputServer;
static struct sockaddr_in g_tSocketInputServerAddr;

#define INPUT_SERVER_PORT 8765

static char g_strInputBuf[1000];

/**********************************************************************
 * �������ƣ� NetInputRecvTreadFunction
 * ���������� "�����������ͨ��"�Ľ����̺߳���
 *            ���տͻ��˷���������
 * ��������� pVoid - δ��
 * ��������� ��
 * �� �� ֵ�� NULL - �߳��˳�
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static int NetInputGetRecv(void)
{
    socklen_t iAddrLen;
    int iRecvLen;
    struct sockaddr_in tSocketClientAddr;

	/* ��ȡ�ͻ��˷��������� */
	iAddrLen = sizeof(struct sockaddr);
	DBG_PRINTF("in NetInputGetRecv\n");
	iRecvLen = recvfrom(g_iSocketInputServer, g_strInputBuf, 999, 0, (struct sockaddr *)&tSocketClientAddr, &iAddrLen);
	
	if (iRecvLen > 0)	
	{
		g_strInputBuf[iRecvLen] = '\0';
		DBG_PRINTF("NetInputGetRecv get msg: %s\n", g_strInputBuf);
        return 0;
	}
    else
    {
        return -1;
    }
	
}

/**********************************************************************
 * �������ƣ� NetInputDevInit
 * ���������� ��ʼ������
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static int NetInputDevInit(void)
{
	/* socket��ʼ�� */
	int iRet;
	
	g_iSocketInputServer = socket(AF_INET, SOCK_DGRAM, 0);
	if (-1 == g_iSocketInputServer)
	{
		printf("socket error!\n");
		return -1;
	}

	g_tSocketInputServerAddr.sin_family      = AF_INET;
	g_tSocketInputServerAddr.sin_port        = htons(INPUT_SERVER_PORT);  /* host to net, short */
 	g_tSocketInputServerAddr.sin_addr.s_addr = INADDR_ANY;
	memset(g_tSocketInputServerAddr.sin_zero, 0, 8);
	
	iRet = bind(g_iSocketInputServer, (const struct sockaddr *)&g_tSocketInputServerAddr, sizeof(struct sockaddr));
	if (-1 == iRet)
	{
		printf("bind error!\n");
		return -1;
	}
    
	return 0;	
}

/**********************************************************************
 * �������ƣ� StdinDevExit
 * ���������� ����������ģ����豸�˳�����
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static int NetInputDevExit(void)
{
	return 0;
}


/**********************************************************************
 * �������ƣ� NetInputGetInputEvent
 * ���������� ��ȡ�������ݲ�����Ϊһ��ָ��
 * ��������� ��
 * ��������� ptInputEvent - �ں��õ�����������
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static int NetInputGetInputEvent(PT_InputEvent ptInputEvent)
{
	int iRet;
	
	while (1)
	{
		iRet = NetInputGetRecv();
		if (!iRet)
		{
			ptInputEvent->iType = INPUT_TYPE_NET;
			strncpy(ptInputEvent->aStr, g_strInputBuf, 1000);
			return 0;
		}
		else
		{
			return -1;
		}
	}

	return 0;
}


static T_InputOpr g_tNetInputOpr = {
	.name          = "netinput",
	.DeviceInit    = NetInputDevInit,
	.DeviceExit    = NetInputDevExit,
	.GetInputEvent = NetInputGetInputEvent,
};

/**********************************************************************
 * �������ƣ� NetInputInit
 * ���������� ע��"����������ģ��"
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int NetInputInit(void)
{
	return RegisterInputOpr(&g_tNetInputOpr);
}

