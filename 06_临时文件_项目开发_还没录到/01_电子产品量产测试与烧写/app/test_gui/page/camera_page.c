
#include <config.h>
#include <render.h>
#include <stdlib.h>
#include <unistd.h>
#include <input_manager.h>
#include <serial.h>
 


static pthread_t g_tCameraTestTreadID;


/**********************************************************************
 * �������ƣ� CameraPageGetInputEvent
 * ���������� Ϊ"��ҳ��"�����������,�ж������¼�λ����һ��ͼ����
 * ��������� ptPageLayout - �ں����ͼ�����ʾ����
 * ��������� ptInputEvent - �ں��õ�����������
 * �� �� ֵ�� -1     - �������ݲ�λ���κ�һ��ͼ��֮��
 *            ����ֵ - �������������ڵ�ͼ��(PageLayout->atLayout�������һ��)
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static int CameraPageGetInputEvent(PT_PageLayout ptPageLayout, PT_InputEvent ptInputEvent)
{
	return GenericGetInputEvent(ptPageLayout, ptInputEvent);
}

static void *CameraTestTreadFunction(void *pVoid)
{
	int oldstate;
	int oldtype;

	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldstate);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldtype);

	while (1)
	{
		system("video2lcd /dev/video0");
		sleep(1);
	}

	return NULL;
}

static void CreateThreadForCameraTest(void)
{
	/* ����netprint�����߷�: �������տ�����Ϣ,�����޸Ĵ�ӡ����,��/�رմ�ӡ */
	pthread_create(&g_tCameraTestTreadID, NULL, CameraTestTreadFunction, NULL);			
}

static void KillThreadForCameraTest(void)
{
	pthread_cancel(g_tCameraTestTreadID);
}

/**********************************************************************
 * �������ƣ� CameraPageRun
 * ���������� "��ҳ��"�����к���: ��ʾ�˵�ͼ��,�����û�����������Ӧ
 * ��������� ptParentPageParams - δ��
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static void CameraPageRun(PT_PageParams ptParentPageParams)
{
	int iIndex = -1;
	T_InputEvent tInputEvent;
    int bTestThreadRunning = 0;

	printf("CameraPageRun\n");

	if (ptParentPageParams)
		iIndex = (int)ptParentPageParams;
	
	/* 1. ��ʾҳ�� */

	/* 2. ����Prepare�߳� */

	/* 3. ����GetInputEvent��������¼����������� */
	while (1)
	{
        switch (iIndex) 
        {
            case EVENT_CAMERA_TEST:
            {
                if (bTestThreadRunning)
                {
                    KillThreadForCameraTest();
                    bTestThreadRunning = 0;
                }
				CreateThreadForCameraTest();
				bTestThreadRunning = 1;
                break;
            }

            case EVENT_KILL :
            case EVENT_EXIT:
            {
                if (bTestThreadRunning)
                {
                    KillThreadForCameraTest();
                    bTestThreadRunning = 0;
                }
                return ;
                break;
            }

            default:
            {
                break;
            }

        }

		iIndex = CameraPageGetInputEvent(NULL, &tInputEvent);				
	}
}

static T_PageAction g_tCameraPageAction = {
	.name          = "camera",
	.Run           = CameraPageRun,
	.GetInputEvent = CameraPageGetInputEvent,
	//.Prepare       = CameraPagePrepare;
};


/**********************************************************************
 * �������ƣ� CameraPageInit
 * ���������� ע��"��ҳ��"
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int CameraPageInit(void)
{
	return RegisterPageAction(&g_tCameraPageAction);
}

