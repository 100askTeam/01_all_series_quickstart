#include <config.h>
#include <render.h>
#include <stdlib.h>
#include <file.h>
#include <fonts_manager.h>
#include <string.h>


/* ͼ����һ��������, "ͼ��+����"Ҳ��һ��������
 *   --------
 *   |  ͼ  |
 *   |  ��  |
 * ------------
 * |   ����   |
 * ------------
 */

#define DIR_FILE_ICON_WIDTH    40
#define DIR_FILE_ICON_HEIGHT   DIR_FILE_ICON_WIDTH
#define DIR_FILE_NAME_HEIGHT   20
#define DIR_FILE_NAME_WIDTH   (DIR_FILE_ICON_HEIGHT + DIR_FILE_NAME_HEIGHT)
#define DIR_FILE_ALL_WIDTH    DIR_FILE_NAME_WIDTH
#define DIR_FILE_ALL_HEIGHT   DIR_FILE_ALL_WIDTH


/* browseҳ�������ʾ�����Ϊ"�˵�"��"Ŀ¼���ļ�"
 * "�˵�"����"up, select,pre_page,next_page"�ĸ��ɲ�����ͼ��
 * "Ŀ¼���ļ�"�����������
 */

/* �˵������� */
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

/* Ŀ¼���ļ������� */
static char *g_strDirClosedIconName  = "fold_closed.bmp";
static char *g_strDirOpenedIconName  = "fold_opened.bmp";
static char *g_strFileIconName = "file.bmp";
static T_Layout *g_atDirAndFileLayout;
static T_PageLayout g_tBrowsePageDirAndFileLayout = {
	.iMaxTotalBytes = 0,
	//.atLayout       = g_atDirAndFileLayout,
};

static int g_iDirFileNumPerCol, g_iDirFileNumPerRow;

/* ��������ĳĿ¼������� */
static PT_DirContent *g_aptDirContents;  /* ����:����Ŀ¼��"������Ŀ¼","�ļ�"������ */
static int g_iDirContentsNumber;         /* g_aptDirContents�����ж����� */
static int g_iStartIndex = 0;            /* ����Ļ����ʾ�ĵ�1��"Ŀ¼���ļ�"��g_aptDirContents���������һ�� */

/* ��ǰ��ʾ��Ŀ¼ */
static char g_strCurDir[256] = DEFAULT_DIR;
static char g_strSelectedDir[256] = DEFAULT_DIR;

static T_PixelDatas g_tDirClosedIconPixelDatas;
static T_PixelDatas g_tDirOpenedIconPixelDatas;
static T_PixelDatas g_tFileIconPixelDatas;


/**********************************************************************
 * �������ƣ� GetSelectedDir
 * ���������� ��"���ҳ��"��,�û����ܻ�ѡ��ĳ��Ŀ¼-----������"����ҳ��"Ҫ��ʾ��Ŀ¼
 *            �������������Ŀ¼��
 * ��������� ��
 * ��������� strSeletedDir - ��������û�ѡ�е�Ŀ¼������
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
void GetSelectedDir(char *strSeletedDir)
{
    strncpy(strSeletedDir, g_strSelectedDir, 256);
    strSeletedDir[255] = '\0';
}

/**********************************************************************
 * �������ƣ� CalcBrowsePageMenusLayout
 * ���������� ����ҳ���и�ͼ������ֵ
 * ��������� ��
 * ��������� ptPageLayout - �ں���ͼ������Ͻ�/���½�����ֵ
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
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
		
		/* returnͼ�� */
		atLayout[0].iTopLeftY  = 0;
		atLayout[0].iBotRightY = atLayout[0].iTopLeftY + iHeight - 1;
		atLayout[0].iTopLeftX  = 0;
		atLayout[0].iBotRightX = atLayout[0].iTopLeftX + iWidth - 1;

		/* upͼ�� */
		atLayout[1].iTopLeftY  = 0;
		atLayout[1].iBotRightY = atLayout[1].iTopLeftY + iHeight - 1;
		atLayout[1].iTopLeftX  = atLayout[0].iBotRightX + 1;
		atLayout[1].iBotRightX = atLayout[1].iTopLeftX + iWidth - 1;

		/* selectͼ�� */
		atLayout[2].iTopLeftY  = 0;
		atLayout[2].iBotRightY = atLayout[2].iTopLeftY + iHeight - 1;
		atLayout[2].iTopLeftX  = atLayout[1].iBotRightX + 1;
		atLayout[2].iBotRightX = atLayout[2].iTopLeftX + iWidth - 1;

		/* pre_pageͼ�� */
		atLayout[3].iTopLeftY  = 0;
		atLayout[3].iBotRightY = atLayout[3].iTopLeftY + iHeight - 1;
		atLayout[3].iTopLeftX  = atLayout[2].iBotRightX + 1;
		atLayout[3].iBotRightX = atLayout[3].iTopLeftX + iWidth - 1;
#if 0
		/* next_pageͼ�� */
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

		/* returnͼ�� */
		atLayout[0].iTopLeftY  = 0;
		atLayout[0].iBotRightY = atLayout[0].iTopLeftY + iHeight - 1;
		atLayout[0].iTopLeftX  = 0;
		atLayout[0].iBotRightX = atLayout[0].iTopLeftX + iWidth - 1;
		
		/* upͼ�� */
		atLayout[1].iTopLeftY  = atLayout[0].iBotRightY+ 1;
		atLayout[1].iBotRightY = atLayout[1].iTopLeftY + iHeight - 1;
		atLayout[1].iTopLeftX  = 0;
		atLayout[1].iBotRightX = atLayout[1].iTopLeftX + iWidth - 1;
		
		/* selectͼ�� */
		atLayout[2].iTopLeftY  = atLayout[1].iBotRightY + 1;
		atLayout[2].iBotRightY = atLayout[2].iTopLeftY + iHeight - 1;
		atLayout[2].iTopLeftX  = 0;
		atLayout[2].iBotRightX = atLayout[2].iTopLeftX + iWidth - 1;
		
		/* pre_pageͼ�� */
		atLayout[3].iTopLeftY  = atLayout[2].iBotRightY + 1;
		atLayout[3].iBotRightY = atLayout[3].iTopLeftY + iHeight - 1;
		atLayout[3].iTopLeftX  = 0;
		atLayout[3].iBotRightX = atLayout[3].iTopLeftX + iWidth - 1;
#if 0		
		/* next_pageͼ�� */
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
 * �������ƣ� CalcBrowsePageDirAndFilesLayout
 * ���������� ����"Ŀ¼���ļ�"����ʾ����
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
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
		 *    up select pre_page next_page ͼ��
		 * --------------------------------------
		 *
		 *           Ŀ¼���ļ�
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
		 *             |     Ŀ¼���ļ�
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

    /* ȷ��һ����ʾ���ٸ�"Ŀ¼���ļ�", ��ʾ������ */
    iIconWidth  = DIR_FILE_NAME_WIDTH;
    iIconHeight = iIconWidth;

    /* ͼ��֮��ļ��Ҫ����10������ */
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

    /* ÿ��ͼ��֮��ļ�� */
    iDeltaX = iDeltaX / (iNumPerRow + 1);
    iDeltaY = iDeltaY / (iNumPerCol + 1);

    g_iDirFileNumPerRow = iNumPerRow;
    g_iDirFileNumPerCol = iNumPerCol;


    /* ������ʾ iNumPerRow * iNumPerCol��"Ŀ¼���ļ�"
     * ����"����+1"��T_Layout�ṹ��: һ��������ʾͼ��,��һ��������ʾ����
     * ���һ��������NULL,�����жϽṹ�������ĩβ
     */
	g_atDirAndFileLayout = malloc(sizeof(T_Layout) * (2 * iNumPerRow * iNumPerCol + 1));
    if (NULL == g_atDirAndFileLayout)
    {
        DBG_PRINTF("malloc error!\n");
        return -1;
    }

    /* "Ŀ¼���ļ�"������������Ͻǡ����½����� */
    g_tBrowsePageDirAndFileLayout.iTopLeftX      = iTopLeftX;
    g_tBrowsePageDirAndFileLayout.iBotRightX     = iBotRightX;
    g_tBrowsePageDirAndFileLayout.iTopLeftY      = iTopLeftY;
    g_tBrowsePageDirAndFileLayout.iBotRightY     = iBotRightY;
    g_tBrowsePageDirAndFileLayout.iBpp           = iBpp;
    g_tBrowsePageDirAndFileLayout.atLayout       = g_atDirAndFileLayout;
    g_tBrowsePageDirAndFileLayout.iMaxTotalBytes = DIR_FILE_ALL_WIDTH * DIR_FILE_ALL_HEIGHT * iBpp / 8;

    
    /* ȷ��ͼ������ֵ�λ�� 
     *
     * ͼ����һ��������, "ͼ��+����"Ҳ��һ��������
     *   --------
     *   |  ͼ  |
     *   |  ��  |
     * ------------
     * |   ����   |
     * ------------
     */
    iTopLeftX += iDeltaX;
    iTopLeftY += iDeltaY;
    iTopLeftXBak = iTopLeftX;
    for (i = 0; i < iNumPerCol; i++)
    {        
        for (j = 0; j < iNumPerRow; j++)
        {
            /* ͼ�� */
            g_atDirAndFileLayout[k].iTopLeftX  = iTopLeftX + (DIR_FILE_NAME_WIDTH - DIR_FILE_ICON_WIDTH) / 2;
            g_atDirAndFileLayout[k].iBotRightX = g_atDirAndFileLayout[k].iTopLeftX + DIR_FILE_ICON_WIDTH - 1;
            g_atDirAndFileLayout[k].iTopLeftY  = iTopLeftY;
            g_atDirAndFileLayout[k].iBotRightY = iTopLeftY + DIR_FILE_ICON_HEIGHT - 1;

            /* ���� */
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

    /* ��β */
    g_atDirAndFileLayout[k].iTopLeftX   = 0;
    g_atDirAndFileLayout[k].iBotRightX  = 0;
    g_atDirAndFileLayout[k].iTopLeftY   = 0;
    g_atDirAndFileLayout[k].iBotRightY  = 0;
    g_atDirAndFileLayout[k].strIconName = NULL;

    return 0;
}

/**********************************************************************
 * �������ƣ� BrowsePageGetInputEvent
 * ���������� Ϊ"���ҳ��"�����������,�ж������¼�λ����һ��ͼ����
 * ��������� ptPageLayout - �ں����ͼ�����ʾ����
 * ��������� ptInputEvent - �ں��õ�����������
 * �� �� ֵ�� -1     - �������ݲ�λ���κ�һ��ͼ��֮��
 *            ����ֵ - �������������ڵ�ͼ��(PageLayout->atLayout�������һ��)
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static int BrowsePageGetInputEvent(PT_PageLayout ptPageLayout, PT_InputEvent ptInputEvent)
{
	return GenericGetInputEvent(ptPageLayout, ptInputEvent);
}

/**********************************************************************
 * �������ƣ� GetInputPositionInPageLayout
 * ���������� Ϊ"���ҳ��"�����������,�ж������¼�λ����һ��"Ŀ¼���ļ�"��
 * ��������� ptPageLayout - �ں�"Ŀ¼���ļ�"����ʾ����
 * ��������� ptInputEvent - �ں��õ�����������
 * �� �� ֵ�� -1     - �������ݲ�λ���κ�һ��ͼ��֮��
 *            ����ֵ - �������������ڵ�ͼ��(PageLayout->atLayout�������һ��)
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static int GetInputPositionInPageLayout(PT_PageLayout ptPageLayout, PT_InputEvent ptInputEvent)
{
    int i = 0;
    PT_Layout atLayout = ptPageLayout->atLayout;
        
    /* �������� */
    /* ȷ������λ����һ����ť�� */
    while (atLayout[i].iBotRightY)
    {
        if ((ptInputEvent->iX >= atLayout[i].iTopLeftX) && (ptInputEvent->iX <= atLayout[i].iBotRightX) && \
             (ptInputEvent->iY >= atLayout[i].iTopLeftY) && (ptInputEvent->iY <= atLayout[i].iBotRightY))
        {
            /* �ҵ��˱����еİ�ť */
            return i;
        }
        else
        {
            i++;
        }           
    }

    /* ����û�����ڰ�ť�� */
    return -1;
}


/**********************************************************************
 * �������ƣ� GenerateBrowsePageDirAndFile
 * ���������� Ϊ"���ҳ��"����"Ŀ¼���ļ�"�����е�ͼ�������,������ʾĿ¼����
 * ��������� iStartIndex        - ����Ļ����ʾ�ĵ�1��"Ŀ¼���ļ�"��aptDirContents���������һ��
 *            iDirContentsNumber - aptDirContents�����ж�����
 *            aptDirContents     - ����:����Ŀ¼��"������Ŀ¼","�ļ�"������ 
 *            ptVideoMem         - �����VideoMem�й���ҳ��
 * ��������� ��
 * �� �� ֵ�� 0      - �ɹ�
 *            ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
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
                /* ��ʾĿ¼���ļ���ͼ�� */
                if (aptDirContents[iDirContentIndex]->eFileType == FILETYPE_DIR)
                {
                    PicMerge(atLayout[k].iTopLeftX, atLayout[k].iTopLeftY, &g_tDirClosedIconPixelDatas, &ptVideoMem->tPixelDatas);
                }
                else
                {
                    PicMerge(atLayout[k].iTopLeftX, atLayout[k].iTopLeftY, &g_tFileIconPixelDatas, &ptVideoMem->tPixelDatas);
                }

                k++;
                /* ��ʾĿ¼���ļ������� */
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
 * �������ƣ� GenerateDirAndFileIcons
 * ���������� Ϊ"���ҳ��"���ɲ˵������е�ͼ��
 * ��������� ptPageLayout - �ں����ͼ����ļ�������ʾ����
 * ��������� ��
 * �� �� ֵ�� 0      - �ɹ�
 *            ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static int GenerateDirAndFileIcons(PT_PageLayout ptPageLayout)
{
	T_PixelDatas tOriginIconPixelDatas;
    int iError;
	int iXres, iYres, iBpp;
    PT_Layout atLayout = ptPageLayout->atLayout;

	GetDispResolution(&iXres, &iYres, &iBpp);

    /* ��Ŀ¼ͼ�ꡢ�ļ�ͼ������ڴ� */
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

    /* ��BMP�ļ�����ȡͼ������ */
    /* 1. ��ȡ"fold_closedͼ��" */
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

    /* 2. ��ȡ"fold_openedͼ��" */
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

    /* 3. ��ȡ"fileͼ��" */
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
 * �������ƣ� ShowBrowsePage
 * ���������� ��ʾ"���ҳ��"
 * ��������� ptPageLayout - �ں����ͼ����ļ�������ʾ����
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static void ShowBrowsePage(PT_PageLayout ptPageLayout)
{
	PT_VideoMem ptVideoMem;
	int iError;

	PT_Layout atLayout = ptPageLayout->atLayout;
		
	/* 1. ����Դ� */
	ptVideoMem = GetVideoMem(ID("browse"), 1);
	if (ptVideoMem == NULL)
	{
		DBG_PRINTF("can't get video mem for browse page!\n");
		return;
	}

	/* 2. �軭���� */

	/* �����û�м������ͼ������� */
	if (atLayout[0].iTopLeftX == 0)
	{
		CalcBrowsePageMenusLayout(ptPageLayout);
        CalcBrowsePageDirAndFilesLayout();
	}

    /* �����û������"Ŀ¼���ļ�"��ͼ�� */
    if (!g_tDirClosedIconPixelDatas.aucPixelDatas)
    {
        GenerateDirAndFileIcons(&g_tBrowsePageDirAndFileLayout);
    }

	iError = GeneratePage(ptPageLayout, ptVideoMem);
    iError = GenerateBrowsePageDirAndFile(g_iStartIndex, g_iDirContentsNumber, g_aptDirContents, ptVideoMem);

	/* 3. ˢ���豸��ȥ */
	FlushVideoMemToDev(ptVideoMem);

	/* 4. ����Դ� */
	PutVideoMem(ptVideoMem);
}


/**********************************************************************
 * �������ƣ� SelectDirFileIcon
 * ���������� �ı���ʾ�豸�ϵ�"Ŀ¼���ļ�"��ͼ��, ��ʾ"�Ѿ���ѡ��"
 *            ����Ŀ¼ͼ��, ������Ϊ"file_openedͼ��"
 *            �����ļ�ͼ��, ֻ�ǰ�ͼ�귴ɫ
 * ��������� iDirFileIndex - ѡ��"Ŀ¼���ļ�"��������һ��ͼ��
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static void SelectDirFileIcon(int iDirFileIndex)
{
    int iDirFileContentIndex;
    PT_VideoMem ptDevVideoMem;

    ptDevVideoMem = GetDevVideoMem();

    iDirFileIndex = iDirFileIndex & ~1;    
    iDirFileContentIndex = g_iStartIndex + iDirFileIndex/2;

    /* �����Ŀ¼, �ı�ͼ��Ϊ"file_openedͼ��" */
    if (g_aptDirContents[iDirFileContentIndex]->eFileType == FILETYPE_DIR)
    {
        PicMerge(g_atDirAndFileLayout[iDirFileIndex].iTopLeftX, g_atDirAndFileLayout[iDirFileIndex].iTopLeftY, &g_tDirOpenedIconPixelDatas, &ptDevVideoMem->tPixelDatas);
    }
    else /* ������ļ�, ��ɫȡ�� */
    {
        PressButton(&g_atDirAndFileLayout[iDirFileIndex]);
        PressButton(&g_atDirAndFileLayout[iDirFileIndex + 1]);
    }
}

/**********************************************************************
 * �������ƣ� DeSelectDirFileIcon
 * ���������� �ı���ʾ�豸�ϵ�"Ŀ¼���ļ�"��ͼ��, ��ʾ"δ��ѡ��"
 *            ����Ŀ¼ͼ��, ������Ϊ"file_closedͼ��"
 *            �����ļ�ͼ��, ֻ�ǰ�ͼ�귴ɫ
 * ��������� iDirFileIndex - ѡ��"Ŀ¼���ļ�"��������һ��ͼ��
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
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
 * �������ƣ� BrowsePageRun
 * ���������� "���ҳ��"�����к���: ��ʾ�˵�ͼ��,��ʾĿ¼����,��ȡ�������ݲ�������Ӧ
 *            "���ҳ��"����������: �˵�ͼ��, Ŀ¼���ļ�ͼ��
 *             Ϊͳһ����, "�˵�ͼ��"�����Ϊ0,1,2,3,..., "Ŀ¼���ļ�ͼ��"�����Ϊ1000,1001,1002,....
 * ��������� ptParentPageParams - �ں���һ��ҳ��(��ҳ��)�Ĳ���
 *                                 ptParentPageParams->iPageID����ID("setting")ʱ,��ҳ���������/ѡ���ļ���, ����ļ��޷�Ӧ 
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static void BrowsePageRun(PT_PageParams ptParentPageParams)
{
	int iIndex;
	T_InputEvent tInputEvent;
	T_InputEvent tInputEventPrePress;

    int bUsedToSelectDir = 0;
	int bIconPressed = 0;          /* �������Ƿ���ͼ����ʾΪ"������" */

    /* "��������ͼƬ"ʱ, �Ǵ���һ��Ŀ¼��ȡ�ļ���? ������"ѡ��"��ť��ѡ���Ŀ¼��
     * ���Ŀ¼ͼ�꽫������һ��Ŀ¼, ������ѡ��Ŀ¼��?
     * 1. �ȵ��"ѡ��"��ť
     * 2. �ٵ��ĳ��Ŀ¼ͼ��
     */
    int bHaveClickSelectIcon = 0;
    
	int iIndexPressed = -1;    /* �����µ�ͼ�� */
    int iDirFileContentIndex;
    
    int iError;
    PT_VideoMem ptDevVideoMem;

    T_PageParams tPageParams;
    char strTmp[256];
    char *pcTmp;

	/* ������ֻ��Ϊ�˱�����뾯�� */
	tInputEventPrePress.tTime.tv_sec = 0;
	tInputEventPrePress.tTime.tv_usec = 0;

    if (ptParentPageParams->iPageID == ID("setting"))  /* ����Ǵ�"����ҳ��"�����˱�ҳ��, ��ҳ��������ѡ��Ŀ¼�� */
    {
        bUsedToSelectDir = 1;
    }

    ptDevVideoMem = GetDevVideoMem();

    /* 0. ���Ҫ��ʾ��Ŀ¼������ */
    iError = GetDirContents(g_strCurDir, &g_aptDirContents, &g_iDirContentsNumber);
    if (iError)
    {
        DBG_PRINTF("GetDirContents error!\n");
        return;
    }
    	
	/* 1. ��ʾҳ�� */
	ShowBrowsePage(&g_tBrowsePageMenuIconsLayout);

	/* 2. ����Prepare�߳� */

	/* 3. ����GetInputEvent��������¼����������� */
	while (1)
	{
        /* ��ȷ���Ƿ����˲˵�ͼ�� */
		iIndex = BrowsePageGetInputEvent(&g_tBrowsePageMenuIconsLayout, &tInputEvent);

        /* ������㲻�ڲ˵�ͼ����, ���ж�������һ��"Ŀ¼���ļ�"�� */
        if (iIndex == -1)
        {
            iIndex = GetInputPositionInPageLayout(&g_tBrowsePageDirAndFileLayout, &tInputEvent);
            if (iIndex != -1)
            {                
                if (g_iStartIndex + iIndex / 2 < g_iDirContentsNumber)  /* �ж�����������Ƿ���ͼ�� */
                    iIndex += DIRFILE_ICON_INDEX_BASE; /* ����"Ŀ¼���ļ�ͼ��" */
                else
                    iIndex = -1;
            }
        }
        
        /* ������ɿ� */
		if (tInputEvent.iPressure == 0)
		{
            /* �����ǰ��ͼ�걻���� */
			if (bIconPressed)
			{
                 if (iIndexPressed < DIRFILE_ICON_INDEX_BASE)  /* �˵�ͼ�� */
                 {
                    /* �ͷ�ͼ�� */
                    if (!(bUsedToSelectDir && (iIndexPressed == 1))) /* "ѡ��"ͼ�굥������ */
                    {
        				ReleaseButton(&g_atMenuIconsLayout[iIndexPressed]);
                    }

                    bIconPressed    = 0;

				    if (iIndexPressed == iIndex) /* ���º��ɿ�����ͬһ����ť */
    				{
    					switch (iIndex)
    					{
    						case 0: /* "����"��ť */
    						{
                                if (0 == strcmp(g_strCurDir, "/"))  /* �Ѿ��Ƕ���Ŀ¼ */
                                {
                                    FreeDirContents(g_aptDirContents, g_iDirContentsNumber);
                                    return ;
                                }

                                pcTmp = strrchr(g_strCurDir, '/'); /* �ҵ����һ��'/', ����ȥ�� */
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
                            case 1: /* "ѡ��" */
                            {
                                if (!bUsedToSelectDir)
                                {
                                    /* �����������"ѡ��Ŀ¼", �ð�ť���ô� */
                                    break;
                                }
                                
                                if (!bHaveClickSelectIcon)  /* ��1�ε��"ѡ��"��ť */
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
                            case 2: /* "��һҳ" */
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
                            case 3: /* "��һҳ" */
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
                else /* "Ŀ¼���ļ�ͼ��" */
                {

                    /*
                     * ������º��ɿ�ʱ, ���㲻����ͬһ��ͼ����, ���ͷ�ͼ��
                     */
                    if (iIndexPressed != iIndex)
                    {
                        DeSelectDirFileIcon(iIndexPressed - DIRFILE_ICON_INDEX_BASE);
                        bIconPressed      = 0;
                    }
                    else if (bHaveClickSelectIcon) /* ���º��ɿ�����ͬһ����ť, ����"ѡ��"��ť�ǰ���״̬ */
                    {
                        DeSelectDirFileIcon(iIndexPressed - DIRFILE_ICON_INDEX_BASE);
                        bIconPressed    = 0;
                        
                        /* �����Ŀ¼, ��¼���Ŀ¼ */
                        iDirFileContentIndex = g_iStartIndex + (iIndexPressed - DIRFILE_ICON_INDEX_BASE)/2;
                        if (g_aptDirContents[iDirFileContentIndex]->eFileType == FILETYPE_DIR)
                        {
                            ReleaseButton(&g_atMenuIconsLayout[1]);  /* ͬʱ�ɿ�"ѡ��ť" */
                            bHaveClickSelectIcon = 0;

                            /* ��¼Ŀ¼�� */
                            snprintf(strTmp, 256, "%s/%s", g_strCurDir, g_aptDirContents[iDirFileContentIndex]->strName);
                            strTmp[255] = '\0';
                            strcpy(g_strSelectedDir, strTmp);
                        }
                    }
                    else  /* "ѡ��"��ť��������ʱ, ����Ŀ¼�����, bUsedToSelectDirΪ0ʱ�����ļ�����ʾ�� */
                    {
                        DeSelectDirFileIcon(iIndexPressed - DIRFILE_ICON_INDEX_BASE);
                        bIconPressed    = 0;
                        
                        /* �����Ŀ¼, �������Ŀ¼ */
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
                        else if (bUsedToSelectDir == 0) /* bUsedToSelectDirΪ0ʱ�����ļ�ʱ��ʾ�� */
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
		else /* ����״̬ */
		{			
			if (iIndex != -1)
			{
                if (!bIconPressed)  /* ֮ǰδ���а�ť������ */
                {
    				bIconPressed = 1;
    				iIndexPressed = iIndex;                    
					tInputEventPrePress = tInputEvent;  /* ��¼���� */
                    if (iIndex < DIRFILE_ICON_INDEX_BASE)  /* �˵�ͼ�� */
                    {
                        if (bUsedToSelectDir)
                        {
                            if (!(bHaveClickSelectIcon && (iIndexPressed == 1)))  /* ����Ѿ�����"ѡ��"��ť, ��Ȼ�����ٴη�ת��ͼ�� */
            					PressButton(&g_atMenuIconsLayout[iIndex]);
                        }
                        else
                        {
                            if (!bHaveClickSelectIcon)
            					PressButton(&g_atMenuIconsLayout[iIndex]);
                        }
                    }
                    else   /* Ŀ¼���ļ�ͼ�� */
                    {
                        SelectDirFileIcon(iIndex - DIRFILE_ICON_INDEX_BASE);
                    }
                }

                /* ����"����"��ť, ���� */
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
 * �������ƣ� BrowsePageInit
 * ���������� ע��"���ҳ��"
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int BrowsePageInit(void)
{
	return RegisterPageAction(&g_tBrowsePageAction);
}

