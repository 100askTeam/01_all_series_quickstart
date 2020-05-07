
#include <config.h>
#include <render.h>
#include <stdlib.h>
#include <unistd.h>
#include <input_manager.h>
#include <serial.h>
 


static pthread_t g_tCameraTestTreadID;


/**********************************************************************
 * 函数名称： CameraPageGetInputEvent
 * 功能描述： 为"主页面"获得输入数据,判断输入事件位于哪一个图标上
 * 输入参数： ptPageLayout - 内含多个图标的显示区域
 * 输出参数： ptInputEvent - 内含得到的输入数据
 * 返 回 值： -1     - 输入数据不位于任何一个图标之上
 *            其他值 - 输入数据所落在的图标(PageLayout->atLayout数组的哪一项)
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
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
	/* 创建netprint接收线否: 用来接收控制信息,比如修改打印级别,打开/关闭打印 */
	pthread_create(&g_tCameraTestTreadID, NULL, CameraTestTreadFunction, NULL);			
}

static void KillThreadForCameraTest(void)
{
	pthread_cancel(g_tCameraTestTreadID);
}

/**********************************************************************
 * 函数名称： CameraPageRun
 * 功能描述： "主页面"的运行函数: 显示菜单图标,根据用户输入作出反应
 * 输入参数： ptParentPageParams - 未用
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static void CameraPageRun(PT_PageParams ptParentPageParams)
{
	int iIndex = -1;
	T_InputEvent tInputEvent;
    int bTestThreadRunning = 0;

	printf("CameraPageRun\n");

	if (ptParentPageParams)
		iIndex = (int)ptParentPageParams;
	
	/* 1. 显示页面 */

	/* 2. 创建Prepare线程 */

	/* 3. 调用GetInputEvent获得输入事件，进而处理 */
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
 * 函数名称： CameraPageInit
 * 功能描述： 注册"主页面"
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int CameraPageInit(void)
{
	return RegisterPageAction(&g_tCameraPageAction);
}

