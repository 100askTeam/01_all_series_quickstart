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
 * �������ƣ� RegisterPageAction
 * ���������� ע��"ҳ��ģ��", "ҳ��ģ��"����ҳ����ʾ�ĺ���
 * ��������� ptPageAction - һ���ṹ��,�ں�"ҳ��ģ��"�Ĳ�������
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
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
 * �������ƣ� ShowPages
 * ���������� ��ʾ��������֧�ֵ�"ҳ��ģ��"
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
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
 * �������ƣ� Page
 * ���������� ��������ȡ��ָ����"ҳ��ģ��"
 * ��������� pcName - ����
 * ��������� ��
 * �� �� ֵ�� NULL   - ʧ��,û��ָ����ģ��, 
 *            ��NULL - "ҳ��ģ��"��PT_PageAction�ṹ��ָ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
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
 * �������ƣ� ID
 * ���������� �����������һ��Ψһ������,��������ʶVideoMem�е���ʾ����
 * ��������� strName - ����
 * ��������� ��
 * �� �� ֵ�� һ��Ψһ������
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int ID(char *strName)
{
	return (int)(strName[0]) + (int)(strName[1]) + (int)(strName[2]) + (int)(strName[3]);
}


/**********************************************************************
 * �������ƣ� GeneratePage
 * ���������� ��ͼ���ļ��н�����ͼ�����ݲ�����ָ������,�Ӷ�����ҳ������
 * ��������� ptPageLayout - �ں����ͼ����ļ�������ʾ����
 *            ptVideoMem   - �����VideoMem�ﹹ��ҳ������
 * ��������� ��
 * �� �� ֵ�� 0      - �ɹ�
 *            ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int GeneratePage(PT_PageLayout ptPageLayout, PT_VideoMem ptVideoMem)
{
	T_PixelDatas tOriginIconPixelDatas;
	T_PixelDatas tIconPixelDatas;
	int iError;
	PT_Layout atLayout = ptPageLayout->atLayout;
		
	/* �軭����: VideoMem�е�ҳ������δ���ɵ�����²�ִ��������� */
	if (ptVideoMem->ePicState != PS_GENERATED)
	{
		ClearVideoMem(ptVideoMem, COLOR_BACKGROUND);

		tIconPixelDatas.iBpp          = ptPageLayout->iBpp;
        /* ����һ����ʱ���ڴ�, ������������ź��ͼ������ */
		tIconPixelDatas.aucPixelDatas = malloc(ptPageLayout->iMaxTotalBytes);
		if (tIconPixelDatas.aucPixelDatas == NULL)
		{
			return -1;
		}

		while (atLayout->strIconName)
		{
            /* ȡ��ͼ���ļ����������� */
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

            /* ��ԭʼ��ͼ��������������Ϊָ����С */
 			PicZoom(&tOriginIconPixelDatas, &tIconPixelDatas);

            /* �����ź��ͼ������,�ϲ���VideoMem��ָ������ */
 			PicMerge(atLayout->iTopLeftX, atLayout->iTopLeftY, &tIconPixelDatas, &ptVideoMem->tPixelDatas);

            /* �ͷ�ԭʼ��ͼ���������� */
 			FreePixelDatasForIcon(&tOriginIconPixelDatas);
 			atLayout++;
		}

        /* ��������������ʱ�ڴ� */
		free(tIconPixelDatas.aucPixelDatas);
		ptVideoMem->ePicState = PS_GENERATED;
	}

	return 0;
}



/**********************************************************************
 * �������ƣ� GenerateTextPage
 * ���������� ����������ͼ��
 * ��������� ptPageLayout - �ں����ͼ������ֺ���ʾ����
 *            ptVideoMem   - �����VideoMem�ﹹ��ҳ������
 *            dwBackColor  - ������ɫ
 * ��������� ��
 * �� �� ֵ�� 0      - �ɹ�
 *            ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2020/03/16	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int GenerateTextPage(PT_PageLayout ptPageLayout, PT_VideoMem ptVideoMem, unsigned int dwBackgroundColor)
{
	int iError;
	PT_Layout atLayout = ptPageLayout->atLayout;
	unsigned int dwFontSize;
	int len;
		
	/* �軭����: VideoMem�е�ҳ������δ���ɵ�����²�ִ��������� */
	if (ptVideoMem->ePicState != PS_GENERATED)
	{
		ClearVideoMem(ptVideoMem, COLOR_BACKGROUND);

		while (atLayout->strIconName)
		{
			len = strlen(atLayout->strIconName);
			dwFontSize = atLayout->iBotRightX - atLayout->iTopLeftX;
			dwFontSize = dwFontSize/len;
			SetFontSize(dwFontSize);

			/* ��VideoMem��ָ�����ξ�����ʾ�ַ��� */
			iError = MergerStringToCenterOfRectangleInVideoMem(atLayout->iTopLeftX, atLayout->iTopLeftY, atLayout->iBotRightX, atLayout->iBotRightY, (unsigned char *)atLayout->strIconName, dwBackgroundColor, ptVideoMem);
			iError = iError;
 			atLayout++;
		}

		ptVideoMem->ePicState = PS_GENERATED;
	}

	return 0;
}

/**********************************************************************
 * �������ƣ� GenerateItemForPage
 * ���������� ��ͼ���ļ��н�����ͼ�����ݲ�����ָ������,�Ӷ��޸�ҳ������
 * ��������� strIconName  - �ļ���
 *            ptPageLayout - �ں�1��ͼ�����ʾ����
 *            ptVideoMem   - �����VideoMem�ﹹ��ҳ������
 * ��������� ��
 * �� �� ֵ�� 0      - �ɹ�
 *            ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int GenerateItemForPage(char *strIconName, PT_PageLayout ptPageLayout, int indexLayout, PT_VideoMem ptVideoMem)
{
	T_PixelDatas tOriginIconPixelDatas;
	T_PixelDatas tIconPixelDatas;
	int iError;
	PT_Layout atLayout = &ptPageLayout->atLayout[indexLayout];
		
	/* �軭����: VideoMem�е�ҳ������δ���ɵ�����²�ִ��������� */
	//if (ptVideoMem->ePicState != PS_GENERATED)
	{
		//ClearVideoMem(ptVideoMem, COLOR_BACKGROUND);

		tIconPixelDatas.iBpp          = ptPageLayout->iBpp;
        /* ����һ����ʱ���ڴ�, ������������ź��ͼ������ */
		tIconPixelDatas.aucPixelDatas = malloc(ptPageLayout->iMaxTotalBytes);
		if (tIconPixelDatas.aucPixelDatas == NULL)
		{
			DBG_PRINTF("malloc fail!\n");
			return -1;
		}

        /* ȡ��ͼ���ļ����������� */
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

        /* ��ԭʼ��ͼ��������������Ϊָ����С */
		PicZoom(&tOriginIconPixelDatas, &tIconPixelDatas);

        /* �����ź��ͼ������,�ϲ���VideoMem��ָ������ */
		PicMerge(atLayout->iTopLeftX, atLayout->iTopLeftY, &tIconPixelDatas, &ptVideoMem->tPixelDatas);

        /* �ͷ�ԭʼ��ͼ���������� */
		FreePixelDatasForIcon(&tOriginIconPixelDatas);

        /* ��������������ʱ�ڴ� */
		free(tIconPixelDatas.aucPixelDatas);
		ptVideoMem->ePicState = PS_GENERATED;
	}

	return 0;
}


/**********************************************************************
 * �������ƣ� TimeMSBetween
 * ���������� ����ʱ���ļ��:��λms
 * ��������� tTimeStart - ��ʼʱ���
 *            tTimeEnd   - ����ʱ���
 * ��������� ��
 * �� �� ֵ�� ���,��λms
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int TimeMSBetween(struct timeval tTimeStart, struct timeval tTimeEnd)
{
	int iMS;
	iMS = (tTimeEnd.tv_sec - tTimeStart.tv_sec) * 1000 + (tTimeEnd.tv_usec - tTimeStart.tv_usec) / 1000;
	return iMS;
}

/**********************************************************************
 * �������ƣ� GenericGetInputEvent
 * ���������� ��ȡ��������,���ж���λ����һ��ͼ����
 * ��������� ptPageLayout - �ں����ͼ�����ʾ����
 * ��������� ptInputEvent - �ں��õ�����������
 * �� �� ֵ�� -1     - �������ݲ�λ���κ�һ��ͼ��֮��
 *            ����ֵ - �������������ڵ�ͼ��(PageLayout->atLayout�������һ��)
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int GenericGetInputEvent(PT_PageLayout ptPageLayout, PT_InputEvent ptInputEvent)
{
	T_InputEvent tInputEvent;
	int iRet;
	int i = 0;
	PT_Layout atLayout =NULL;

	if (ptPageLayout)
		atLayout = ptPageLayout->atLayout;
	
	/* ���ԭʼ�Ĵ��������� 
	 * ���ǵ���input_manager.c�ĺ������˺������õ�ǰ�߷�����
	 * ���������̻߳�����ݺ󣬻��������
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
	
	/* �������� */
	/* ȷ������λ����һ����ť�� */
	if (tInputEvent.iType == INPUT_TYPE_TOUCHSCREEN)
	{
    	while (atLayout && atLayout[i].strIconName)
    	{
    		if ((tInputEvent.iX >= atLayout[i].iTopLeftX) && (tInputEvent.iX <= atLayout[i].iBotRightX) && \
    			 (tInputEvent.iY >= atLayout[i].iTopLeftY) && (tInputEvent.iY <= atLayout[i].iBotRightY))
    		{
    			/* �ҵ��˱����еİ�ť */
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
        

	/* ����û�����ڰ�ť�� */
	return -1;
}


/**********************************************************************
 * �������ƣ� GetPageCfg
 * ���������� ���ҳ������ò���,
 *            ������������ҳ��,����Ҫ�õ�2������:�����ĸ�Ŀ¼�µ�ͼƬ,ͼƬ֮��Ĳ��ż��
 * ��������� ��
 * ��������� ptPageCfg - �ں��õ��Ĳ���
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
void GetPageCfg(PT_PageCfg ptPageCfg)
{
//    GetSelectedDir(ptPageCfg->strSeletedDir);
//    GetIntervalTime(&ptPageCfg->iIntervalSecond);
}

/**********************************************************************
 * �������ƣ� PagesInit
 * ���������� ���ø���"ҳ��ģ��"�ĳ�ʼ������,����ע������
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
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


