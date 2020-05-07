
#include <config.h>
#include <render.h>
#include <stdlib.h>
#include <math.h>
#include <test_items.h>
#include <string.h>
#include <fonts_manager.h>

#define X_GAP 5
#define Y_GAP 5

static T_Layout g_atMainPageIconsLayout[100];

static T_PageLayout g_tMainPageLayout = {
	.iMaxTotalBytes = 0,
	.atLayout       = g_atMainPageIconsLayout,
};

static void UpdateItemForTouch(PT_ItemCfg ptItemCfg);


/**********************************************************************
 * 函数名称： CalcMainPageLayoutForItemCfg
 * 功能描述： 计算页面中各图标座标值
 * 输入参数： 无
 * 输出参数： ptPageLayout - 内含各图标的左上角/右下角座标值
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static void  CalcMainPageLayoutForItemCfg(PT_PageLayout ptPageLayout)
{
//	int iStartY;
//	int iWidth;
//	int iHeight;
	int iXres, iYres, iBpp;
	int iTmpTotalBytes;
	PT_Layout atLayout;
	int n;
	int icon_width, icon_height;
	int i;
	int line = 0;
	int start_x, start_y;
	int item_count;
	PT_ItemCfg ptItemCfg;

	atLayout = ptPageLayout->atLayout;
	GetDispResolution(&iXres, &iYres, &iBpp);
	ptPageLayout->iBpp = iBpp;

	item_count = GetConfigNumber();

	n = sqrt(item_count) + 1;
	icon_width  = iXres/ n;
	icon_height = iYres / n;

	line = (item_count + n - 1) / n;
	start_x = (iXres - n * icon_width)/2;
	start_y = (iYres - line * icon_height)/2;	

	atLayout[0].iTopLeftX  = start_x;
	atLayout[0].iBotRightX = atLayout[0].iTopLeftX + icon_width - X_GAP;
	atLayout[0].iTopLeftY  = start_y;
	atLayout[0].iBotRightY = atLayout[0].iTopLeftY + icon_height - Y_GAP;

	line = 0;		
	for (i = 1; i < item_count; i++)
	{
		atLayout[i].iTopLeftX  = atLayout[i-1].iTopLeftX + icon_width;
		atLayout[i].iBotRightX = atLayout[i].iTopLeftX + icon_width - X_GAP;
		if (atLayout[i].iBotRightX >= iXres)
		{
			atLayout[i].iTopLeftX = start_x;
			atLayout[i].iBotRightX = atLayout[i].iTopLeftX + icon_width - X_GAP;
			line++;
		}
		
		atLayout[i].iTopLeftY  = icon_height * line + start_y;
		atLayout[i].iBotRightY = atLayout[i].iTopLeftY + icon_height - Y_GAP;
	}

	iTmpTotalBytes = (atLayout[0].iBotRightX - atLayout[0].iTopLeftX + 1) * (atLayout[0].iBotRightY - atLayout[0].iTopLeftY + 1) * iBpp / 8;
	if (ptPageLayout->iMaxTotalBytes < iTmpTotalBytes)
	{
		ptPageLayout->iMaxTotalBytes = iTmpTotalBytes;
	}

	for (i = 0; i < item_count; i++)
	{
		ptItemCfg = GetConfigByIndex(i);
		atLayout[i].strIconName = malloc(128);
	    snprintf(atLayout[i].strIconName, 128, "%s", ptItemCfg->name);
	    atLayout[i].strIconName[127] = '\0';
	}

	atLayout[i].strIconName = NULL;

}


/**********************************************************************
 * 函数名称： CalcMainPageLayout
 * 功能描述： 计算页面中各图标座标值
 * 输入参数： 无
 * 输出参数： ptPageLayout - 内含各图标的左上角/右下角座标值
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static void  CalcMainPageLayout(void)
{
	CalcMainPageLayoutForItemCfg(&g_tMainPageLayout);
}


/**********************************************************************
 * 函数名称： MainPageGetInputEvent
 * 功能描述： 为"主页面"获得输入数据,判断输入事件位于哪一个图标上
 * 输入参数： ptPageLayout - 内含多个图标的显示区域
 * 输出参数： ptInputEvent - 内含得到的输入数据
 * 返 回 值： -1     - 输入数据不位于任何一个图标之上
 *            其他值 - 输入数据所落在的图标(PageLayout->atLayout数组的哪一项)
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int MainPageGetInputEvent(PT_PageLayout ptPageLayout, PT_InputEvent ptInputEvent)
{
	return GenericGetInputEvent(ptPageLayout, ptInputEvent);
}


/**********************************************************************
 * 函数名称： ShowMainPage
 * 功能描述： 显示"主页面"
 * 输入参数： ptPageLayout - 内含多个图标的文件名和显示区域
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static void ShowMainPage(PT_PageLayout ptPageLayout)
{
	PT_VideoMem ptVideoMem;
	int iError;

	PT_Layout atLayout = ptPageLayout->atLayout;
		
	/* 1. 获得显存 */
	ptVideoMem = GetVideoMem(ID("main"), 1);
	if (ptVideoMem == NULL)
	{
		DBG_PRINTF("can't get video mem for main page!\n");
		return;
	}

	/* 2. 描画数据 */

	/* 如果还没有计算过各图标的坐标 */
	if (atLayout[0].iTopLeftX == 0)
	{
		CalcMainPageLayout();
	}

	iError = GenerateTextPage(ptPageLayout, ptVideoMem, 0x00FF0000);	
	iError = iError;

	/* 3. 刷到设备上去 */
	FlushVideoMemToDev(ptVideoMem);

	/* 4. 解放显存 */
	PutVideoMem(ptVideoMem);
}



/**********************************************************************
 * 函数名称： DrawIconWithString
 * 功能描述： 
 * 输入参数： 
 *            
 * 输出参数： 无
 * 返 回 值： 0      - 成功
 *            其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int DrawIconWithString(PT_ItemCfg ptItemCfg, char *str, unsigned int dwBackgroundColor)
{
	unsigned int dwFontSize;
	int iError;
	int index = ptItemCfg->index;
	int len = strlen(str);

    PT_VideoMem ptDevVideoMem = GetDevVideoMem();
	

	dwFontSize = g_atMainPageIconsLayout[index].iBotRightX - g_atMainPageIconsLayout[index].iTopLeftX;
	dwFontSize = dwFontSize/len;
	SetFontSize(dwFontSize);

    /* 在VideoMem的指定矩形居中显示字符串 */
	iError = MergerStringToCenterOfRectangleInVideoMem(g_atMainPageIconsLayout[index].iTopLeftX, g_atMainPageIconsLayout[index].iTopLeftY, g_atMainPageIconsLayout[index].iBotRightX, g_atMainPageIconsLayout[index].iBotRightY, (unsigned char *)str, dwBackgroundColor, ptDevVideoMem);

	return iError;
}

static void UpdateItemCorlor(PT_ItemCfg ptItemCfg)
{
	unsigned int dwBackgroundColor;
	
	if (ptItemCfg->status == TEST_STATUS_ERR)
		dwBackgroundColor = 0x00FF0000;
	else if (ptItemCfg->status == TEST_STATUS_OK)
		dwBackgroundColor = 0x0000FF00;
	else
		return;
	DrawIconWithString(ptItemCfg, ptItemCfg->name, dwBackgroundColor);
}

static void TestIfAtEndForAll(void)
{
	int item_cnt = GetConfigNumber();
	PT_ItemCfg ptItemCfg;
	int i;
	int ok_cnt = 0;
	char command[2048];

	for (i = 0; i < item_cnt; i++)
	{
		ptItemCfg = GetConfigByIndex(i);
		if (TEST_STATUS_OK == ptItemCfg->status)
			ok_cnt++;
	}

	ptItemCfg = GetConfigByName("ALL");
	if (ptItemCfg)
	{
		if ((ptItemCfg->status != TEST_STATUS_OK) && (ok_cnt == item_cnt - 1)) /* 只剩"ALL"最后一项了 */
		{
			if (ptItemCfg->command[0] && (ptItemCfg->status != TEST_STATUS_PROCESSING))
			{
				ptItemCfg->status = TEST_STATUS_PROCESSING;
				sprintf(command, "%s %s", ptItemCfg->command, GetItemStatus(ptItemCfg->status));
				system(command);
			}
		}
		else if ((ptItemCfg->status == TEST_STATUS_OK) && (ok_cnt != item_cnt))
		{
			ptItemCfg->status = TEST_STATUS_ERR;
			UpdateItemCorlor(ptItemCfg);
			sprintf(command, "%s %s", ptItemCfg->command, GetItemStatus(ptItemCfg->status));
			system(command);
		}
	}
	
}

static void UpdateItemForString(char *str)
{
	int cnt = GetConfigNumber();
	PT_ItemCfg ptItemCfg;
	int i;
	char *per;
	char processing[128];

	for (i = 0; i < cnt; i++)
	{
		ptItemCfg = GetConfigByIndex(i);
		if (strstr(str, ptItemCfg->name))
		{
			if (strstr(str, "ok"))
			{
				ptItemCfg->status = TEST_STATUS_OK;
			}
			else if (strstr(str, "err"))
			{
				ptItemCfg->status = TEST_STATUS_ERR;
			}
			else 
			{
				per = strstr(str, "percent");
				if (per)
				{
					per += strlen("percent");
					ptItemCfg->status  = TEST_STATUS_PERCENT;
					ptItemCfg->percent = strtoul(per, NULL, 0);
					snprintf(processing, 128, "%s %d%%", ptItemCfg->name, ptItemCfg->percent);
					
					DrawIconWithString(ptItemCfg, processing, 0x000000FF);
					return;
				}
			}
			UpdateItemForTouch(ptItemCfg);
		}
	}
}


static void UpdateItemForTouch(PT_ItemCfg ptItemCfg)
{
	char command[2048];
	
	UpdateItemCorlor(ptItemCfg);

	if (ptItemCfg->command[0])
	{
		sprintf(command, "%s %s", ptItemCfg->command, GetItemStatus(ptItemCfg->status));
		system(command);
	}

	TestIfAtEndForAll();
}


/**********************************************************************
 * 函数名称： MainPageRun
 * 功能描述： "主页面"的运行函数: 显示菜单图标,根据用户输入作出反应
 * 输入参数： ptParentPageParams - 未用
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static void MainPageRun(PT_PageParams ptParentPageParams)
{
	int iIndex;
	T_InputEvent tInputEvent;
	PT_ItemCfg ptItemCfg;
//	int bPressed = 0;
//	int iIndexPressed = -1;
//  T_PageParams tPageParams;

//  tPageParams.iPageID = ID("main");
	
	/* 1. 显示页面 */
	ShowMainPage(&g_tMainPageLayout);

	/* 2. 创建Prepare线程 */

	/* 3. 调用GetInputEvent获得输入事件，进而处理 */
	while (1)
	{
		DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
		iIndex = MainPageGetInputEvent(&g_tMainPageLayout, &tInputEvent);
		if (tInputEvent.iType == INPUT_TYPE_TOUCHSCREEN)
		{
			DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
			if (iIndex != -1)
			{
				/* 如果是按下 */
				if (tInputEvent.iPressure != 0)
				{
					ptItemCfg = GetConfigByIndex(iIndex);
					if (ptItemCfg && ptItemCfg->can_be_pressed)
					{
						ptItemCfg->status = !ptItemCfg->status;
						UpdateItemForTouch(ptItemCfg);
					}
				}
			}		
		}
		else if (tInputEvent.iType == INPUT_TYPE_NET)
		{
			DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
			UpdateItemForString((char *)tInputEvent.aStr);
			DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
		}
	}
}

static T_PageAction g_tMainPageAction = {
	.name          = "main",
	.Run           = MainPageRun,
	.GetInputEvent = MainPageGetInputEvent,
	//.Prepare       = MainPagePrepare;
};


/**********************************************************************
 * 函数名称： MainPageInit
 * 功能描述： 注册"主页面"
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int MainPageInit(void)
{
	return RegisterPageAction(&g_tMainPageAction);
}

