#include <config.h>
#include <input_manager.h>
#include <disp_manager.h>
#include <stdlib.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <pthread.h>

static int g_iSocketInputServer;
static struct sockaddr_in g_tSocketInputServerAddr;

#define INPUT_SERVER_PORT 8765

static char g_strInputBuf[1000];

/**********************************************************************
 * 函数名称： NetInputRecvTreadFunction
 * 功能描述： "网络输出调试通道"的接收线程函数
 *            接收客户端发来的数据
 * 输入参数： pVoid - 未用
 * 输出参数： 无
 * 返 回 值： NULL - 线程退出
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int NetInputGetRecv(void)
{
    socklen_t iAddrLen;
    int iRecvLen;
    struct sockaddr_in tSocketClientAddr;

	/* 读取客户端发来的数据 */
	iAddrLen = sizeof(struct sockaddr);
	DBG_PRINTF("in NetInputGetRecv\n");
	iRecvLen = recvfrom(g_iSocketInputServer, g_strInputBuf, 999, 0, (struct sockaddr *)&tSocketClientAddr, &iAddrLen);
	
	if (iRecvLen > 0)	
	{
		g_strInputBuf[iRecvLen] = '\0';
		DBG_PRINTF("NetInputGetRecv get msg: %s\n", g_strInputBuf);
        return 0;
	}
    else
    {
        return -1;
    }
	
}

/**********************************************************************
 * 函数名称： NetInputDevInit
 * 功能描述： 初始化网络
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int NetInputDevInit(void)
{
	/* socket初始化 */
	int iRet;
	
	g_iSocketInputServer = socket(AF_INET, SOCK_DGRAM, 0);
	if (-1 == g_iSocketInputServer)
	{
		printf("socket error!\n");
		return -1;
	}

	g_tSocketInputServerAddr.sin_family      = AF_INET;
	g_tSocketInputServerAddr.sin_port        = htons(INPUT_SERVER_PORT);  /* host to net, short */
 	g_tSocketInputServerAddr.sin_addr.s_addr = INADDR_ANY;
	memset(g_tSocketInputServerAddr.sin_zero, 0, 8);
	
	iRet = bind(g_iSocketInputServer, (const struct sockaddr *)&g_tSocketInputServerAddr, sizeof(struct sockaddr));
	if (-1 == iRet)
	{
		printf("bind error!\n");
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
static int NetInputDevExit(void)
{
	return 0;
}


/**********************************************************************
 * 函数名称： NetInputGetInputEvent
 * 功能描述： 获取网络数据并解析为一个指令
 * 输入参数： 无
 * 输出参数： ptInputEvent - 内含得到的输入数据
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int NetInputGetInputEvent(PT_InputEvent ptInputEvent)
{
	int iRet;
	
	while (1)
	{
		iRet = NetInputGetRecv();
		if (!iRet)
		{
			ptInputEvent->iType = INPUT_TYPE_NET;
			strncpy(ptInputEvent->aStr, g_strInputBuf, 1000);
			return 0;
		}
		else
		{
			return -1;
		}
	}

	return 0;
}


static T_InputOpr g_tNetInputOpr = {
	.name          = "netinput",
	.DeviceInit    = NetInputDevInit,
	.DeviceExit    = NetInputDevExit,
	.GetInputEvent = NetInputGetInputEvent,
};

/**********************************************************************
 * 函数名称： NetInputInit
 * 功能描述： 注册"触摸屏输入模块"
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int NetInputInit(void)
{
	return RegisterInputOpr(&g_tNetInputOpr);
}

