
#include <config.h>
#include <render.h>
#include <stdlib.h>
#include <unistd.h>
#include <input_manager.h>
#include <serial.h>
 

static T_Layout g_atOpenJTAGPageIconsLayout[] = {
#if 0
	{0, 0, 0, 0, "browse_mode.bmp"},
	{0, 0, 0, 0, "continue_mod.bmp"},
	{0, 0, 0, 0, "setting.bmp"},
#else
	{0, 0, 0, 0, "openjtag_burn.bmp"},
	{0, 0, 0, 0, "openjtag_test_jtag.bmp"},
	{0, 0, 0, 0, "openjtag_test_serial.bmp"},
#endif
	{0, 0, 0, 0, NULL},
};

static T_PageLayout g_tOpenJTAGPageLayout = {
	.iMaxTotalBytes = 0,
	.atLayout       = g_atOpenJTAGPageIconsLayout,
};

static pthread_t g_tJTAGTestTreadID;
static pthread_t g_tSerialTestTreadID;


/**********************************************************************
 * 函数名称： CalcOpenJTAGPageLayout
 * 功能描述： 计算页面中各图标座标值
 * 输入参数： 无
 * 输出参数： ptPageLayout - 内含各图标的左上角/右下角座标值
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static void  CalcOpenJTAGPageLayout(PT_PageLayout ptPageLayout)
{
	int iStartY;
	int iWidth;
	int iHeight;
	int iXres, iYres, iBpp;
	int iTmpTotalBytes;
	PT_Layout atLayout;

	atLayout = ptPageLayout->atLayout;
	GetDispResolution(&iXres, &iYres, &iBpp);
	ptPageLayout->iBpp = iBpp;

	/*   
	 *    ----------------------
	 *                           1/2 * iHeight
	 *          openjtag_burn.bmp  iHeight
	 *                           1/2 * iHeight
	 *         openjtag_test_jtag.bmp     iHeight
	 *                           1/2 * iHeight
	 *          openjtag_test_serial.bmp       iHeight
	 *                           1/2 * iHeight
	 *    ----------------------
	 */
	 
	iHeight = iYres * 2 / 10;
	iWidth  = iHeight;
	iStartY = iHeight / 2;
	
	/* openjtag_burn图标 */
	atLayout[0].iTopLeftY  = iStartY;
	atLayout[0].iBotRightY = atLayout[0].iTopLeftY + iHeight - 1;
	atLayout[0].iTopLeftX  = (iXres - iWidth * 2) / 2;
	atLayout[0].iBotRightX = atLayout[0].iTopLeftX + iWidth * 2 - 1;

	iTmpTotalBytes = (atLayout[0].iBotRightX - atLayout[0].iTopLeftX + 1) * (atLayout[0].iBotRightY - atLayout[0].iTopLeftY + 1) * iBpp / 8;
	if (ptPageLayout->iMaxTotalBytes < iTmpTotalBytes)
	{
		ptPageLayout->iMaxTotalBytes = iTmpTotalBytes;
	}


	/* openjtag_test_jtag图标 */
	atLayout[1].iTopLeftY  = atLayout[0].iBotRightY + iHeight / 2 + 1;
	atLayout[1].iBotRightY = atLayout[1].iTopLeftY + iHeight - 1;
	atLayout[1].iTopLeftX  = (iXres - iWidth * 2) / 2;
	atLayout[1].iBotRightX = atLayout[1].iTopLeftX + iWidth * 2 - 1;

	iTmpTotalBytes = (atLayout[1].iBotRightX - atLayout[1].iTopLeftX + 1) * (atLayout[1].iBotRightY - atLayout[1].iTopLeftY + 1) * iBpp / 8;
	if (ptPageLayout->iMaxTotalBytes < iTmpTotalBytes)
	{
		ptPageLayout->iMaxTotalBytes = iTmpTotalBytes;
	}

	/* openjtag_test_serial图标 */
	atLayout[2].iTopLeftY  = atLayout[1].iBotRightY + iHeight / 2 + 1;
	atLayout[2].iBotRightY = atLayout[2].iTopLeftY + iHeight - 1;
	atLayout[2].iTopLeftX  = (iXres - iWidth * 2) / 2;
	atLayout[2].iBotRightX = atLayout[2].iTopLeftX + iWidth * 2 - 1;

	iTmpTotalBytes = (atLayout[2].iBotRightX - atLayout[2].iTopLeftX + 1) * (atLayout[2].iBotRightY - atLayout[2].iTopLeftY + 1) * iBpp / 8;
	if (ptPageLayout->iMaxTotalBytes < iTmpTotalBytes)
	{
		ptPageLayout->iMaxTotalBytes = iTmpTotalBytes;
	}

}


/**********************************************************************
 * 函数名称： OpenJTAGPageGetInputEvent
 * 功能描述： 为"主页面"获得输入数据,判断输入事件位于哪一个图标上
 * 输入参数： ptPageLayout - 内含多个图标的显示区域
 * 输出参数： ptInputEvent - 内含得到的输入数据
 * 返 回 值： -1     - 输入数据不位于任何一个图标之上
 *            其他值 - 输入数据所落在的图标(PageLayout->atLayout数组的哪一项)
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int OpenJTAGPageGetInputEvent(PT_PageLayout ptPageLayout, PT_InputEvent ptInputEvent)
{
	return GenericGetInputEvent(ptPageLayout, ptInputEvent);
}


/**********************************************************************
 * 函数名称： ShowOpenJTAGPage
 * 功能描述： 显示"主页面"
 * 输入参数： ptPageLayout - 内含多个图标的文件名和显示区域
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static void ShowOpenJTAGPage(PT_PageLayout ptPageLayout)
{
	PT_VideoMem ptVideoMem;
	int iError;

	PT_Layout atLayout = ptPageLayout->atLayout;
		
	/* 1. 获得显存 */
	ptVideoMem = GetVideoMem(ID("openjtag"), 1);
	if (ptVideoMem == NULL)
	{
		DBG_PRINTF("can't get video mem for main page!\n");
		return;
	}

	/* 2. 描画数据 */

	/* 如果还没有计算过各图标的坐标 */
	if (atLayout[0].iTopLeftX == 0)
	{
		CalcOpenJTAGPageLayout(ptPageLayout);
	}

	iError = GeneratePage(ptPageLayout, ptVideoMem);	

	/* 3. 刷到设备上去 */
	FlushVideoMemToDev(ptVideoMem);

	/* 4. 解放显存 */
	PutVideoMem(ptVideoMem);
}

static void *JTAGTestTreadFunction(void *pVoid)
{
	int oldstate;
	int oldtype;
	int iRet;

    PT_VideoMem ptDevVideoMem;

	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldstate);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldtype);

    ptDevVideoMem = GetDevVideoMem();

	while (1)
	{
		//printf("JTAGTestTreadFunction, run openocd.sh\n");
		system("openocd.sh > /tmp/test.log 2>&1");
		iRet = system("grep 12345678 /tmp/test.log");

		if (iRet) 		
		{
			system("openocd_s3c2440.sh > /tmp/test.log 2>&1");
			iRet = system("grep 12345678 /tmp/test.log");
		}
		
		if (!iRet)
		{
            GenerateItemForPage("test_jtag_ok.bmp", &g_tOpenJTAGPageLayout, 1, ptDevVideoMem);
		}
		else
		{
            GenerateItemForPage("test_jtag_fail.bmp", &g_tOpenJTAGPageLayout, 1, ptDevVideoMem);
		}
		//sleep(2);
	}
	
	return NULL;
}

void SerialTestTreadCleanupPush(void *arg)
{
	int fd = *((int *)arg);
	if (fd >= 0)
		close(fd);
}

static void *SerialTestTreadFunction(void *pVoid)
{
	int oldstate;
	int oldtype;
	int iRet;
	int fd = -1;
	unsigned char c;
	int ok_cnt = 0;
	int err_cnt = 0;

    PT_VideoMem ptDevVideoMem;

	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldstate);
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &oldtype);
	pthread_cleanup_push(SerialTestTreadCleanupPush, &fd);

    ptDevVideoMem = GetDevVideoMem();

	while (1)
	{
		while (fd < 0)
		{
			fd = open_port("/dev/ttyUSB0");
			if (fd < 0)
			{
				printf("open port err!\n");
				goto test_serial_err;
			}
			iRet = set_opt(fd, 115200, 8, 'N', 1);
			if (iRet)
			{
				printf("set port err!\n");
				goto test_serial_err;
			}
			sleep(1);
		}

		c = 0x5A;
		write(fd, &c, 1);
		iRet = read(fd, &c, 1);
		if (iRet == 1)
		{
			if (c == 0x5A)
				ok_cnt++;
			else
			{
				printf("%02x ", c);
				ok_cnt = 0;
			}
		}
		else
		{
			err_cnt++;
		}

		if (ok_cnt >= 100)
		{
			ok_cnt = 0;
			err_cnt = 0;
	        GenerateItemForPage("test_serial_ok.bmp", &g_tOpenJTAGPageLayout, 2, ptDevVideoMem);	
		}

		if (err_cnt >= 100)
		{
			ok_cnt = 0;
			err_cnt = 0;
	        GenerateItemForPage("test_serial_fail.bmp", &g_tOpenJTAGPageLayout, 2, ptDevVideoMem);	
		}
		
		pthread_testcancel();
		continue;
test_serial_err:
        GenerateItemForPage("test_serial_fail.bmp", &g_tOpenJTAGPageLayout, 2, ptDevVideoMem);	
	}

	pthread_cleanup_pop(0);
	return NULL;
}

static void CreateThreadForOpenJTAGTest(void)
{
	/* 创建netprint发送线程: 它用来发送打印信息给客户端 */
	pthread_create(&g_tJTAGTestTreadID, NULL, JTAGTestTreadFunction, NULL);			
	
	/* 创建netprint接收线否: 用来接收控制信息,比如修改打印级别,打开/关闭打印 */
	pthread_create(&g_tSerialTestTreadID, NULL, SerialTestTreadFunction, NULL);			
}

static void KillThreadForOpenJTAGTest(void)
{
	pthread_cancel(g_tJTAGTestTreadID);
	pthread_cancel(g_tSerialTestTreadID);
}

/**********************************************************************
 * 函数名称： OpenJTAGPageRun
 * 功能描述： "主页面"的运行函数: 显示菜单图标,根据用户输入作出反应
 * 输入参数： ptParentPageParams - 未用
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static void OpenJTAGPageRun(PT_PageParams ptParentPageParams)
{
	int iIndex = -1;
	T_InputEvent tInputEvent;
    T_PageParams tPageParams;
    int iRet;
    PT_VideoMem ptDevVideoMem;
    int bTestThreadRunning = 0;

    tPageParams.iPageID = ID("openjtag");

    ptDevVideoMem = GetDevVideoMem();

	if (ptParentPageParams)
		iIndex = (int)ptParentPageParams;
	
	/* 1. 显示页面 */
	ShowOpenJTAGPage(&g_tOpenJTAGPageLayout);

	/* 2. 创建Prepare线程 */

	/* 3. 调用GetInputEvent获得输入事件，进而处理 */
	while (1)
	{
        switch (iIndex) 
        {
            case EVENT_OPENJTAG_BURN :
            {
                if (bTestThreadRunning)
                {
                    KillThreadForOpenJTAGTest();
                    bTestThreadRunning = 0;
                }
                
                iRet = system("eeprom -w");
                DBG_PRINTF("run eeprom -w\n");
                if (iRet == 0)
                {
                    DBG_PRINTF("eeprom ok\n");
                    /* 在LCD上显示"烧写...成功" */
                    GenerateItemForPage("openjtag_burn_ok.bmp", &g_tOpenJTAGPageLayout, 0, ptDevVideoMem);
                }
                else
                {
                    DBG_PRINTF("eeprom fail\n");
                    /* 在LCD上显示"烧写...失败" */
                    GenerateItemForPage("openjtag_burn_fail.bmp", &g_tOpenJTAGPageLayout, 0, ptDevVideoMem);
                }
                break;
            }

            case EVENT_OPENJTAG_TEST:
            {
				printf("CreateThreadForOpenJTAGTest\n");

                CreateThreadForOpenJTAGTest();
                bTestThreadRunning = 1;               
                break;
            }

            case EVENT_KILL :
            {
                if (bTestThreadRunning)
                {
                    KillThreadForOpenJTAGTest();
                    bTestThreadRunning = 0;
                }
                break;
            }
            
            case EVENT_EXIT:
            {
                if (bTestThreadRunning)
                {
                    KillThreadForOpenJTAGTest();
                    bTestThreadRunning = 0;
                }
                DBG_PRINTF("********EXIT********\n");
                return ;
                break;
            }

            default:
            {
                break;
            }

        }

		iIndex = OpenJTAGPageGetInputEvent(&g_tOpenJTAGPageLayout, &tInputEvent);
		printf("OpenJTAGPageGetInputEvent, iIndex = %d\n", iIndex);
				
	}
}

static T_PageAction g_tOpenJTAGPageAction = {
	.name          = "openjtag",
	.Run           = OpenJTAGPageRun,
	.GetInputEvent = OpenJTAGPageGetInputEvent,
	//.Prepare       = OpenJTAGPagePrepare;
};


/**********************************************************************
 * 函数名称： OpenJTAGPageInit
 * 功能描述： 注册"主页面"
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int OpenJTAGPageInit(void)
{
	return RegisterPageAction(&g_tOpenJTAGPageAction);
}

