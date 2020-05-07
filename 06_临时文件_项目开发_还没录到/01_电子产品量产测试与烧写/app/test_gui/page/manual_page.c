#include <config.h>
#include <render.h>
#include <stdlib.h>
#include <file.h>
#include <fonts_manager.h>
#include <string.h>

/* �Ŵ�/��Сϵ�� */
#define ZOOM_RATIO (0.9)

#define SLIP_MIN_DISTANCE (2*2)


/* �˵������� */
static T_Layout g_atManualMenuIconsLayout[] = {
	{0, 0, 0, 0, "return.bmp"},
	{0, 0, 0, 0, "zoomout.bmp"},
	{0, 0, 0, 0, "zoomin.bmp"},
	{0, 0, 0, 0, "pre_pic.bmp"},
    {0, 0, 0, 0, "next_pic.bmp"},
    {0, 0, 0, 0, "continue_mod_small.bmp"},
	{0, 0, 0, 0, NULL},
};

static T_PageLayout g_tManualPageMenuIconsLayout = {
	.iMaxTotalBytes = 0,
	.atLayout       = g_atManualMenuIconsLayout,
};

static T_Layout g_tManualPictureLayout;

static T_PixelDatas g_tOriginPicPixelDatas;
static T_PixelDatas g_tZoomedPicPixelDatas;

/* ��ʾ��LCD�ϵ�ͼƬ, �������ĵ�, ��g_tZoomedPicPixelDatas������� */
static int g_iXofZoomedPicShowInCenter;  
static int g_iYofZoomedPicShowInCenter;


/**********************************************************************
 * �������ƣ� CalcManualPageMenusLayout
 * ���������� ����ҳ���и�ͼ������ֵ
 * ��������� ��
 * ��������� ptPageLayout - �ں���ͼ������Ͻ�/���½�����ֵ
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static void  CalcManualPageMenusLayout(PT_PageLayout ptPageLayout)
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
		/*	 iXres/6
		 *	  --------------------------------------------------------------
		 *	   return	zoomout	zoomin  pre_pic next_pic continue_mod_small
		 *
		 *
		 *
		 *
		 *
		 *
		 *	  --------------------------------------------------------------
		 */
		 
		iWidth  = iXres / 6;
		iHeight = iWidth;

		/* returnͼ�� */
		atLayout[0].iTopLeftY  = 0;
		atLayout[0].iBotRightY = atLayout[0].iTopLeftY + iHeight - 1;
		atLayout[0].iTopLeftX  = 0;
		atLayout[0].iBotRightX = atLayout[0].iTopLeftX + iWidth - 1;

        /* ����5��ͼ�� */
        for (i = 1; i < 6; i++)
        {
    		atLayout[i].iTopLeftY  = 0;
    		atLayout[i].iBotRightY = atLayout[i].iTopLeftY + iHeight - 1;
    		atLayout[i].iTopLeftX  = atLayout[i-1].iBotRightX + 1;
    		atLayout[i].iBotRightX = atLayout[i].iTopLeftX + iWidth - 1;
        }

	}
	else
	{
		/*	 iYres/6
		 *	  ----------------------------------
		 *	   up		  
		 *
		 *    zoomout	    
		 *
		 *    zoomin
		 *  
		 *    pre_pic
		 *
		 *    next_pic
		 *
		 *    continue_mod_small
		 *
		 *	  ----------------------------------
		 */
		 
		iHeight  = iYres / 6;
		iWidth = iHeight;

		/* returnͼ�� */
		atLayout[0].iTopLeftY  = 0;
		atLayout[0].iBotRightY = atLayout[0].iTopLeftY + iHeight - 1;
		atLayout[0].iTopLeftX  = 0;
		atLayout[0].iBotRightX = atLayout[0].iTopLeftX + iWidth - 1;
		
        /* ����5��ͼ�� */
        for (i = 1; i < 6; i++)
        {
    		atLayout[i].iTopLeftY  = atLayout[i-1].iBotRightY+ 1;
    		atLayout[i].iBotRightY = atLayout[i].iTopLeftY + iHeight - 1;
    		atLayout[i].iTopLeftX  = 0;
    		atLayout[i].iBotRightX = atLayout[i].iTopLeftX + iWidth - 1;
        }		
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
 * �������ƣ� CalcManualPagePictureLayout
 * ���������� ����"manualҳ��"��"ͼƬ����ʾ����"
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static void CalcManualPagePictureLayout(void)
{
	int iXres, iYres, iBpp;
	int iTopLeftX, iTopLeftY;
	int iBotRightX, iBotRightY;
	
	GetDispResolution(&iXres, &iYres, &iBpp);

	if (iXres < iYres)
	{
		/*	 iXres/6
		 *	  --------------------------------------------------------------
		 *	   return	zoomout	zoomin  pre_pic next_pic continue_mod_small  (ͼ��)
		 *	  --------------------------------------------------------------
		 *
		 *                              ͼƬ
		 *
		 *
		 *	  --------------------------------------------------------------
		 */
		iTopLeftX  = 0;
		iBotRightX = iXres - 1;
		iTopLeftY  = g_atManualMenuIconsLayout[0].iBotRightY + 1;
		iBotRightY = iYres - 1;
	}
	else
	{
		/*	 iYres/6
		 *	  --------------------------------------------------------------
		 *	   up		         |
		 *                       |
		 *    zoomout	         |
		 *                       |
		 *    zoomin             |
		 *                       |
		 *    pre_pic            |                 ͼƬ
		 *                       |
		 *    next_pic           |
		 *                       |
		 *    continue_mod_small | 
		 *                       |
		 *	  --------------------------------------------------------------
		 */
		iTopLeftX  = g_atManualMenuIconsLayout[0].iBotRightX + 1;
		iBotRightX = iXres - 1;
		iTopLeftY  = 0;
		iBotRightY = iYres - 1;
	}

    g_tManualPictureLayout.iTopLeftX   = iTopLeftX;
    g_tManualPictureLayout.iTopLeftY   = iTopLeftY;
    g_tManualPictureLayout.iBotRightX  = iBotRightX;
    g_tManualPictureLayout.iBotRightY  = iBotRightY;
    g_tManualPictureLayout.strIconName = NULL;
}


/**********************************************************************
 * �������ƣ� GetOriginPictureFilePixelDatas
 * ���������� ���ͼƬ�ļ���ԭʼ��������
 * ��������� strFileName - �ļ���(������·��)
 * ��������� ��
 * �� �� ֵ�� NULL   - ʧ��
 *            ��NULL - һ��PT_PixelDatas�ṹָ��,�ں�ͼ����������
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static PT_PixelDatas GetOriginPictureFilePixelDatas(char *strFileName)
{
    int iError;

    if (g_tOriginPicPixelDatas.aucPixelDatas)
    {
        free(g_tOriginPicPixelDatas.aucPixelDatas);
        g_tOriginPicPixelDatas.aucPixelDatas = NULL;
    }
    
    /* ���ͼƬ�ļ������� */
    iError = GetPixelDatasFrmFile(strFileName, &g_tOriginPicPixelDatas);
    if (iError)
    {
        return NULL;
    }
    else
    {
        return &g_tOriginPicPixelDatas;
    }
}


/**********************************************************************
 * �������ƣ� GetZoomedPicPixelDatas
 * ���������� ������ź��ͼƬ��������
 * ��������� strFileName - �ļ���(������·��)
 * ��������� ��
 * �� �� ֵ�� NULL   - ʧ��
 *            ��NULL - һ��PT_PixelDatas�ṹָ��,�ں�ͼ������
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static PT_PixelDatas GetZoomedPicPixelDatas(PT_PixelDatas ptOriginPicPixelDatas, int iZoomedWidth, int iZoomedHeight)
{
    float k;
	int iXres, iYres, iBpp;
    
	GetDispResolution(&iXres, &iYres, &iBpp);

    if (g_tZoomedPicPixelDatas.aucPixelDatas)
    {
        free(g_tZoomedPicPixelDatas.aucPixelDatas);
        g_tZoomedPicPixelDatas.aucPixelDatas = NULL;
    }
    
    k = (float)ptOriginPicPixelDatas->iHeight / ptOriginPicPixelDatas->iWidth;
    g_tZoomedPicPixelDatas.iWidth  = iZoomedWidth;
    g_tZoomedPicPixelDatas.iHeight = iZoomedWidth * k;
    if (g_tZoomedPicPixelDatas.iHeight > iZoomedHeight)
    {
        g_tZoomedPicPixelDatas.iWidth  = iZoomedHeight / k;
        g_tZoomedPicPixelDatas.iHeight = iZoomedHeight;
    }
    g_tZoomedPicPixelDatas.iBpp        = iBpp;
    g_tZoomedPicPixelDatas.iLineBytes  = g_tZoomedPicPixelDatas.iWidth * g_tZoomedPicPixelDatas.iBpp / 8;
    g_tZoomedPicPixelDatas.iTotalBytes = g_tZoomedPicPixelDatas.iLineBytes * g_tZoomedPicPixelDatas.iHeight;
    g_tZoomedPicPixelDatas.aucPixelDatas = malloc(g_tZoomedPicPixelDatas.iTotalBytes);
    if (g_tZoomedPicPixelDatas.aucPixelDatas == NULL)
    {
        return NULL;
    }
    
    PicZoom(ptOriginPicPixelDatas, &g_tZoomedPicPixelDatas);
    return &g_tZoomedPicPixelDatas;
}


/**********************************************************************
 * �������ƣ� ShowPictureInManualPage
 * ���������� ��"manualҳ��"����ʾͼƬ
 * ��������� strFileName - Ҫ��ʾ���ļ�������(������·��)
 *            ptVideoMem  - �����VideoMem����ʾ
 * ��������� ��
 * �� �� ֵ�� 0      - �ɹ�
 *            ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static int ShowPictureInManualPage(PT_VideoMem ptVideoMem, char *strFileName)
{
    PT_PixelDatas ptOriginPicPixelDatas;
    PT_PixelDatas ptZoomedPicPixelDatas;
    int iPictureLayoutWidth;
    int iPictureLayoutHeight;
    int iTopLeftX, iTopLeftY;
	int iXres, iYres, iBpp;

	GetDispResolution(&iXres, &iYres, &iBpp);

    /* ���ͼƬ�ļ���ԭʼ���� */
    ptOriginPicPixelDatas = GetOriginPictureFilePixelDatas(strFileName);
    if (!ptOriginPicPixelDatas)
    {
        return -1;
    }
    
    /* ��ͼƬ���������ŵ�LCD��Ļ��, ������ʾ */
    iPictureLayoutWidth  = g_tManualPictureLayout.iBotRightX - g_tManualPictureLayout.iTopLeftX + 1;
    iPictureLayoutHeight = g_tManualPictureLayout.iBotRightY - g_tManualPictureLayout.iTopLeftY + 1;

    ptZoomedPicPixelDatas = GetZoomedPicPixelDatas(&g_tOriginPicPixelDatas, iPictureLayoutWidth, iPictureLayoutHeight);
    if (!ptZoomedPicPixelDatas)
    {
        return -1;
    }
        
    /* ���������ʾʱ���Ͻ����� */
    iTopLeftX = g_tManualPictureLayout.iTopLeftX + (iPictureLayoutWidth - ptZoomedPicPixelDatas->iWidth) / 2;
    iTopLeftY = g_tManualPictureLayout.iTopLeftY + (iPictureLayoutHeight - ptZoomedPicPixelDatas->iHeight) / 2;
    g_iXofZoomedPicShowInCenter = ptZoomedPicPixelDatas->iWidth / 2;
    g_iYofZoomedPicShowInCenter = ptZoomedPicPixelDatas->iHeight / 2;

    ClearVideoMemRegion(ptVideoMem, &g_tManualPictureLayout, COLOR_BACKGROUND);
    PicMerge(iTopLeftX, iTopLeftY, ptZoomedPicPixelDatas, &ptVideoMem->tPixelDatas);

    return 0;
}


/**********************************************************************
 * �������ƣ� ShowManualPage
 * ���������� ��ʾ"manualҳ��": ������ʾ�˵�ͼ����,������ʾͼƬ
 * ��������� ptPageLayout - �ں����ͼ����ļ�������ʾ����
 *            strFileName  - Ҫ��ʾ��ͼƬ
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static void ShowManualPage(PT_PageLayout ptPageLayout, char *strFileName)
{
	PT_VideoMem ptVideoMem;
	int iError;

	PT_Layout atLayout = ptPageLayout->atLayout;
		
	/* 1. ����Դ� */
	ptVideoMem = GetVideoMem(ID("manual"), 1);
	if (ptVideoMem == NULL)
	{
		DBG_PRINTF("can't get video mem for manual page!\n");
		return;
	}

	/* 2. �軭���� */

	/* �����û�м������ͼ������� */
	if (atLayout[0].iTopLeftX == 0)
	{
		CalcManualPageMenusLayout(ptPageLayout);
        CalcManualPagePictureLayout();
	}

    /* ��videomem������ͼ�� */
	iError = GeneratePage(ptPageLayout, ptVideoMem);

    iError = ShowPictureInManualPage(ptVideoMem, strFileName);
    if (iError)
    {
        PutVideoMem(ptVideoMem);
        return;
    }
    
	/* 3. ˢ���豸��ȥ */
	FlushVideoMemToDev(ptVideoMem);

	/* 4. ����Դ� */
	PutVideoMem(ptVideoMem);
}


/**********************************************************************
 * �������ƣ� ManualPageGetInputEvent
 * ���������� Ϊ"manualҳ��"�����������,�ж������¼�λ����һ��ͼ����
 * ��������� ptPageLayout - �ں����ͼ�����ʾ����
 * ��������� ptInputEvent - �ں��õ�����������
 * �� �� ֵ�� -1     - �������ݲ�λ���κ�һ��ͼ��֮��
 *            ����ֵ - �������������ڵ�ͼ��(PageLayout->atLayout�������һ��)
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static int ManualPageGetInputEvent(PT_PageLayout ptPageLayout, PT_InputEvent ptInputEvent)
{
	return GenericGetInputEvent(ptPageLayout, ptInputEvent);
}


/**********************************************************************
 * �������ƣ� DistanceBetweenTwoPoint
 * ���������� ������������ľ���, Ϊ�򻯼���, ���ؾ����ƽ��ֵ
 * ��������� ptInputEvent1 - ����1
              ptInputEvent1 - ����2
 * ��������� ��
 * �� �� ֵ�� ��������ƽ��ֵ
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static int DistanceBetweenTwoPoint(PT_InputEvent ptInputEvent1, PT_InputEvent ptInputEvent2)
{
    return (ptInputEvent1->iX - ptInputEvent2->iX) * (ptInputEvent1->iX - ptInputEvent2->iX) + \
           (ptInputEvent1->iY - ptInputEvent2->iY) * (ptInputEvent1->iY - ptInputEvent2->iY);
}

/**********************************************************************
 * �������ƣ� ShowZoomedPictureInLayout
 * ���������� ��"manualҳ��"����ʾ�������ŵ�ͼƬ
 * ��������� ptZoomedPicPixelDatas - �ں��Ѿ����ŵ�ͼƬ����������
 *            ptVideoMem            - �����VideoMem����ʾ
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static void ShowZoomedPictureInLayout(PT_PixelDatas ptZoomedPicPixelDatas, PT_VideoMem ptVideoMem)
{
    int iStartXofNewPic, iStartYofNewPic;
    int iStartXofOldPic, iStartYofOldPic;
    int iWidthPictureInPlay, iHeightPictureInPlay;
    int iPictureLayoutWidth, iPictureLayoutHeight;
    int iDeltaX, iDeltaY;

    iPictureLayoutWidth  = g_tManualPictureLayout.iBotRightX - g_tManualPictureLayout.iTopLeftX + 1;
    iPictureLayoutHeight = g_tManualPictureLayout.iBotRightY - g_tManualPictureLayout.iTopLeftY + 1;
    
    /* ��ʾ������ */
    iStartXofNewPic = g_iXofZoomedPicShowInCenter - iPictureLayoutWidth/2;
    if (iStartXofNewPic < 0)
    {
        iStartXofNewPic = 0;
    }
    if (iStartXofNewPic > ptZoomedPicPixelDatas->iWidth)
    {
        iStartXofNewPic = ptZoomedPicPixelDatas->iWidth;
    }

    /* 
     * g_iXofZoomedPicShowInCenter - iStartXofNewPic = PictureLayout���ĵ�X���� - iStartXofOldPic
     */
    iDeltaX = g_iXofZoomedPicShowInCenter - iStartXofNewPic;
    iStartXofOldPic = (g_tManualPictureLayout.iTopLeftX + iPictureLayoutWidth / 2) - iDeltaX;
    if (iStartXofOldPic < g_tManualPictureLayout.iTopLeftX)
    {
        iStartXofOldPic = g_tManualPictureLayout.iTopLeftX;
    }
    if (iStartXofOldPic > g_tManualPictureLayout.iBotRightX)
    {
        iStartXofOldPic = g_tManualPictureLayout.iBotRightX + 1;
    }
        
    if ((ptZoomedPicPixelDatas->iWidth - iStartXofNewPic) > (g_tManualPictureLayout.iBotRightX - iStartXofOldPic + 1))
        iWidthPictureInPlay = (g_tManualPictureLayout.iBotRightX - iStartXofOldPic + 1);
    else
        iWidthPictureInPlay = (ptZoomedPicPixelDatas->iWidth - iStartXofNewPic);
    
    iStartYofNewPic = g_iYofZoomedPicShowInCenter - iPictureLayoutHeight/2;
    if (iStartYofNewPic < 0)
    {
        iStartYofNewPic = 0;
    }
    if (iStartYofNewPic > ptZoomedPicPixelDatas->iHeight)
    {
        iStartYofNewPic = ptZoomedPicPixelDatas->iHeight;
    }

    /* 
     * g_iYofZoomedPicShowInCenter - iStartYofNewPic = PictureLayout���ĵ�Y���� - iStartYofOldPic
     */
    iDeltaY = g_iYofZoomedPicShowInCenter - iStartYofNewPic;
    iStartYofOldPic = (g_tManualPictureLayout.iTopLeftY + iPictureLayoutHeight / 2) - iDeltaY;

    if (iStartYofOldPic < g_tManualPictureLayout.iTopLeftY)
    {
        iStartYofOldPic = g_tManualPictureLayout.iTopLeftY;
    }
    if (iStartYofOldPic > g_tManualPictureLayout.iBotRightY)
    {
        iStartYofOldPic = g_tManualPictureLayout.iBotRightY + 1;
    }
    
    if ((ptZoomedPicPixelDatas->iHeight - iStartYofNewPic) > (g_tManualPictureLayout.iBotRightY - iStartYofOldPic + 1))
    {
        iHeightPictureInPlay = (g_tManualPictureLayout.iBotRightY - iStartYofOldPic + 1);
    }
    else
    {
        iHeightPictureInPlay = (ptZoomedPicPixelDatas->iHeight - iStartYofNewPic);
    }
        
    ClearVideoMemRegion(ptVideoMem, &g_tManualPictureLayout, COLOR_BACKGROUND);
    PicMergeRegion(iStartXofNewPic, iStartYofNewPic, iStartXofOldPic, iStartYofOldPic, iWidthPictureInPlay, iHeightPictureInPlay, ptZoomedPicPixelDatas, &ptVideoMem->tPixelDatas);
}

/**********************************************************************
 * �������ƣ� ManualPageRun
 * ���������� "manualҳ��"�����к���: ��ʾ�˵�ͼ��,��ʾͼƬ,�����û�����������Ӧ(��������/�ƶ�ͼƬ��)
 * ��������� ptParentPageParams - �ں���һ��ҳ��(��ҳ��)�Ĳ���
 *                                 ptParentPageParams->iPageID����ID("main")ʱ,��ʾ"���ҳ��";������ʾͼƬ
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static void ManualPageRun(PT_PageParams ptParentPageParams)
{
	T_InputEvent tInputEvent;
	T_InputEvent tPreInputEvent;
	int bButtonPressed = 0;
	int bPicSlipping = 0;
	int iIndexPressed = -1;
    int iIndex;
    T_PageParams tPageParams;
    int iError;
    char strDirName[256];
    char strFileName[256];
    char strFullPathName[256];
    PT_DirContent *aptDirContents;
    int iDirContentsNumber;
    int iPicFileIndex;
    char *pcTmp;
    PT_VideoMem ptDevVideoMem;
    int iZoomedWidth;
    int iZoomedHeight;
    PT_PixelDatas ptZoomedPicPixelDatas = &g_tZoomedPicPixelDatas;

    /* ������ֻ�Ǳ�����뾯�� */
    tPreInputEvent.iX = 0;
    tPreInputEvent.iY = 0;

    tPageParams.iPageID = ID("manual");
    
    ptDevVideoMem = GetDevVideoMem();
    strcpy(strFullPathName, ptParentPageParams->strCurPictureFile);

    /* ��ʾ�˵���ͼƬ�ļ� */
    ShowManualPage(&g_tManualPageMenuIconsLayout, strFullPathName);

    /* ȡ��Ŀ¼�� */
    strcpy(strDirName, ptParentPageParams->strCurPictureFile);
    pcTmp = strrchr(strDirName, '/');
    *pcTmp = '\0';

    /* ȡ���ļ��� */
    strcpy(strFileName, pcTmp+1);

    /* ��õ�ǰĿ¼������Ŀ¼���ļ������� */
    iError = GetDirContents(strDirName, &aptDirContents, &iDirContentsNumber);

    /* ȷ����ǰ��ʾ������һ���ļ� */
    for (iPicFileIndex = 0; iPicFileIndex < iDirContentsNumber; iPicFileIndex++)
    {
        if (0 == strcmp(strFileName, aptDirContents[iPicFileIndex]->strName))
        {
            break;
        }
    }

    while (1)
    {
        /* ��ȷ���Ƿ����˲˵�ͼ�� */
        iIndex = ManualPageGetInputEvent(&g_tManualPageMenuIconsLayout, &tInputEvent);
		
		/* ������ɿ� */
        if (tInputEvent.iPressure == 0)
        {
			bPicSlipping = 0;
			
            if (bButtonPressed)
            {
                /* �����а�ť������ */
                ReleaseButton(&g_atManualMenuIconsLayout[iIndexPressed]);
                bButtonPressed = 0;

                if (iIndexPressed == iIndex) /* ���º��ɿ�����ͬһ����ť */
                {
                    switch (iIndexPressed)
                    {
                        case 0: /* ���ذ�ť */
                        {
                            return;
                            break;
                        }
                        case 1: /* ��С��ť */
                        {
                            /* �����С������� */
                            iZoomedWidth  = (float)g_tZoomedPicPixelDatas.iWidth * ZOOM_RATIO;
                            iZoomedHeight = (float)g_tZoomedPicPixelDatas.iHeight * ZOOM_RATIO;
                            ptZoomedPicPixelDatas = GetZoomedPicPixelDatas(&g_tOriginPicPixelDatas, iZoomedWidth, iZoomedHeight);

                            /* ���¼������ĵ� */
                            g_iXofZoomedPicShowInCenter = (float)g_iXofZoomedPicShowInCenter * ZOOM_RATIO;
                            g_iYofZoomedPicShowInCenter = (float)g_iYofZoomedPicShowInCenter * ZOOM_RATIO;

                            /* ��ʾ������ */
                            ShowZoomedPictureInLayout(ptZoomedPicPixelDatas, ptDevVideoMem);

                            break;
                        }
                        case 2: /* �Ŵ�ť */
                        {
                            /* ��÷Ŵ������� */
                            iZoomedWidth  = (float)g_tZoomedPicPixelDatas.iWidth / ZOOM_RATIO;
                            iZoomedHeight = (float)g_tZoomedPicPixelDatas.iHeight / ZOOM_RATIO;
                            ptZoomedPicPixelDatas = GetZoomedPicPixelDatas(&g_tOriginPicPixelDatas, iZoomedWidth, iZoomedHeight);

                            /* ���¼������ĵ� */
                            g_iXofZoomedPicShowInCenter = (float)g_iXofZoomedPicShowInCenter / ZOOM_RATIO;
                            g_iYofZoomedPicShowInCenter = (float)g_iYofZoomedPicShowInCenter / ZOOM_RATIO;

                            /* ��ʾ������ */
                            ShowZoomedPictureInLayout(ptZoomedPicPixelDatas, ptDevVideoMem);

                            break;
                        }
                        case 3: /* "��һ��"��ť */
                        {
                            while (iPicFileIndex > 0)
                            {
                                iPicFileIndex--;
                                snprintf(strFullPathName, 256, "%s/%s", strDirName, aptDirContents[iPicFileIndex]->strName);
                                strFullPathName[255] = '\0';
                                
                                if (isPictureFileSupported(strFullPathName))
                                {
                                    ShowPictureInManualPage(ptDevVideoMem, strFullPathName);
                                    break;
                                }
                            }
                            
                            break;
                        }
                        case 4: /* "��һ��"��ť */
                        {
                            while (iPicFileIndex < iDirContentsNumber - 1)
                            {
                                iPicFileIndex++;
                                snprintf(strFullPathName, 256, "%s/%s", strDirName, aptDirContents[iPicFileIndex]->strName);
                                strFullPathName[255] = '\0';

                                if (isPictureFileSupported(strFullPathName))
                                {
                                    ShowPictureInManualPage(ptDevVideoMem, strFullPathName);
                                    break;
                                }
                            }
                            break;
                        }
                        case 5: /* "����"��ť */
                        {
                            /* Manualҳ��Ĵ�������������: ����ҳ�水"���ģʽ"����"���ҳ��"->"ѡ��ĳ���ļ�", ��"����ҳ��"����������ʾ��ͼƬ 
                             * ����Ǻ���, ֱ��return�Ϳ�����:��Ϊreturn���Ƿ��ص�"����ҳ��"��, �������"����"
                             */
                            if (ptParentPageParams->iPageID == ID("browse"))  /* ������"���ҳ��" */
                            {
                                strcpy(tPageParams.strCurPictureFile, strFullPathName);
                                Page("auto")->Run(&tPageParams);
                                ShowManualPage(&g_tManualPageMenuIconsLayout, tPageParams.strCurPictureFile);
                            }
                            else /* ��ǰmanualҳ��ĸ�ҳ����autoҳ��, ֱ�ӷ��ؼ��� */
                            {
                                return;
                            }
                            break;
                        }
                        default:
                        {
                            break;
                        }
                    }
                }
                
                iIndexPressed = -1;
            }
        }
        else /* ����״̬ */
        {
            /* ������ǲ˵���ť */
            if (iIndex != -1)
            {
                if (!bButtonPressed)
                {
                    /* δ�����°�ť */
                    bButtonPressed = 1;
                    iIndexPressed = iIndex;
                    PressButton(&g_atManualMenuIconsLayout[iIndexPressed]);
                }
            }
			else  /* �������ͼƬ��ʾ����, ����ͼƬ */
			{
				/* ���û�а�ť������ */
				if (!bButtonPressed && !bPicSlipping)
				{
					bPicSlipping = 1;
                    tPreInputEvent = tInputEvent;
				}

				if (bPicSlipping)
				{
                    /* ������㻬��������ڹ涨ֵ, ��Ų��ͼƬ */
                    if (DistanceBetweenTwoPoint(&tInputEvent, &tPreInputEvent) > SLIP_MIN_DISTANCE)
                    {                            
                        /* ���¼������ĵ� */
                        g_iXofZoomedPicShowInCenter -= (tInputEvent.iX - tPreInputEvent.iX);
                        g_iYofZoomedPicShowInCenter -= (tInputEvent.iY - tPreInputEvent.iY);
                        
                        /* ��ʾ������ */
                        ShowZoomedPictureInLayout(ptZoomedPicPixelDatas, ptDevVideoMem);
                        
                        /* ��¼������ */
                        tPreInputEvent = tInputEvent;                            
                    }
				}
			}
        }       
    }
}

static T_PageAction g_tManualPageAction = {
	.name          = "manual",
	.Run           = ManualPageRun,
};


/**********************************************************************
 * �������ƣ� ManualPageInit
 * ���������� ע��"manualҳ��"
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int ManualPageInit(void)
{
	return RegisterPageAction(&g_tManualPageAction);
}

