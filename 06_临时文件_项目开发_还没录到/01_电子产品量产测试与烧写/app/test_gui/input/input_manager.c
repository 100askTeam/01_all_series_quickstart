
#include <config.h>
#include <input_manager.h>
#include <string.h>

#define INPUT_EVENT_BUF_SIZE 1024
#define NEXT_BUFFER(w) ((w + 1) % INPUT_EVENT_BUF_SIZE)

static T_InputEvent g_atInputEvent[INPUT_EVENT_BUF_SIZE];
static int g_iRead;
static int g_iWrite;


static PT_InputOpr g_ptInputOprHead;

static pthread_mutex_t g_tMutex  = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  g_tConVar = PTHREAD_COND_INITIALIZER;

static int isEventBufferFull(void)
{
	return (NEXT_BUFFER(g_iWrite) == g_iRead);
}

static int isEventBufferEmpty(void)
{
	return (g_iWrite == g_iRead);
}

static void PutEventBuffer(PT_InputEvent pInputEvent)
{
	if (!isEventBufferFull())
	{
		//g_atInputEvent[g_iWrite] = *pInputEvent;
		memcpy(&g_atInputEvent[g_iWrite], pInputEvent, INPUT_EVENT_STR_OFFSET);
		if (pInputEvent->iType == INPUT_TYPE_NET)
			strcpy(g_atInputEvent[g_iWrite].aStr, pInputEvent->aStr);
		g_iWrite = NEXT_BUFFER(g_iWrite);
	}
}

static PT_InputEvent GetEventBuffer(void)
{
	PT_InputEvent pInputEvent;
	
	if (isEventBufferEmpty())
		return NULL;
	else
	{
		pInputEvent = &g_atInputEvent[g_iRead];
		g_iRead = NEXT_BUFFER(g_iRead);
		return pInputEvent;
	}
}

/**********************************************************************
 * 函数名称： RegisterInputOpr
 * 功能描述： 注册"输入模块"
 * 输入参数： ptInputOpr - 输入模块的结构体指针
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int RegisterInputOpr(PT_InputOpr ptInputOpr)
{
	PT_InputOpr ptTmp;

	if (!g_ptInputOprHead)
	{
		g_ptInputOprHead   = ptInputOpr;
		ptInputOpr->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptInputOprHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = ptInputOpr;
		ptInputOpr->ptNext = NULL;
	}

	return 0;
}


/**********************************************************************
 * 函数名称： ShowInputOpr
 * 功能描述： 显示本程序能支持的"输入模块"
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
void ShowInputOpr(void)
{
	int i = 0;
	PT_InputOpr ptTmp = g_ptInputOprHead;

	while (ptTmp)
	{
		printf("%02d %s\n", i++, ptTmp->name);
		ptTmp = ptTmp->ptNext;
	}
}

/**********************************************************************
 * 函数名称： InputEventThreadFunction
 * 功能描述： "输入模块"的线程函数,每个输入模块都是通过创建子线程来读取输入数据,
 *            读到数据后它会唤醒等得数据的其他线程
 * 输入参数： pVoid - 输入模块的"读输入数据函数"
 * 输出参数： 无
 * 返 回 值： NULL - 正常退出
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static void *InputEventThreadFunction(void *pVoid)
{
	T_InputEvent tInputEvent;
	
	/* 定义函数指针 */
	int (*GetInputEvent)(PT_InputEvent ptInputEvent);
	GetInputEvent = (int (*)(PT_InputEvent))pVoid;

	while (1)
	{
		//DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
		if(0 == GetInputEvent(&tInputEvent))
		{
			/* 唤醒主线程, 把tInputEvent的值放入唤醒缓冲区 */
			/* 访问临界资源前，先获得互斥量 */
			pthread_mutex_lock(&g_tMutex);

			//DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
			PutEventBuffer(&tInputEvent);
			// g_tInputEvent = tInputEvent;

			/*  唤醒主线程 */
			pthread_cond_signal(&g_tConVar);

			/* 释放互斥量 */
			pthread_mutex_unlock(&g_tMutex);
			//DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
		}
		//DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	}

	return NULL;
}

/**********************************************************************
 * 函数名称： AllInputDevicesInit
 * 功能描述： 调用所有"输入模块"的设备相关的初始化函数
 *            并创建用于读取输入数据的子线程
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int AllInputDevicesInit(void)
{
	PT_InputOpr ptTmp = g_ptInputOprHead;
	int iError = -1;

	while (ptTmp)
	{
		if (0 == ptTmp->DeviceInit())
		{
			/* 创建子线程 */
			pthread_create(&ptTmp->tTreadID, NULL, InputEventThreadFunction, ptTmp->GetInputEvent);			
			iError = 0;
		}
		ptTmp = ptTmp->ptNext;
	}
	return iError;
}

/**********************************************************************
 * 函数名称： GetInputEvent
 * 功能描述： 获得输入数据,它会使得当前线程休眠,
 *            当各输入模块的子线程读到数据后会把它唤醒
 * 输入参数： 无
 * 输出参数： ptInputEvent - 内含得到的输入数据
 * 返 回 值： 0 - 成功
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int GetInputEvent(PT_InputEvent ptInputEvent)
{
	PT_InputEvent p = NULL;

	while (!p)
	{
		/* 休眠 */
		pthread_mutex_lock(&g_tMutex);
		p = GetEventBuffer();
		DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

		if (!p) 
		{
			DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
			pthread_cond_wait(&g_tConVar, &g_tMutex);	
			/* 被唤醒后,返回数据 */ 
			DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
			p = GetEventBuffer();
			if (p)
				*ptInputEvent = *p;
			pthread_mutex_unlock(&g_tMutex);
		}
		else
		{
			DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
			*ptInputEvent = *p;
			pthread_mutex_unlock(&g_tMutex);
			break;
		}

	}
	
	return 0;	
}

/**********************************************************************
 * 函数名称： InputInit
 * 功能描述： 调用各个输入模块的初始化函数,就是注册各个输入模块
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int InputInit(void)
{
	int iError = 0;
	// iError = StdinInit();
	iError |= TouchScreenInit();
	iError |= NetInputInit();
	return iError;
}

