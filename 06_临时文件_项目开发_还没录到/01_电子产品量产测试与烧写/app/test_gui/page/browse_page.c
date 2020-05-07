#include <config.h>
#include <render.h>
#include <stdlib.h>
#include <file.h>
#include <fonts_manager.h>
#include <string.h>


/* 图标是一个正方体, "图标+名字"也是一个正方体
 *   --------
 *   |  图  |
 *   |  标  |
 * ------------
 * |   名字   |
 * ------------
 */

#define DIR_FILE_ICON_WIDTH    40
#define DIR_FILE_ICON_HEIGHT   DIR_FILE_ICON_WIDTH
#define DIR_FILE_NAME_HEIGHT   20
#define DIR_FILE_NAME_WIDTH   (DIR_FILE_ICON_HEIGHT + DIR_FILE_NAME_HEIGHT)
#define DIR_FILE_ALL_WIDTH    DIR_FILE_NAME_WIDTH
#define DIR_FILE_ALL_HEIGHT   DIR_FILE_ALL_WIDTH


/* browse页面里把显示区域分为"菜单"和"目录和文件"
 * "菜单"就是"up, select,pre_page,next_page"四个可操作的图标
 * "目录和文件"是浏览的内容
 */

/* 菜单的区域 */
static T_Layout g_atMenuIconsLayout[] = {
//	{0, 0, 0, 0, "return.bmp"},
	{0, 0, 0, 0, "up.bmp"},
	{0, 0, 0, 0, "select.bmp"},
	{0, 0, 0, 0, "pre_page.bmp"},
	{0, 0, 0, 0, "next_page.bmp"},
	{0, 0, 0, 0, NULL},
};

static T_PageLayout g_tBrowsePageMenuIconsLayout = {
	.iMaxTotalBytes = 0,
	.atLayout       = g_atMenuIconsLayout,
};

/* 目录或文件的区域 */
static char *g_strDirClosedIconName  = "fold_closed.bmp";
static char *g_strDirOpenedIconName  = "fold_opened.bmp";
static char *g_strFileIconName = "file.bmp";
static T_Layout *g_atDirAndFileLayout;
static T_PageLayout g_tBrowsePageDirAndFileLayout = {
	.iMaxTotalBytes = 0,
	//.atLayout       = g_atDirAndFileLayout,
};

static int g_iDirFileNumPerCol, g_iDirFileNumPerRow;

/* 用来描述某目录里的内容 */
static PT_DirContent *g_aptDirContents;  /* 数组:存有目录下"顶层子目录","文件"的名字 */
static int g_iDirContentsNumber;         /* g_aptDirContents数组有多少项 */
static int g_iStartIndex = 0;            /* 在屏幕上显示的第1个"目录和文件"是g_aptDirContents数组里的哪一项 */

/* 当前显示的目录 */
static char g_strCurDir[256] = DEFAULT_DIR;
static char g_strSelectedDir[256] = DEFAULT_DIR;

static T_PixelDatas g_tDirClosedIconPixelDatas;
static T_PixelDatas g_tDirOpenedIconPixelDatas;
static T_PixelDatas g_tFileIconPixelDatas;


/**********************************************************************
 * 函数名称： GetSelectedDir
 * 功能描述： 在"浏览页面"中,用户可能会选中某个目录-----它就是"连播页面"要显示的目录
 *            本函数返回这个目录名
 * 输入参数： 无
 * 输出参数： strSeletedDir - 里面存有用户选中的目录的名字
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
void GetSelectedDir(char *strSeletedDir)
{
    strncpy(strSeletedDir, g_strSelectedDir, 256);
    strSeletedDir[255] = '\0';
}

/**********************************************************************
 * 函数名称： CalcBrowsePageMenusLayout
 * 功能描述： 计算页面中各图标座标值
 * 输入参数： 无
 * 输出参数： ptPageLayout - 内含各图标的左上角/右下角座标值
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static void  CalcBrowsePageMenusLayout(PT_PageLayout ptPageLayout)
{
	int iWidth;
	int iHeight;
	int iXres, iYres, iBpp;
	int iTmpTotalBytes;
	PT_Layout atLayout;
	int i;

	atLayout = ptPageLayout->atLayout;
	GetDispResolution(&iXres, &iYres, &iBpp);
	ptPageLayout->iBpp = iBpp;

	if (iXres < iYres)
	{			 
		/*	 iXres/4
		 *	  ----------------------------------
		 *	   up	select	pre_page  next_page
		 *
		 *
		 *
		 *
		 *
		 *
		 *	  ----------------------------------
		 */
		 
		iWidth  = iXres / 4;
		iHeight = iWidth;
		
		/* return图标 */
		atLayout[0].iTopLeftY  = 0;
		atLayout[0].iBotRightY = atLayout[0].iTopLeftY + iHeight - 1;
		atLayout[0].iTopLeftX  = 0;
		atLayout[0].iBotRightX = atLayout[0].iTopLeftX + iWidth - 1;

		/* up图标 */
		atLayout[1].iTopLeftY  = 0;
		atLayout[1].iBotRightY = atLayout[1].iTopLeftY + iHeight - 1;
		atLayout[1].iTopLeftX  = atLayout[0].iBotRightX + 1;
		atLayout[1].iBotRightX = atLayout[1].iTopLeftX + iWidth - 1;

		/* select图标 */
		atLayout[2].iTopLeftY  = 0;
		atLayout[2].iBotRightY = atLayout[2].iTopLeftY + iHeight - 1;
		atLayout[2].iTopLeftX  = atLayout[1].iBotRightX + 1;
		atLayout[2].iBotRightX = atLayout[2].iTopLeftX + iWidth - 1;

		/* pre_page图标 */
		atLayout[3].iTopLeftY  = 0;
		atLayout[3].iBotRightY = atLayout[3].iTopLeftY + iHeight - 1;
		atLayout[3].iTopLeftX  = atLayout[2].iBotRightX + 1;
		atLayout[3].iBotRightX = atLayout[3].iTopLeftX + iWidth - 1;
#if 0
		/* next_page图标 */
		atLayout[4].iTopLeftY  = 0;
		atLayout[4].iBotRightY = atLayout[4].iTopLeftY + iHeight - 1;
		atLayout[4].iTopLeftX  = atLayout[3].iBotRightX + 1;
		atLayout[4].iBotRightX = atLayout[4].iTopLeftX + iWidth - 1;
#endif
	}
	else
	{
		/*	 iYres/4
		 *	  ----------------------------------
		 *	   up		  
		 *
		 *    select
		 *
		 *    pre_page
		 *  
		 *   next_page
		 *
		 *	  ----------------------------------
		 */
		 
		iHeight  = iYres / 4;
		iWidth = iHeight;

		/* return图标 */
		atLayout[0].iTopLeftY  = 0;
		atLayout[0].iBotRightY = atLayout[0].iTopLeftY + iHeight - 1;
		atLayout[0].iTopLeftX  = 0;
		atLayout[0].iBotRightX = atLayout[0].iTopLeftX + iWidth - 1;
		
		/* up图标 */
		atLayout[1].iTopLeftY  = atLayout[0].iBotRightY+ 1;
		atLayout[1].iBotRightY = atLayout[1].iTopLeftY + iHeight - 1;
		atLayout[1].iTopLeftX  = 0;
		atLayout[1].iBotRightX = atLayout[1].iTopLeftX + iWidth - 1;
		
		/* select图标 */
		atLayout[2].iTopLeftY  = atLayout[1].iBotRightY + 1;
		atLayout[2].iBotRightY = atLayout[2].iTopLeftY + iHeight - 1;
		atLayout[2].iTopLeftX  = 0;
		atLayout[2].iBotRightX = atLayout[2].iTopLeftX + iWidth - 1;
		
		/* pre_page图标 */
		atLayout[3].iTopLeftY  = atLayout[2].iBotRightY + 1;
		atLayout[3].iBotRightY = atLayout[3].iTopLeftY + iHeight - 1;
		atLayout[3].iTopLeftX  = 0;
		atLayout[3].iBotRightX = atLayout[3].iTopLeftX + iWidth - 1;
#if 0		
		/* next_page图标 */
		atLayout[4].iTopLeftY  = atLayout[3].iBotRightY + 1;
		atLayout[4].iBotRightY = atLayout[4].iTopLeftY + iHeight - 1;
		atLayout[4].iTopLeftX  = 0;
		atLayout[4].iBotRightX = atLayout[4].iTopLeftX + iWidth - 1;
#endif		
	}

	i = 0;
	while (atLayout[i].strIconName)
	{
		iTmpTotalBytes = (atLayout[i].iBotRightX - atLayout[i].iTopLeftX + 1) * (atLayout[i].iBotRightY - atLayout[i].iTopLeftY + 1) * iBpp / 8;
		if (ptPageLayout->iMaxTotalBytes < iTmpTotalBytes)
		{
			ptPageLayout->iMaxTotalBytes = iTmpTotalBytes;
		}
		i++;
	}
}


/**********************************************************************
 * 函数名称： CalcBrowsePageDirAndFilesLayout
 * 功能描述： 计算"目录和文件"的显示区域
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int CalcBrowsePageDirAndFilesLayout(void)
{
	int iXres, iYres, iBpp;
	int iTopLeftX, iTopLeftY;
	int iTopLeftXBak;
	int iBotRightX, iBotRightY;
    int iIconWidth, iIconHeight;
    int iNumPerCol, iNumPerRow;
    int iDeltaX, iDeltaY;
    int i, j, k = 0;
	
	GetDispResolution(&iXres, &iYres, &iBpp);

	if (iXres < iYres)
	{
		/* --------------------------------------
		 *    up select pre_page next_page 图标
		 * --------------------------------------
		 *
		 *           目录和文件
		 *
		 *
		 * --------------------------------------
		 */
		iTopLeftX  = 0;
		iBotRightX = iXres - 1;
		iTopLeftY  = g_atMenuIconsLayout[0].iBotRightY + 1;
		iBotRightY = iYres - 1;
	}
	else
	{
		/*	 iYres/4
		 *	  ----------------------------------
		 *	   up      |
		 *             |
		 *    select   |
		 *             |     目录和文件
		 *    pre_page |
		 *             |
		 *   next_page |
		 *             |
		 *	  ----------------------------------
		 */
		iTopLeftX  = g_atMenuIconsLayout[0].iBotRightX + 1;
		iBotRightX = iXres - 1;
		iTopLeftY  = 0;
		iBotRightY = iYres - 1;
	}

    /* 确定一行显示多少个"目录或文件", 显示多少行 */
    iIconWidth  = DIR_FILE_NAME_WIDTH;
    iIconHeight = iIconWidth;

    /* 图标之间的间隔要大于10个象素 */
    iNumPerRow = (iBotRightX - iTopLeftX + 1) / iIconWidth;
    while (1)
    {
        iDeltaX  = (iBotRightX - iTopLeftX + 1) - iIconWidth * iNumPerRow;
        if ((iDeltaX / (iNumPerRow + 1)) < 10)
            iNumPerRow--;
        else
            break;
    }

    iNumPerCol = (iBotRightY - iTopLeftY + 1) / iIconHeight;
    while (1)
    {
        iDeltaY  = (iBotRightY - iTopLeftY + 1) - iIconHeight * iNumPerCol;
        if ((iDeltaY / (iNumPerCol + 1)) < 10)
            iNumPerCol--;
        else
            break;
    }

    /* 每个图标之间的间隔 */
    iDeltaX = iDeltaX / (iNumPerRow + 1);
    iDeltaY = iDeltaY / (iNumPerCol + 1);

    g_iDirFileNumPerRow = iNumPerRow;
    g_iDirFileNumPerCol = iNumPerCol;


    /* 可以显示 iNumPerRow * iNumPerCol个"目录或文件"
     * 分配"两倍+1"的T_Layout结构体: 一个用来表示图标,另一个用来表示名字
     * 最后一个用来存NULL,借以判断结构体数组的末尾
     */
	g_atDirAndFileLayout = malloc(sizeof(T_Layout) * (2 * iNumPerRow * iNumPerCol + 1));
    if (NULL == g_atDirAndFileLayout)
    {
        DBG_PRINTF("malloc error!\n");
        return -1;
    }

    /* "目录和文件"整体区域的左上角、右下角坐标 */
    g_tBrowsePageDirAndFileLayout.iTopLeftX      = iTopLeftX;
    g_tBrowsePageDirAndFileLayout.iBotRightX     = iBotRightX;
    g_tBrowsePageDirAndFileLayout.iTopLeftY      = iTopLeftY;
    g_tBrowsePageDirAndFileLayout.iBotRightY     = iBotRightY;
    g_tBrowsePageDirAndFileLayout.iBpp           = iBpp;
    g_tBrowsePageDirAndFileLayout.atLayout       = g_atDirAndFileLayout;
    g_tBrowsePageDirAndFileLayout.iMaxTotalBytes = DIR_FILE_ALL_WIDTH * DIR_FILE_ALL_HEIGHT * iBpp / 8;

    
    /* 确定图标和名字的位置 
     *
     * 图标是一个正方体, "图标+名字"也是一个正方体
     *   --------
     *   |  图  |
     *   |  标  |
     * ------------
     * |   名字   |
     * ------------
     */
    iTopLeftX += iDeltaX;
    iTopLeftY += iDeltaY;
    iTopLeftXBak = iTopLeftX;
    for (i = 0; i < iNumPerCol; i++)
    {        
        for (j = 0; j < iNumPerRow; j++)
        {
            /* 图标 */
            g_atDirAndFileLayout[k].iTopLeftX  = iTopLeftX + (DIR_FILE_NAME_WIDTH - DIR_FILE_ICON_WIDTH) / 2;
            g_atDirAndFileLayout[k].iBotRightX = g_atDirAndFileLayout[k].iTopLeftX + DIR_FILE_ICON_WIDTH - 1;
            g_atDirAndFileLayout[k].iTopLeftY  = iTopLeftY;
            g_atDirAndFileLayout[k].iBotRightY = iTopLeftY + DIR_FILE_ICON_HEIGHT - 1;

            /* 名字 */
            g_atDirAndFileLayout[k+1].iTopLeftX  = iTopLeftX;
            g_atDirAndFileLayout[k+1].iBotRightX = iTopLeftX + DIR_FILE_NAME_WIDTH - 1;
            g_atDirAndFileLayout[k+1].iTopLeftY  = g_atDirAndFileLayout[k].iBotRightY + 1;
            g_atDirAndFileLayout[k+1].iBotRightY = g_atDirAndFileLayout[k+1].iTopLeftY + DIR_FILE_NAME_HEIGHT - 1;

            iTopLeftX += DIR_FILE_ALL_WIDTH + iDeltaX;
            k += 2;
        }
        iTopLeftX = iTopLeftXBak;
        iTopLeftY += DIR_FILE_ALL_HEIGHT + iDeltaY;
    }

    /* 结尾 */
    g_atDirAndFileLayout[k].iTopLeftX   = 0;
    g_atDirAndFileLayout[k].iBotRightX  = 0;
    g_atDirAndFileLayout[k].iTopLeftY   = 0;
    g_atDirAndFileLayout[k].iBotRightY  = 0;
    g_atDirAndFileLayout[k].strIconName = NULL;

    return 0;
}

/**********************************************************************
 * 函数名称： BrowsePageGetInputEvent
 * 功能描述： 为"浏览页面"获得输入数据,判断输入事件位于哪一个图标上
 * 输入参数： ptPageLayout - 内含多个图标的显示区域
 * 输出参数： ptInputEvent - 内含得到的输入数据
 * 返 回 值： -1     - 输入数据不位于任何一个图标之上
 *            其他值 - 输入数据所落在的图标(PageLayout->atLayout数组的哪一项)
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int BrowsePageGetInputEvent(PT_PageLayout ptPageLayout, PT_InputEvent ptInputEvent)
{
	return GenericGetInputEvent(ptPageLayout, ptInputEvent);
}

/**********************************************************************
 * 函数名称： GetInputPositionInPageLayout
 * 功能描述： 为"浏览页面"获得输入数据,判断输入事件位于哪一个"目录或文件"上
 * 输入参数： ptPageLayout - 内含"目录或文件"的显示区域
 * 输出参数： ptInputEvent - 内含得到的输入数据
 * 返 回 值： -1     - 输入数据不位于任何一个图标之上
 *            其他值 - 输入数据所落在的图标(PageLayout->atLayout数组的哪一项)
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int GetInputPositionInPageLayout(PT_PageLayout ptPageLayout, PT_InputEvent ptInputEvent)
{
    int i = 0;
    PT_Layout atLayout = ptPageLayout->atLayout;
        
    /* 处理数据 */
    /* 确定触点位于哪一个按钮上 */
    while (atLayout[i].iBotRightY)
    {
        if ((ptInputEvent->iX >= atLayout[i].iTopLeftX) && (ptInputEvent->iX <= atLayout[i].iBotRightX) && \
             (ptInputEvent->iY >= atLayout[i].iTopLeftY) && (ptInputEvent->iY <= atLayout[i].iBotRightY))
        {
            /* 找到了被点中的按钮 */
            return i;
        }
        else
        {
            i++;
        }           
    }

    /* 触点没有落在按钮上 */
    return -1;
}


/**********************************************************************
 * 函数名称： GenerateBrowsePageDirAndFile
 * 功能描述： 为"浏览页面"生成"目录或文件"区域中的图标和文字,就是显示目录内容
 * 输入参数： iStartIndex        - 在屏幕上显示的第1个"目录和文件"是aptDirContents数组里的哪一项
 *            iDirContentsNumber - aptDirContents数组有多少项
 *            aptDirContents     - 数组:存有目录下"顶层子目录","文件"的名字 
 *            ptVideoMem         - 在这个VideoMem中构造页面
 * 输出参数： 无
 * 返 回 值： 0      - 成功
 *            其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int GenerateBrowsePageDirAndFile(int iStartIndex, int iDirContentsNumber, PT_DirContent *aptDirContents, PT_VideoMem ptVideoMem)
{
    int iError;
    int i, j, k = 0;
    int iDirContentIndex = iStartIndex;
    PT_PageLayout ptPageLayout = &g_tBrowsePageDirAndFileLayout;
	PT_Layout atLayout = ptPageLayout->atLayout;

    ClearRectangleInVideoMem(ptPageLayout->iTopLeftX, ptPageLayout->iTopLeftY, ptPageLayout->iBotRightX, ptPageLayout->iBotRightY, ptVideoMem, COLOR_BACKGROUND);

    SetFontSize(atLayout[1].iBotRightY - atLayout[1].iTopLeftY - 5);
    
    for (i = 0; i < g_iDirFileNumPerCol; i++)
    {
        for (j = 0; j < g_iDirFileNumPerRow; j++)
        {
            if (iDirContentIndex < iDirContentsNumber)
            {
                /* 显示目录或文件的图标 */
                if (aptDirContents[iDirContentIndex]->eFileType == FILETYPE_DIR)
                {
                    PicMerge(atLayout[k].iTopLeftX, atLayout[k].iTopLeftY, &g_tDirClosedIconPixelDatas, &ptVideoMem->tPixelDatas);
                }
                else
                {
                    PicMerge(atLayout[k].iTopLeftX, atLayout[k].iTopLeftY, &g_tFileIconPixelDatas, &ptVideoMem->tPixelDatas);
                }

                k++;
                /* 显示目录或文件的名字 */
                //DBG_PRINTF("MergerStringToCenterOfRectangleInVideoMem: %s\n", aptDirContents[iDirContentIndex]->strName);
                iError = MergerStringToCenterOfRectangleInVideoMem(atLayout[k].iTopLeftX, atLayout[k].iTopLeftY, atLayout[k].iBotRightX, atLayout[k].iBotRightY, (unsigned char *)aptDirContents[iDirContentIndex]->strName, ptVideoMem);
                //ClearRectangleInVideoMem(atLayout[k].iTopLeftX, atLayout[k].iTopLeftY, atLayout[k].iBotRightX, atLayout[k].iBotRightY, ptVideoMem, 0xff0000);
                k++;

                iDirContentIndex++;
            }
            else
            {
                break;
            }
        }
        if (iDirContentIndex >= iDirContentsNumber)
        {
            break;
        }
    }
	return 0;
}


/**********************************************************************
 * 函数名称： GenerateDirAndFileIcons
 * 功能描述： 为"浏览页面"生成菜单区域中的图标
 * 输入参数： ptPageLayout - 内含多个图标的文件名和显示区域
 * 输出参数： 无
 * 返 回 值： 0      - 成功
 *            其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int GenerateDirAndFileIcons(PT_PageLayout ptPageLayout)
{
	T_PixelDatas tOriginIconPixelDatas;
    int iError;
	int iXres, iYres, iBpp;
    PT_Layout atLayout = ptPageLayout->atLayout;

	GetDispResolution(&iXres, &iYres, &iBpp);

    /* 给目录图标、文件图标分配内存 */
    g_tDirClosedIconPixelDatas.iBpp          = iBpp;
    g_tDirClosedIconPixelDatas.aucPixelDatas = malloc(ptPageLayout->iMaxTotalBytes);
    if (g_tDirClosedIconPixelDatas.aucPixelDatas == NULL)
    {
        return -1;
    }

    g_tDirOpenedIconPixelDatas.iBpp          = iBpp;
    g_tDirOpenedIconPixelDatas.aucPixelDatas = malloc(ptPageLayout->iMaxTotalBytes);
    if (g_tDirOpenedIconPixelDatas.aucPixelDatas == NULL)
    {
        return -1;
    }

    g_tFileIconPixelDatas.iBpp          = iBpp;
    g_tFileIconPixelDatas.aucPixelDatas = malloc(ptPageLayout->iMaxTotalBytes);
    if (g_tFileIconPixelDatas.aucPixelDatas == NULL)
    {
        return -1;
    }

    /* 从BMP文件里提取图像数据 */
    /* 1. 提取"fold_closed图标" */
    iError = GetPixelDatasForIcon(g_strDirClosedIconName, &tOriginIconPixelDatas);
    if (iError)
    {
        DBG_PRINTF("GetPixelDatasForIcon %s error!\n", g_strDirClosedIconName);
        return -1;
    }
    g_tDirClosedIconPixelDatas.iHeight = atLayout[0].iBotRightY - atLayout[0].iTopLeftY + 1;
    g_tDirClosedIconPixelDatas.iWidth  = atLayout[0].iBotRightX - atLayout[0].iTopLeftX + 1;
    g_tDirClosedIconPixelDatas.iLineBytes  = g_tDirClosedIconPixelDatas.iWidth * g_tDirClosedIconPixelDatas.iBpp / 8;
    g_tDirClosedIconPixelDatas.iTotalBytes = g_tDirClosedIconPixelDatas.iLineBytes * g_tDirClosedIconPixelDatas.iHeight;
    PicZoom(&tOriginIconPixelDatas, &g_tDirClosedIconPixelDatas);
    FreePixelDatasForIcon(&tOriginIconPixelDatas);

    /* 2. 提取"fold_opened图标" */
    iError = GetPixelDatasForIcon(g_strDirOpenedIconName, &tOriginIconPixelDatas);
    if (iError)
    {
        DBG_PRINTF("GetPixelDatasForIcon %s error!\n", g_strDirOpenedIconName);
        return -1;
    }
    g_tDirOpenedIconPixelDatas.iHeight = atLayout[0].iBotRightY - atLayout[0].iTopLeftY + 1;
    g_tDirOpenedIconPixelDatas.iWidth  = atLayout[0].iBotRightX - atLayout[0].iTopLeftX + 1;
    g_tDirOpenedIconPixelDatas.iLineBytes  = g_tDirOpenedIconPixelDatas.iWidth * g_tDirOpenedIconPixelDatas.iBpp / 8;
    g_tDirOpenedIconPixelDatas.iTotalBytes = g_tDirOpenedIconPixelDatas.iLineBytes * g_tDirOpenedIconPixelDatas.iHeight;
    PicZoom(&tOriginIconPixelDatas, &g_tDirOpenedIconPixelDatas);
    FreePixelDatasForIcon(&tOriginIconPixelDatas);

    /* 3. 提取"file图标" */
    iError = GetPixelDatasForIcon(g_strFileIconName, &tOriginIconPixelDatas);
    if (iError)
    {
        DBG_PRINTF("GetPixelDatasForIcon %s error!\n", g_strFileIconName);
        return -1;
    }
    g_tFileIconPixelDatas.iHeight = atLayout[0].iBotRightY - atLayout[0].iTopLeftY + 1;
    g_tFileIconPixelDatas.iWidth  = atLayout[0].iBotRightX - atLayout[0].iTopLeftX+ 1;
    g_tFileIconPixelDatas.iLineBytes  = g_tDirClosedIconPixelDatas.iWidth * g_tDirClosedIconPixelDatas.iBpp / 8;
    g_tFileIconPixelDatas.iTotalBytes = g_tFileIconPixelDatas.iLineBytes * g_tFileIconPixelDatas.iHeight;
    PicZoom(&tOriginIconPixelDatas, &g_tFileIconPixelDatas);
    FreePixelDatasForIcon(&tOriginIconPixelDatas);

    return 0;
}


/**********************************************************************
 * 函数名称： ShowBrowsePage
 * 功能描述： 显示"浏览页面"
 * 输入参数： ptPageLayout - 内含多个图标的文件名和显示区域
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static void ShowBrowsePage(PT_PageLayout ptPageLayout)
{
	PT_VideoMem ptVideoMem;
	int iError;

	PT_Layout atLayout = ptPageLayout->atLayout;
		
	/* 1. 获得显存 */
	ptVideoMem = GetVideoMem(ID("browse"), 1);
	if (ptVideoMem == NULL)
	{
		DBG_PRINTF("can't get video mem for browse page!\n");
		return;
	}

	/* 2. 描画数据 */

	/* 如果还没有计算过各图标的坐标 */
	if (atLayout[0].iTopLeftX == 0)
	{
		CalcBrowsePageMenusLayout(ptPageLayout);
        CalcBrowsePageDirAndFilesLayout();
	}

    /* 如果还没有生成"目录和文件"的图标 */
    if (!g_tDirClosedIconPixelDatas.aucPixelDatas)
    {
        GenerateDirAndFileIcons(&g_tBrowsePageDirAndFileLayout);
    }

	iError = GeneratePage(ptPageLayout, ptVideoMem);
    iError = GenerateBrowsePageDirAndFile(g_iStartIndex, g_iDirContentsNumber, g_aptDirContents, ptVideoMem);

	/* 3. 刷到设备上去 */
	FlushVideoMemToDev(ptVideoMem);

	/* 4. 解放显存 */
	PutVideoMem(ptVideoMem);
}


/**********************************************************************
 * 函数名称： SelectDirFileIcon
 * 功能描述： 改变显示设备上的"目录或文件"的图标, 表示"已经被选中"
 *            对于目录图标, 把它改为"file_opened图标"
 *            对于文件图标, 只是把图标反色
 * 输入参数： iDirFileIndex - 选中"目录和文件"区域中哪一个图标
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static void SelectDirFileIcon(int iDirFileIndex)
{
    int iDirFileContentIndex;
    PT_VideoMem ptDevVideoMem;

    ptDevVideoMem = GetDevVideoMem();

    iDirFileIndex = iDirFileIndex & ~1;    
    iDirFileContentIndex = g_iStartIndex + iDirFileIndex/2;

    /* 如果是目录, 改变图标为"file_opened图标" */
    if (g_aptDirContents[iDirFileContentIndex]->eFileType == FILETYPE_DIR)
    {
        PicMerge(g_atDirAndFileLayout[iDirFileIndex].iTopLeftX, g_atDirAndFileLayout[iDirFileIndex].iTopLeftY, &g_tDirOpenedIconPixelDatas, &ptDevVideoMem->tPixelDatas);
    }
    else /* 如果是文件, 颜色取反 */
    {
        PressButton(&g_atDirAndFileLayout[iDirFileIndex]);
        PressButton(&g_atDirAndFileLayout[iDirFileIndex + 1]);
    }
}

/**********************************************************************
 * 函数名称： DeSelectDirFileIcon
 * 功能描述： 改变显示设备上的"目录或文件"的图标, 表示"未被选中"
 *            对于目录图标, 把它改为"file_closed图标"
 *            对于文件图标, 只是把图标反色
 * 输入参数： iDirFileIndex - 选中"目录和文件"区域中哪一个图标
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static void DeSelectDirFileIcon(int iDirFileIndex)
{
    int iDirFileContentIndex;
    PT_VideoMem ptDevVideoMem;
    
    ptDevVideoMem = GetDevVideoMem();

    iDirFileIndex = iDirFileIndex & ~1;    
    iDirFileContentIndex = g_iStartIndex + iDirFileIndex/2;
    
    if (g_aptDirContents[iDirFileContentIndex]->eFileType == FILETYPE_DIR)
    {
        PicMerge(g_atDirAndFileLayout[iDirFileIndex].iTopLeftX, g_atDirAndFileLayout[iDirFileIndex].iTopLeftY, &g_tDirClosedIconPixelDatas, &ptDevVideoMem->tPixelDatas);
    }
    else
    {
        ReleaseButton(&g_atDirAndFileLayout[iDirFileIndex]);
        ReleaseButton(&g_atDirAndFileLayout[iDirFileIndex + 1]);
    }
}


#define DIRFILE_ICON_INDEX_BASE 1000
/**********************************************************************
 * 函数名称： BrowsePageRun
 * 功能描述： "浏览页面"的运行函数: 显示菜单图标,显示目录内容,读取输入数据并作出反应
 *            "浏览页面"有两个区域: 菜单图标, 目录和文件图标
 *             为统一处理, "菜单图标"的序号为0,1,2,3,..., "目录和文件图标"的序号为1000,1001,1002,....
 * 输入参数： ptParentPageParams - 内含上一个页面(父页面)的参数
 *                                 ptParentPageParams->iPageID等于ID("setting")时,本页面用于浏览/选择文件夹, 点击文件无反应 
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static void BrowsePageRun(PT_PageParams ptParentPageParams)
{
	int iIndex;
	T_InputEvent tInputEvent;
	T_InputEvent tInputEventPrePress;

    int bUsedToSelectDir = 0;
	int bIconPressed = 0;          /* 界面上是否有图标显示为"被按下" */

    /* "连续播放图片"时, 是从哪一个目录读取文件呢? 这是由"选择"按钮来选择该目录的
     * 点击目录图标将进入下一级目录, 哪怎样选择目录呢?
     * 1. 先点击"选择"按钮
     * 2. 再点击某个目录图标
     */
    int bHaveClickSelectIcon = 0;
    
	int iIndexPressed = -1;    /* 被按下的图标 */
    int iDirFileContentIndex;
    
    int iError;
    PT_VideoMem ptDevVideoMem;

    T_PageParams tPageParams;
    char strTmp[256];
    char *pcTmp;

	/* 这两句只是为了避免编译警告 */
	tInputEventPrePress.tTime.tv_sec = 0;
	tInputEventPrePress.tTime.tv_usec = 0;

    if (ptParentPageParams->iPageID == ID("setting"))  /* 如果是从"设置页面"启动了本页面, 则本页面是用来选择目录的 */
    {
        bUsedToSelectDir = 1;
    }

    ptDevVideoMem = GetDevVideoMem();

    /* 0. 获得要显示的目录的内容 */
    iError = GetDirContents(g_strCurDir, &g_aptDirContents, &g_iDirContentsNumber);
    if (iError)
    {
        DBG_PRINTF("GetDirContents error!\n");
        return;
    }
    	
	/* 1. 显示页面 */
	ShowBrowsePage(&g_tBrowsePageMenuIconsLayout);

	/* 2. 创建Prepare线程 */

	/* 3. 调用GetInputEvent获得输入事件，进而处理 */
	while (1)
	{
        /* 先确定是否触摸了菜单图标 */
		iIndex = BrowsePageGetInputEvent(&g_tBrowsePageMenuIconsLayout, &tInputEvent);

        /* 如果触点不在菜单图标上, 则判断是在哪一个"目录和文件"上 */
        if (iIndex == -1)
        {
            iIndex = GetInputPositionInPageLayout(&g_tBrowsePageDirAndFileLayout, &tInputEvent);
            if (iIndex != -1)
            {                
                if (g_iStartIndex + iIndex / 2 < g_iDirContentsNumber)  /* 判断这个触点上是否有图标 */
                    iIndex += DIRFILE_ICON_INDEX_BASE; /* 这是"目录和文件图标" */
                else
                    iIndex = -1;
            }
        }
        
        /* 如果是松开 */
		if (tInputEvent.iPressure == 0)
		{
            /* 如果当前有图标被按下 */
			if (bIconPressed)
			{
                 if (iIndexPressed < DIRFILE_ICON_INDEX_BASE)  /* 菜单图标 */
                 {
                    /* 释放图标 */
                    if (!(bUsedToSelectDir && (iIndexPressed == 1))) /* "选择"图标单独处理 */
                    {
        				ReleaseButton(&g_atMenuIconsLayout[iIndexPressed]);
                    }

                    bIconPressed    = 0;

				    if (iIndexPressed == iIndex) /* 按下和松开都是同一个按钮 */
    				{
    					switch (iIndex)
    					{
    						case 0: /* "向上"按钮 */
    						{
                                if (0 == strcmp(g_strCurDir, "/"))  /* 已经是顶层目录 */
                                {
                                    FreeDirContents(g_aptDirContents, g_iDirContentsNumber);
                                    return ;
                                }

                                pcTmp = strrchr(g_strCurDir, '/'); /* 找到最后一个'/', 把它去掉 */
                                *pcTmp = '\0';

                                FreeDirContents(g_aptDirContents, g_iDirContentsNumber);
                                iError = GetDirContents(g_strCurDir, &g_aptDirContents, &g_iDirContentsNumber);
                                if (iError)
                                {
                                    DBG_PRINTF("GetDirContents error!\n");
                                    return;
                                }
                                g_iStartIndex = 0;
                                iError = GenerateBrowsePageDirAndFile(g_iStartIndex, g_iDirContentsNumber, g_aptDirContents, ptDevVideoMem);
                                
    							break;
    						}
                            case 1: /* "选择" */
                            {
                                if (!bUsedToSelectDir)
                                {
                                    /* 如果不是用于"选择目录", 该按钮无用处 */
                                    break;
                                }
                                
                                if (!bHaveClickSelectIcon)  /* 第1次点击"选择"按钮 */
                                {
                                    bHaveClickSelectIcon = 1;
                                }
                                else
                                {
                                    ReleaseButton(&g_atMenuIconsLayout[iIndexPressed]);
                                    bIconPressed    = 0;
                                    bHaveClickSelectIcon = 0;
                                }
                                break;
                            }
                            case 2: /* "上一页" */
                            {
                                g_iStartIndex -= g_iDirFileNumPerCol * g_iDirFileNumPerRow;
                                if (g_iStartIndex >= 0)
                                {
                                    iError = GenerateBrowsePageDirAndFile(g_iStartIndex, g_iDirContentsNumber, g_aptDirContents, ptDevVideoMem);
                                }
                                else
                                {
                                    g_iStartIndex += g_iDirFileNumPerCol * g_iDirFileNumPerRow;
                                }
                                break;
                            }
                            case 3: /* "下一页" */
                            {
                                g_iStartIndex += g_iDirFileNumPerCol * g_iDirFileNumPerRow;
                                if (g_iStartIndex < g_iDirContentsNumber)
                                {
                                    iError = GenerateBrowsePageDirAndFile(g_iStartIndex, g_iDirContentsNumber, g_aptDirContents, ptDevVideoMem);
                                }
                                else
                                {
                                    g_iStartIndex -= g_iDirFileNumPerCol * g_iDirFileNumPerRow;
                                }
                                break;
                            }
    						default:
    						{
    							break;
    						}
    					}
    				}
                }                
                else /* "目录和文件图标" */
                {

                    /*
                     * 如果按下和松开时, 触点不处于同一个图标上, 则释放图标
                     */
                    if (iIndexPressed != iIndex)
                    {
                        DeSelectDirFileIcon(iIndexPressed - DIRFILE_ICON_INDEX_BASE);
                        bIconPressed      = 0;
                    }
                    else if (bHaveClickSelectIcon) /* 按下和松开都是同一个按钮, 并且"选择"按钮是按下状态 */
                    {
                        DeSelectDirFileIcon(iIndexPressed - DIRFILE_ICON_INDEX_BASE);
                        bIconPressed    = 0;
                        
                        /* 如果是目录, 记录这个目录 */
                        iDirFileContentIndex = g_iStartIndex + (iIndexPressed - DIRFILE_ICON_INDEX_BASE)/2;
                        if (g_aptDirContents[iDirFileContentIndex]->eFileType == FILETYPE_DIR)
                        {
                            ReleaseButton(&g_atMenuIconsLayout[1]);  /* 同时松开"选择按钮" */
                            bHaveClickSelectIcon = 0;

                            /* 记录目录名 */
                            snprintf(strTmp, 256, "%s/%s", g_strCurDir, g_aptDirContents[iDirFileContentIndex]->strName);
                            strTmp[255] = '\0';
                            strcpy(g_strSelectedDir, strTmp);
                        }
                    }
                    else  /* "选择"按钮不被按下时, 单击目录则进入, bUsedToSelectDir为0时单击文件则显示它 */
                    {
                        DeSelectDirFileIcon(iIndexPressed - DIRFILE_ICON_INDEX_BASE);
                        bIconPressed    = 0;
                        
                        /* 如果是目录, 进入这个目录 */
                        iDirFileContentIndex = g_iStartIndex + (iIndexPressed - DIRFILE_ICON_INDEX_BASE)/2;
                        if (g_aptDirContents[iDirFileContentIndex]->eFileType == FILETYPE_DIR)
                        {
                            snprintf(strTmp, 256, "%s/%s", g_strCurDir, g_aptDirContents[iDirFileContentIndex]->strName);
                            strTmp[255] = '\0';
                            strcpy(g_strCurDir, strTmp);
                            FreeDirContents(g_aptDirContents, g_iDirContentsNumber);
                            iError = GetDirContents(g_strCurDir, &g_aptDirContents, &g_iDirContentsNumber);
                            if (iError)
                            {
                                DBG_PRINTF("GetDirContents error!\n");
                                return;
                            }
                            g_iStartIndex = 0;
                            iError = GenerateBrowsePageDirAndFile(g_iStartIndex, g_iDirContentsNumber, g_aptDirContents, ptDevVideoMem);
                        }
                        else if (bUsedToSelectDir == 0) /* bUsedToSelectDir为0时单击文件时显示它 */
                        {
                            snprintf(tPageParams.strCurPictureFile, 256, "%s/%s", g_strCurDir, g_aptDirContents[iDirFileContentIndex]->strName);
                            tPageParams.strCurPictureFile[255] = '\0';
                            if (isPictureFileSupported(tPageParams.strCurPictureFile))
                            {
                                tPageParams.iPageID = ID("browse");
                                Page("manual")->Run(&tPageParams);
                                ShowBrowsePage(&g_tBrowsePageMenuIconsLayout);
                            }
                        }
                    }                    
                }
            }                                
		}        
		else /* 按下状态 */
		{			
			if (iIndex != -1)
			{
                if (!bIconPressed)  /* 之前未曾有按钮被按下 */
                {
    				bIconPressed = 1;
    				iIndexPressed = iIndex;                    
					tInputEventPrePress = tInputEvent;  /* 记录下来 */
                    if (iIndex < DIRFILE_ICON_INDEX_BASE)  /* 菜单图标 */
                    {
                        if (bUsedToSelectDir)
                        {
                            if (!(bHaveClickSelectIcon && (iIndexPressed == 1)))  /* 如果已经按下"选择"按钮, 自然不用再次反转该图标 */
            					PressButton(&g_atMenuIconsLayout[iIndex]);
                        }
                        else
                        {
                            if (!bHaveClickSelectIcon)
            					PressButton(&g_atMenuIconsLayout[iIndex]);
                        }
                    }
                    else   /* 目录和文件图标 */
                    {
                        SelectDirFileIcon(iIndex - DIRFILE_ICON_INDEX_BASE);
                    }
                }

                /* 长按"向上"按钮, 返回 */
				if (iIndexPressed == 0)
				{
					if (TimeMSBetween(tInputEventPrePress.tTime, tInputEvent.tTime) > 2000)
					{
                        FreeDirContents(g_aptDirContents, g_iDirContentsNumber);
                        return;
					}
				}

			}
		}
	}
}

static T_PageAction g_tBrowsePageAction = {
	.name          = "browse",
	.Run           = BrowsePageRun,
	.GetInputEvent = BrowsePageGetInputEvent,
	//.Prepare       = BrowsePagePrepare;
};


/**********************************************************************
 * 函数名称： BrowsePageInit
 * 功能描述： 注册"浏览页面"
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int BrowsePageInit(void)
{
	return RegisterPageAction(&g_tBrowsePageAction);
}

