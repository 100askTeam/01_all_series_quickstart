
#include <config.h>
#include <input_manager.h>
#include <string.h>

#define INPUT_EVENT_BUF_SIZE 1024
#define NEXT_BUFFER(w) ((w + 1) % INPUT_EVENT_BUF_SIZE)

static T_InputEvent g_atInputEvent[INPUT_EVENT_BUF_SIZE];
static int g_iRead;
static int g_iWrite;


static PT_InputOpr g_ptInputOprHead;

static pthread_mutex_t g_tMutex  = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  g_tConVar = PTHREAD_COND_INITIALIZER;

static int isEventBufferFull(void)
{
	return (NEXT_BUFFER(g_iWrite) == g_iRead);
}

static int isEventBufferEmpty(void)
{
	return (g_iWrite == g_iRead);
}

static void PutEventBuffer(PT_InputEvent pInputEvent)
{
	if (!isEventBufferFull())
	{
		//g_atInputEvent[g_iWrite] = *pInputEvent;
		memcpy(&g_atInputEvent[g_iWrite], pInputEvent, INPUT_EVENT_STR_OFFSET);
		if (pInputEvent->iType == INPUT_TYPE_NET)
			strcpy(g_atInputEvent[g_iWrite].aStr, pInputEvent->aStr);
		g_iWrite = NEXT_BUFFER(g_iWrite);
	}
}

static PT_InputEvent GetEventBuffer(void)
{
	PT_InputEvent pInputEvent;
	
	if (isEventBufferEmpty())
		return NULL;
	else
	{
		pInputEvent = &g_atInputEvent[g_iRead];
		g_iRead = NEXT_BUFFER(g_iRead);
		return pInputEvent;
	}
}

/**********************************************************************
 * �������ƣ� RegisterInputOpr
 * ���������� ע��"����ģ��"
 * ��������� ptInputOpr - ����ģ��Ľṹ��ָ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int RegisterInputOpr(PT_InputOpr ptInputOpr)
{
	PT_InputOpr ptTmp;

	if (!g_ptInputOprHead)
	{
		g_ptInputOprHead   = ptInputOpr;
		ptInputOpr->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptInputOprHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = ptInputOpr;
		ptInputOpr->ptNext = NULL;
	}

	return 0;
}


/**********************************************************************
 * �������ƣ� ShowInputOpr
 * ���������� ��ʾ��������֧�ֵ�"����ģ��"
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
void ShowInputOpr(void)
{
	int i = 0;
	PT_InputOpr ptTmp = g_ptInputOprHead;

	while (ptTmp)
	{
		printf("%02d %s\n", i++, ptTmp->name);
		ptTmp = ptTmp->ptNext;
	}
}

/**********************************************************************
 * �������ƣ� InputEventThreadFunction
 * ���������� "����ģ��"���̺߳���,ÿ������ģ�鶼��ͨ���������߳�����ȡ��������,
 *            �������ݺ����ỽ�ѵȵ����ݵ������߳�
 * ��������� pVoid - ����ģ���"���������ݺ���"
 * ��������� ��
 * �� �� ֵ�� NULL - �����˳�
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static void *InputEventThreadFunction(void *pVoid)
{
	T_InputEvent tInputEvent;
	
	/* ���庯��ָ�� */
	int (*GetInputEvent)(PT_InputEvent ptInputEvent);
	GetInputEvent = (int (*)(PT_InputEvent))pVoid;

	while (1)
	{
		//DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
		if(0 == GetInputEvent(&tInputEvent))
		{
			/* �������߳�, ��tInputEvent��ֵ���뻽�ѻ����� */
			/* �����ٽ���Դǰ���Ȼ�û����� */
			pthread_mutex_lock(&g_tMutex);

			//DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
			PutEventBuffer(&tInputEvent);
			// g_tInputEvent = tInputEvent;

			/*  �������߳� */
			pthread_cond_signal(&g_tConVar);

			/* �ͷŻ����� */
			pthread_mutex_unlock(&g_tMutex);
			//DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
		}
		//DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	}

	return NULL;
}

/**********************************************************************
 * �������ƣ� AllInputDevicesInit
 * ���������� ��������"����ģ��"���豸��صĳ�ʼ������
 *            ���������ڶ�ȡ�������ݵ����߳�
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int AllInputDevicesInit(void)
{
	PT_InputOpr ptTmp = g_ptInputOprHead;
	int iError = -1;

	while (ptTmp)
	{
		if (0 == ptTmp->DeviceInit())
		{
			/* �������߳� */
			pthread_create(&ptTmp->tTreadID, NULL, InputEventThreadFunction, ptTmp->GetInputEvent);			
			iError = 0;
		}
		ptTmp = ptTmp->ptNext;
	}
	return iError;
}

/**********************************************************************
 * �������ƣ� GetInputEvent
 * ���������� �����������,����ʹ�õ�ǰ�߳�����,
 *            ��������ģ������̶߳������ݺ���������
 * ��������� ��
 * ��������� ptInputEvent - �ں��õ�����������
 * �� �� ֵ�� 0 - �ɹ�
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int GetInputEvent(PT_InputEvent ptInputEvent)
{
	PT_InputEvent p = NULL;

	while (!p)
	{
		/* ���� */
		pthread_mutex_lock(&g_tMutex);
		p = GetEventBuffer();
		DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

		if (!p) 
		{
			DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
			pthread_cond_wait(&g_tConVar, &g_tMutex);	
			/* �����Ѻ�,�������� */ 
			DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
			p = GetEventBuffer();
			if (p)
				*ptInputEvent = *p;
			pthread_mutex_unlock(&g_tMutex);
		}
		else
		{
			DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
			*ptInputEvent = *p;
			pthread_mutex_unlock(&g_tMutex);
			break;
		}

	}
	
	return 0;	
}

/**********************************************************************
 * �������ƣ� InputInit
 * ���������� ���ø�������ģ��ĳ�ʼ������,����ע���������ģ��
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int InputInit(void)
{
	int iError = 0;
	// iError = StdinInit();
	iError |= TouchScreenInit();
	iError |= NetInputInit();
	return iError;
}

