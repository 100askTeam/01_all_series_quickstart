#include <config.h>
#include <input_manager.h>
#include <disp_manager.h>
#include <stdlib.h>

#include <tslib.h>
//#include <draw.h>

/* 参考tslib里的ts_print.c */

static struct tsdev *g_tTSDev;
static int giXres;
static int giYres;

/**********************************************************************
 * 函数名称： TouchScreenDevInit
 * 功能描述： 触摸屏输入模块的设备初始化函数
 *            注意: 由于要用到LCD的分辨率, 此函数要在SelectAndInitDisplay之后才能调用 
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int TouchScreenDevInit(void)
{
	const char *tsdevice = NULL;
	int iBpp;
#if 0
	char *pcTSName = NULL;
	if ((pcTSName = getenv("TSLIB_TSDEVICE")) != NULL ) 
	{
		g_tTSDev = ts_open(pcTSName, 0);  /* 以阻塞方式打开 */
	}
	else
	{
		g_tTSDev = ts_open("/dev/event0", 1);
	}
#else
	g_tTSDev = ts_setup(tsdevice, 0);
#endif
	if (!g_tTSDev) {
		DBG_PRINTF(APP_ERR"ts_open error!\n");
		return -1;
	}

#if 0
	if (ts_config(g_tTSDev)) {
		DBG_PRINTF("ts_config error!\n");
		return -1;
	}
#endif
	if (GetDispResolution(&giXres, &giYres, &iBpp))
	{
		return -1;
	}

	return 0;
}

/**********************************************************************
 * 函数名称： StdinDevExit
 * 功能描述： 触摸屏输入模块的设备退出函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int TouchScreenDevExit(void)
{
	return 0;
}


/**********************************************************************
 * 函数名称： TouchScreenGetInputEvent
 * 功能描述： 触摸屏输入模块的读取数据函数,它在触摸屏输入模块的子线程中被调用
 * 输入参数： 无
 * 输出参数： ptInputEvent - 内含得到的输入数据
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int TouchScreenGetInputEvent(PT_InputEvent ptInputEvent)
{
	struct ts_sample tSamp;
	int iRet;	

	while (1)
	{
		//DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
		iRet = ts_read(g_tTSDev, &tSamp, 1); /* 如果无数据则休眠 */
		if (iRet == 1)
		{
			//DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
			ptInputEvent->tTime     = tSamp.tv;
			ptInputEvent->iType     = INPUT_TYPE_TOUCHSCREEN;
			ptInputEvent->iX        = tSamp.x;
			ptInputEvent->iY        = tSamp.y;
			ptInputEvent->iPressure = tSamp.pressure;
			return 0;
		}
		else
		{
			//DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
			return -1;
		}
	}

	return 0;
}


static T_InputOpr g_tTouchScreenOpr = {
	.name          = "touchscreen",
	.DeviceInit    = TouchScreenDevInit,
	.DeviceExit    = TouchScreenDevExit,
	.GetInputEvent = TouchScreenGetInputEvent,
};

/**********************************************************************
 * 函数名称： TouchScreenInit
 * 功能描述： 注册"触摸屏输入模块"
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int TouchScreenInit(void)
{
	return RegisterInputOpr(&g_tTouchScreenOpr);
}

