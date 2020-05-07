#include <config.h>
#include <page_manager.h>
#include <render.h>
#include <string.h>
#include <stdlib.h>
#include <fonts_manager.h>


extern void GetSelectedDir(char *strSeletedDir);
extern void GetIntervalTime(int *piIntervalSecond);

static PT_PageAction g_ptPageActionHead;


/**********************************************************************
 * 函数名称： RegisterPageAction
 * 功能描述： 注册"页面模块", "页面模块"含有页面显示的函数
 * 输入参数： ptPageAction - 一个结构体,内含"页面模块"的操作函数
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int RegisterPageAction(PT_PageAction ptPageAction)
{
	PT_PageAction ptTmp;

	if (!g_ptPageActionHead)
	{
		g_ptPageActionHead   = ptPageAction;
		ptPageAction->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptPageActionHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = ptPageAction;
		ptPageAction->ptNext = NULL;
	}

	return 0;
}


/**********************************************************************
 * 函数名称： ShowPages
 * 功能描述： 显示本程序能支持的"页面模块"
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
void ShowPages(void)
{
	int i = 0;
	PT_PageAction ptTmp = g_ptPageActionHead;

	while (ptTmp)
	{
		printf("%02d %s\n", i++, ptTmp->name);
		ptTmp = ptTmp->ptNext;
	}
}

/**********************************************************************
 * 函数名称： Page
 * 功能描述： 根据名字取出指定的"页面模块"
 * 输入参数： pcName - 名字
 * 输出参数： 无
 * 返 回 值： NULL   - 失败,没有指定的模块, 
 *            非NULL - "页面模块"的PT_PageAction结构体指针
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
PT_PageAction Page(char *pcName)
{
	PT_PageAction ptTmp = g_ptPageActionHead;
	
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
 * 函数名称： ID
 * 功能描述： 根据名字算出一个唯一的整数,它用来标识VideoMem中的显示数据
 * 输入参数： strName - 名字
 * 输出参数： 无
 * 返 回 值： 一个唯一的整数
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int ID(char *strName)
{
	return (int)(strName[0]) + (int)(strName[1]) + (int)(strName[2]) + (int)(strName[3]);
}


/**********************************************************************
 * 函数名称： GeneratePage
 * 功能描述： 从图标文件中解析出图像数据并放在指定区域,从而生成页面数据
 * 输入参数： ptPageLayout - 内含多个图标的文件名和显示区域
 *            ptVideoMem   - 在这个VideoMem里构造页面数据
 * 输出参数： 无
 * 返 回 值： 0      - 成功
 *            其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int GeneratePage(PT_PageLayout ptPageLayout, PT_VideoMem ptVideoMem)
{
	T_PixelDatas tOriginIconPixelDatas;
	T_PixelDatas tIconPixelDatas;
	int iError;
	PT_Layout atLayout = ptPageLayout->atLayout;
		
	/* 描画数据: VideoMem中的页面数据未生成的情况下才执行下面操作 */
	if (ptVideoMem->ePicState != PS_GENERATED)
	{
		ClearVideoMem(ptVideoMem, COLOR_BACKGROUND);

		tIconPixelDatas.iBpp          = ptPageLayout->iBpp;
        /* 分配一个临时的内存, 它用来存放缩放后的图标数据 */
		tIconPixelDatas.aucPixelDatas = malloc(ptPageLayout->iMaxTotalBytes);
		if (tIconPixelDatas.aucPixelDatas == NULL)
		{
			return -1;
		}

		while (atLayout->strIconName)
		{
            /* 取得图标文件的象素数据 */
			iError = GetPixelDatasForIcon(atLayout->strIconName, &tOriginIconPixelDatas);
			if (iError)
			{
				DBG_PRINTF("GetPixelDatasForIcon %s error!\n", atLayout->strIconName);
                free(tIconPixelDatas.aucPixelDatas);
				return -1;
			}
 			tIconPixelDatas.iHeight = atLayout->iBotRightY - atLayout->iTopLeftY + 1;
			tIconPixelDatas.iWidth  = atLayout->iBotRightX - atLayout->iTopLeftX+ 1;
			tIconPixelDatas.iLineBytes  = tIconPixelDatas.iWidth * tIconPixelDatas.iBpp / 8;
			tIconPixelDatas.iTotalBytes = tIconPixelDatas.iLineBytes * tIconPixelDatas.iHeight;

            /* 把原始的图标象素数据缩放为指定大小 */
 			PicZoom(&tOriginIconPixelDatas, &tIconPixelDatas);

            /* 把缩放后的图标数据,合并到VideoMem的指定区域 */
 			PicMerge(atLayout->iTopLeftX, atLayout->iTopLeftY, &tIconPixelDatas, &ptVideoMem->tPixelDatas);

            /* 释放原始的图标象素数据 */
 			FreePixelDatasForIcon(&tOriginIconPixelDatas);
 			atLayout++;
		}

        /* 数据上面分配的临时内存 */
		free(tIconPixelDatas.aucPixelDatas);
		ptVideoMem->ePicState = PS_GENERATED;
	}

	return 0;
}



/**********************************************************************
 * 函数名称： GenerateTextPage
 * 功能描述： 用文字生成图标
 * 输入参数： ptPageLayout - 内含多个图标的名字和显示区域
 *            ptVideoMem   - 在这个VideoMem里构造页面数据
 *            dwBackColor  - 背景颜色
 * 输出参数： 无
 * 返 回 值： 0      - 成功
 *            其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/03/16	     V1.0	  韦东山	      创建
 ***********************************************************************/
int GenerateTextPage(PT_PageLayout ptPageLayout, PT_VideoMem ptVideoMem, unsigned int dwBackgroundColor)
{
	int iError;
	PT_Layout atLayout = ptPageLayout->atLayout;
	unsigned int dwFontSize;
	int len;
		
	/* 描画数据: VideoMem中的页面数据未生成的情况下才执行下面操作 */
	if (ptVideoMem->ePicState != PS_GENERATED)
	{
		ClearVideoMem(ptVideoMem, COLOR_BACKGROUND);

		while (atLayout->strIconName)
		{
			len = strlen(atLayout->strIconName);
			dwFontSize = atLayout->iBotRightX - atLayout->iTopLeftX;
			dwFontSize = dwFontSize/len;
			SetFontSize(dwFontSize);

			/* 在VideoMem的指定矩形居中显示字符串 */
			iError = MergerStringToCenterOfRectangleInVideoMem(atLayout->iTopLeftX, atLayout->iTopLeftY, atLayout->iBotRightX, atLayout->iBotRightY, (unsigned char *)atLayout->strIconName, dwBackgroundColor, ptVideoMem);
			iError = iError;
 			atLayout++;
		}

		ptVideoMem->ePicState = PS_GENERATED;
	}

	return 0;
}

/**********************************************************************
 * 函数名称： GenerateItemForPage
 * 功能描述： 从图标文件中解析出图像数据并放在指定区域,从而修改页面数据
 * 输入参数： strIconName  - 文件名
 *            ptPageLayout - 内含1个图标的显示区域
 *            ptVideoMem   - 在这个VideoMem里构造页面数据
 * 输出参数： 无
 * 返 回 值： 0      - 成功
 *            其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int GenerateItemForPage(char *strIconName, PT_PageLayout ptPageLayout, int indexLayout, PT_VideoMem ptVideoMem)
{
	T_PixelDatas tOriginIconPixelDatas;
	T_PixelDatas tIconPixelDatas;
	int iError;
	PT_Layout atLayout = &ptPageLayout->atLayout[indexLayout];
		
	/* 描画数据: VideoMem中的页面数据未生成的情况下才执行下面操作 */
	//if (ptVideoMem->ePicState != PS_GENERATED)
	{
		//ClearVideoMem(ptVideoMem, COLOR_BACKGROUND);

		tIconPixelDatas.iBpp          = ptPageLayout->iBpp;
        /* 分配一个临时的内存, 它用来存放缩放后的图标数据 */
		tIconPixelDatas.aucPixelDatas = malloc(ptPageLayout->iMaxTotalBytes);
		if (tIconPixelDatas.aucPixelDatas == NULL)
		{
			DBG_PRINTF("malloc fail!\n");
			return -1;
		}

        /* 取得图标文件的象素数据 */
		iError = GetPixelDatasForIcon(strIconName, &tOriginIconPixelDatas);
		if (iError)
		{
			DBG_PRINTF("GetPixelDatasForIcon %s error!\n", atLayout->strIconName);
            free(tIconPixelDatas.aucPixelDatas);
			return -1;
		}
		tIconPixelDatas.iHeight = atLayout->iBotRightY - atLayout->iTopLeftY + 1;
		tIconPixelDatas.iWidth  = atLayout->iBotRightX - atLayout->iTopLeftX+ 1;
		tIconPixelDatas.iLineBytes  = tIconPixelDatas.iWidth * tIconPixelDatas.iBpp / 8;
		tIconPixelDatas.iTotalBytes = tIconPixelDatas.iLineBytes * tIconPixelDatas.iHeight;

        /* 把原始的图标象素数据缩放为指定大小 */
		PicZoom(&tOriginIconPixelDatas, &tIconPixelDatas);

        /* 把缩放后的图标数据,合并到VideoMem的指定区域 */
		PicMerge(atLayout->iTopLeftX, atLayout->iTopLeftY, &tIconPixelDatas, &ptVideoMem->tPixelDatas);

        /* 释放原始的图标象素数据 */
		FreePixelDatasForIcon(&tOriginIconPixelDatas);

        /* 数据上面分配的临时内存 */
		free(tIconPixelDatas.aucPixelDatas);
		ptVideoMem->ePicState = PS_GENERATED;
	}

	return 0;
}


/**********************************************************************
 * 函数名称： TimeMSBetween
 * 功能描述： 两个时间点的间隔:单位ms
 * 输入参数： tTimeStart - 起始时间点
 *            tTimeEnd   - 结束时间点
 * 输出参数： 无
 * 返 回 值： 间隔,单位ms
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int TimeMSBetween(struct timeval tTimeStart, struct timeval tTimeEnd)
{
	int iMS;
	iMS = (tTimeEnd.tv_sec - tTimeStart.tv_sec) * 1000 + (tTimeEnd.tv_usec - tTimeStart.tv_usec) / 1000;
	return iMS;
}

/**********************************************************************
 * 函数名称： GenericGetInputEvent
 * 功能描述： 读取输入数据,并判断它位于哪一个图标上
 * 输入参数： ptPageLayout - 内含多个图标的显示区域
 * 输出参数： ptInputEvent - 内含得到的输入数据
 * 返 回 值： -1     - 输入数据不位于任何一个图标之上
 *            其他值 - 输入数据所落在的图标(PageLayout->atLayout数组的哪一项)
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int GenericGetInputEvent(PT_PageLayout ptPageLayout, PT_InputEvent ptInputEvent)
{
	T_InputEvent tInputEvent;
	int iRet;
	int i = 0;
	PT_Layout atLayout =NULL;

	if (ptPageLayout)
		atLayout = ptPageLayout->atLayout;
	
	/* 获得原始的触摸屏数据 
	 * 它是调用input_manager.c的函数，此函数会让当前线否休眠
	 * 当触摸屏线程获得数据后，会把它唤醒
	 */
	DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	iRet = GetInputEvent(&tInputEvent);
	if (iRet)
	{
		DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
		return -1;
	}
	DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

	*ptInputEvent = tInputEvent;
	
	/* 处理数据 */
	/* 确定触点位于哪一个按钮上 */
	if (tInputEvent.iType == INPUT_TYPE_TOUCHSCREEN)
	{
    	while (atLayout && atLayout[i].strIconName)
    	{
    		if ((tInputEvent.iX >= atLayout[i].iTopLeftX) && (tInputEvent.iX <= atLayout[i].iBotRightX) && \
    			 (tInputEvent.iY >= atLayout[i].iTopLeftY) && (tInputEvent.iY <= atLayout[i].iBotRightY))
    		{
    			/* 找到了被点中的按钮 */
    			return i;
    		}
    		else
    		{
    			i++;
    		}			
    	}
	}
    else if (tInputEvent.iType == INPUT_TYPE_NET)
    {
		DBG_PRINTF("get net msg: %s \n", (char *)tInputEvent.aStr);
        return 1;
    }
        

	/* 触点没有落在按钮上 */
	return -1;
}


/**********************************************************************
 * 函数名称： GetPageCfg
 * 功能描述： 获得页面的配置参数,
 *            对于连续播放页面,它需要得到2个参数:播放哪个目录下的图片,图片之间的播放间隔
 * 输入参数： 无
 * 输出参数： ptPageCfg - 内含得到的参数
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
void GetPageCfg(PT_PageCfg ptPageCfg)
{
//    GetSelectedDir(ptPageCfg->strSeletedDir);
//    GetIntervalTime(&ptPageCfg->iIntervalSecond);
}

/**********************************************************************
 * 函数名称： PagesInit
 * 功能描述： 调用各个"页面模块"的初始化函数,就是注册它们
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int PagesInit(void)
{
	int iError;

	iError  = MainPageInit();
#if 0
	iError |= SettingPageInit();
	iError |= IntervalPageInit();
	iError |= BrowsePageInit();
    iError |= AutoPageInit();
    iError |= ManualPageInit();		
#endif    
	return iError;
}


