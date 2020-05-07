
#include <config.h>
#include <disp_manager.h>
#include <string.h>

static PT_DispOpr g_ptDispOprHead;
static PT_DispOpr g_ptDefaultDispOpr;
static PT_VideoMem g_ptVideoMemHead;

/**********************************************************************
 * 函数名称： RegisterDispOpr
 * 功能描述： 注册"显示模块", 把所能支持的显示设备的操作函数放入链表
 * 输入参数： ptDispOpr - 一个结构体,内含显示设备的操作函数
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int RegisterDispOpr(PT_DispOpr ptDispOpr)
{
	PT_DispOpr ptTmp;

	if (!g_ptDispOprHead)
	{
		g_ptDispOprHead   = ptDispOpr;
		ptDispOpr->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptDispOprHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = ptDispOpr;
		ptDispOpr->ptNext = NULL;
	}

	return 0;
}


/**********************************************************************
 * 函数名称： ShowDispOpr
 * 功能描述： 显示本程序能支持的"显示模块"
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
void ShowDispOpr(void)
{
	int i = 0;
	PT_DispOpr ptTmp = g_ptDispOprHead;

	while (ptTmp)
	{
		printf("%02d %s\n", i++, ptTmp->name);
		ptTmp = ptTmp->ptNext;
	}
}

/**********************************************************************
 * 函数名称： GetDispOpr
 * 功能描述： 根据名字取出指定的"显示模块"
 * 输入参数： pcName - 名字
 * 输出参数： 无
 * 返 回 值： NULL   - 失败,没有指定的模块, 
 *            非NULL - 显示模块的PT_DispOpr结构体指针
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
PT_DispOpr GetDispOpr(char *pcName)
{
	PT_DispOpr ptTmp = g_ptDispOprHead;
	
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
 * 函数名称： SelectAndInitDefaultDispDev
 * 功能描述： 根据名字取出指定的"显示模块", 调用它的初始化函数, 并且清屏
 * 输入参数： name - 名字
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
void SelectAndInitDefaultDispDev(char *name)
{
	g_ptDefaultDispOpr = GetDispOpr(name);
	if (g_ptDefaultDispOpr)
	{
		g_ptDefaultDispOpr->DeviceInit();
		g_ptDefaultDispOpr->CleanScreen(0);
	}
}

/**********************************************************************
 * 函数名称： GetDefaultDispDev
 * 功能描述： 程序事先用SelectAndInitDefaultDispDev选择了显示模块,
 *            本函数返回该显示模块
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 显示模块的PT_DispOpr结构体指针
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
PT_DispOpr GetDefaultDispDev(void)
{
	return g_ptDefaultDispOpr;
}

/**********************************************************************
 * 函数名称： GetDispResolution
 * 功能描述： 获得所使用的显示设备的分辨率和BPP
 * 输入参数： 无
 * 输出参数： piXres - 存X分辨率
 *            piYres - 存X分辨率
 *            piBpp  - 存BPP
 * 返 回 值： 0  - 成功
 *            -1 - 失败(未使用SelectAndInitDefaultDispDev来选择显示模块)
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int GetDispResolution(int *piXres, int *piYres, int *piBpp)
{
	if (g_ptDefaultDispOpr)
	{
		*piXres = g_ptDefaultDispOpr->iXres;
		*piYres = g_ptDefaultDispOpr->iYres;
		*piBpp  = g_ptDefaultDispOpr->iBpp;
		return 0;
	}
	else
	{
		return -1;
	}
}

/**********************************************************************
 * 函数名称： AllocVideoMem
 * 功能描述： VideoMem: 为加快显示速度,我们事先在缓存中构造好显示的页面的数据,
 *            (这个缓存称为VideoMem)
 *            显示时再把VideoMem中的数据复制到设备的显存上
 * 输入参数： iNum
 * 输出参数： 无
 * 返 回 值： 0  - 成功
 *            -1 - 失败(未使用SelectAndInitDefaultDispDev来选择显示模块)
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int AllocVideoMem(int iNum)
{
	int i;

	int iXres = 0;
	int iYres = 0;
	int iBpp  = 0;

	int iVMSize;
	int iLineBytes;

	PT_VideoMem ptNew;

	/* 确定VideoMem的大小
	 */
	GetDispResolution(&iXres, &iYres, &iBpp);
	iVMSize = iXres * iYres * iBpp / 8;
	iLineBytes = iXres * iBpp / 8;

	/* 先把设备本身的framebuffer放入链表
	 * 分配一个T_VideoMem结构体, 注意我们没有分配里面的tPixelDatas.aucPixelDatas
	 * 而是让tPixelDatas.aucPixelDatas指向显示设备的framebuffer
	 */
	ptNew = malloc(sizeof(T_VideoMem));
	if (ptNew == NULL)
	{
		return -1;
	}

	/* 指向framebuffer */
	ptNew->tPixelDatas.aucPixelDatas = g_ptDefaultDispOpr->pucDispMem;
	
	ptNew->iID = 0;
	ptNew->bDevFrameBuffer = 1;        /* 表示这个VideoMem是设备本身的framebuffer, 而不是用作缓存作用的VideoMem */
	ptNew->eVideoMemState  = VMS_FREE;
	ptNew->ePicState	   = PS_BLANK;
	ptNew->tPixelDatas.iWidth  = iXres;
	ptNew->tPixelDatas.iHeight = iYres;
	ptNew->tPixelDatas.iBpp    = iBpp;
	ptNew->tPixelDatas.iLineBytes  = iLineBytes;
	ptNew->tPixelDatas.iTotalBytes = iVMSize;

	if (iNum != 0)
	{
		/* 如果下面要分配用于缓存的VideoMem, 
		 * 把设备本身framebuffer对应的VideoMem状态设置为VMS_USED_FOR_CUR,
		 * 表示这个VideoMem不会被作为缓存分配出去
		 */
		ptNew->eVideoMemState = VMS_USED_FOR_CUR;
	}
	
	/* 放入链表 */
	ptNew->ptNext = g_ptVideoMemHead;
	g_ptVideoMemHead = ptNew;
	

	/*
	 * 分配用于缓存的VideoMem
	 */
	for (i = 0; i < iNum; i++)
	{
		/* 分配T_VideoMem结构体本身和"跟framebuffer同样大小的缓存" */
		ptNew = malloc(sizeof(T_VideoMem) + iVMSize);
		if (ptNew == NULL)
		{
			return -1;
		}
		/* 在T_VideoMem结构体里记录上面分配的"跟framebuffer同样大小的缓存" */
		ptNew->tPixelDatas.aucPixelDatas = (unsigned char *)(ptNew + 1);

		ptNew->iID = 0;
		ptNew->bDevFrameBuffer = 0;
		ptNew->eVideoMemState = VMS_FREE;
		ptNew->ePicState      = PS_BLANK;
		ptNew->tPixelDatas.iWidth  = iXres;
		ptNew->tPixelDatas.iHeight = iYres;
		ptNew->tPixelDatas.iBpp    = iBpp;
		ptNew->tPixelDatas.iLineBytes = iLineBytes;
		ptNew->tPixelDatas.iTotalBytes = iVMSize;

		/* 放入链表 */
		ptNew->ptNext = g_ptVideoMemHead;
		g_ptVideoMemHead = ptNew;
	}
	
	return 0;
}

/**********************************************************************
 * 函数名称： GetVideoMem
 * 功能描述： 获得一块可操作的VideoMem(它用于存储要显示的数据), 
 *            用完后用PutVideoMem来释放
 * 输入参数： iID  - ID值,先尝试在众多VideoMem中找到ID值相同的
 *            bCur - 1表示当前程序马上要使用VideoMem,无法如何都要返回一个VideoMem
 *                   0表示这是为了改进性能而提前取得VideoMem,不是必需的
 * 输出参数： 无
 * 返 回 值： NULL   - 失败,没有可用的VideoMem
 *            非NULL - 成功,返回PT_VideoMem结构体
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
PT_VideoMem GetVideoMem(int iID, int bCur)
{
	PT_VideoMem ptTmp = g_ptVideoMemHead;
	
	/* 1. 优先: 取出空闲的、ID相同的videomem */
	while (ptTmp)
	{
		if ((ptTmp->eVideoMemState == VMS_FREE) && (ptTmp->iID == iID))
		{
			ptTmp->eVideoMemState = bCur ? VMS_USED_FOR_CUR : VMS_USED_FOR_PREPARE;
			return ptTmp;
		}
		ptTmp = ptTmp->ptNext;
	}

	/* 2. 如果前面不成功, 取出一个空闲的并且里面没有数据(ptVideoMem->ePicState = PS_BLANK)的VideoMem */
	ptTmp = g_ptVideoMemHead;
	while (ptTmp)
	{
		if ((ptTmp->eVideoMemState == VMS_FREE) && (ptTmp->ePicState == PS_BLANK))
		{
			ptTmp->iID = iID;
			ptTmp->eVideoMemState = bCur ? VMS_USED_FOR_CUR : VMS_USED_FOR_PREPARE;
			return ptTmp;
		}
		ptTmp = ptTmp->ptNext;
	}	
	
	/* 3. 如果前面不成功: 取出任意一个空闲的VideoMem */
	ptTmp = g_ptVideoMemHead;
	while (ptTmp)
	{
		if (ptTmp->eVideoMemState == VMS_FREE)
		{
			ptTmp->iID = iID;
			ptTmp->ePicState = PS_BLANK;
			ptTmp->eVideoMemState = bCur ? VMS_USED_FOR_CUR : VMS_USED_FOR_PREPARE;
			return ptTmp;
		}
		ptTmp = ptTmp->ptNext;
	}

    /* 4. 如果没有空闲的VideoMem并且bCur为1, 则取出任意一个VideoMem(不管它是否空闲) */
    if (bCur)
    {
    	ptTmp = g_ptVideoMemHead;
    	ptTmp->iID = iID;
    	ptTmp->ePicState = PS_BLANK;
    	ptTmp->eVideoMemState = bCur ? VMS_USED_FOR_CUR : VMS_USED_FOR_PREPARE;
    	return ptTmp;
    }
    
	return NULL;
}

/**********************************************************************
 * 函数名称： PutVideoMem
 * 功能描述： 使用GetVideoMem获得的VideoMem, 用完时用PutVideoMem释放掉
 * 输入参数： ptVideoMem - 使用完毕的VideoMem
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
void PutVideoMem(PT_VideoMem ptVideoMem)
{
	ptVideoMem->eVideoMemState = VMS_FREE;  /* 设置VideoMem状态为空闲 */
    if (ptVideoMem->iID == -1)
    {
        ptVideoMem->ePicState = PS_BLANK;  /* 表示里面的数据没有用了 */
    }
}

/**********************************************************************
 * 函数名称： GetDevVideoMem
 * 功能描述： 获得显示设备的显存, 在该显存上操作就可以直接在LCD上显示出来
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 显存对应的VideoMem结构体指针
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
PT_VideoMem GetDevVideoMem(void)
{
	PT_VideoMem ptTmp = g_ptVideoMemHead;
	
	while (ptTmp)
	{
		if (ptTmp->bDevFrameBuffer)
		{
			return ptTmp;
		}
		ptTmp = ptTmp->ptNext;
	}
	return NULL;
}


/**********************************************************************
 * 函数名称： ClearVideoMem
 * 功能描述： 把VideoMem中内存全部清为某种颜色
 * 输入参数： ptVideoMem - VideoMem结构体指针, 内含要操作的内存
 *            dwColor    - 设置为该颜色
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
void ClearVideoMem(PT_VideoMem ptVideoMem, unsigned int dwColor)
{
	unsigned char *pucVM;
	unsigned short *pwVM16bpp;
	unsigned int *pdwVM32bpp;
	unsigned short wColor16bpp; /* 565 */
	int iRed;
	int iGreen;
	int iBlue;
	int i = 0;

	pucVM	   = ptVideoMem->tPixelDatas.aucPixelDatas;
	pwVM16bpp  = (unsigned short *)pucVM;
	pdwVM32bpp = (unsigned int *)pucVM;

	switch (ptVideoMem->tPixelDatas.iBpp)
	{
		case 8:
		{
			memset(pucVM, dwColor, ptVideoMem->tPixelDatas.iTotalBytes);
			break;
		}
		case 16:
		{
			/* 先根据32位的dwColor构造出16位的wColor16bpp */
			iRed   = (dwColor >> (16+3)) & 0x1f;
			iGreen = (dwColor >> (8+2)) & 0x3f;
			iBlue  = (dwColor >> 3) & 0x1f;
			wColor16bpp = (iRed << 11) | (iGreen << 5) | iBlue;
			while (i < ptVideoMem->tPixelDatas.iTotalBytes)
			{
				*pwVM16bpp	= wColor16bpp;
				pwVM16bpp++;
				i += 2;
			}
			break;
		}
		case 32:
		{
			while (i < ptVideoMem->tPixelDatas.iTotalBytes)
			{
				*pdwVM32bpp = dwColor;
				pdwVM32bpp++;
				i += 4;
			}
			break;
		}
		default :
		{
			DBG_PRINTF("can't support %d bpp\n", ptVideoMem->tPixelDatas.iBpp);
			return;
		}
	}

}


/**********************************************************************
 * 函数名称： ClearVideoMemRegion
 * 功能描述： 把VideoMem中内存的指定区域全部清为某种颜色
 * 输入参数： ptVideoMem - VideoMem结构体指针, 内含要操作的内存
 *            ptLayout   - 矩形区域, 指定了左上角,右在角的坐标
 *            dwColor    - 设置为该颜色
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
void ClearVideoMemRegion(PT_VideoMem ptVideoMem, PT_Layout ptLayout, unsigned int dwColor)
{
	unsigned char *pucVM;
	unsigned short *pwVM16bpp;
	unsigned int *pdwVM32bpp;
	unsigned short wColor16bpp; /* 565 */
	int iRed;
	int iGreen;
	int iBlue;
	int iX;
	int iY;
    int iLineBytesClear;
    int i;

	pucVM	   = ptVideoMem->tPixelDatas.aucPixelDatas + ptLayout->iTopLeftY * ptVideoMem->tPixelDatas.iLineBytes + ptLayout->iTopLeftX * ptVideoMem->tPixelDatas.iBpp / 8;
	pwVM16bpp  = (unsigned short *)pucVM;
	pdwVM32bpp = (unsigned int *)pucVM;

    iLineBytesClear = (ptLayout->iBotRightX - ptLayout->iTopLeftX + 1) * ptVideoMem->tPixelDatas.iBpp / 8;

	switch (ptVideoMem->tPixelDatas.iBpp)
	{
		case 8:
		{
            for (iY = ptLayout->iTopLeftY; iY <= ptLayout->iBotRightY; iY++)
            {
    			memset(pucVM, dwColor, iLineBytesClear);
                pucVM += ptVideoMem->tPixelDatas.iLineBytes;
            }
			break;
		}
		case 16:
		{
			/* 先根据32位的dwColor构造出16位的wColor16bpp */
			iRed   = (dwColor >> (16+3)) & 0x1f;
			iGreen = (dwColor >> (8+2)) & 0x3f;
			iBlue  = (dwColor >> 3) & 0x1f;
			wColor16bpp = (iRed << 11) | (iGreen << 5) | iBlue;
            for (iY = ptLayout->iTopLeftY; iY <= ptLayout->iBotRightY; iY++)
            {
                i = 0;
                for (iX = ptLayout->iTopLeftX; iX <= ptLayout->iBotRightX; iX++)
    			{
    				pwVM16bpp[i++]	= wColor16bpp;
    			}
                pwVM16bpp = (unsigned short *)((unsigned int)pwVM16bpp + ptVideoMem->tPixelDatas.iLineBytes);
            }
			break;
		}
		case 32:
		{
            for (iY = ptLayout->iTopLeftY; iY <= ptLayout->iBotRightY; iY++)
            {
                i = 0;
                for (iX = ptLayout->iTopLeftX; iX <= ptLayout->iBotRightX; iX++)
    			{
    				pdwVM32bpp[i++]	= dwColor;
    			}
                pdwVM32bpp = (unsigned int *)((unsigned int)pdwVM32bpp + ptVideoMem->tPixelDatas.iLineBytes);
            }
			break;
		}
		default :
		{
			DBG_PRINTF("can't support %d bpp\n", ptVideoMem->tPixelDatas.iBpp);
			return;
		}
	}

}

/**********************************************************************
 * 函数名称： DisplayInit
 * 功能描述： 注册显示设备
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int DisplayInit(void)
{
	int iError;
	
	iError = FBInit();

	return iError;
}

