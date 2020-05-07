
#include <input_manager.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>


/**********************************************************************
 * 函数名称： StdinDevInit
 * 功能描述： 标准输入模块的设备初始化函数,用于设置标准输入的属性,
 *            比如默认的标准输入是接收到回车换行符时才返回数据,
 *            在本程序里把它改为"接收到任意一个字符即返回数据"
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int StdinDevInit(void)
{
    struct termios tTTYState;
 
    //get the terminal state
    tcgetattr(STDIN_FILENO, &tTTYState);
 
    //turn off canonical mode
    tTTYState.c_lflag &= ~ICANON;
    //minimum of number input read.
    tTTYState.c_cc[VMIN] = 1;   /* 有一个数据时就立刻返回 */

    //set the terminal attributes.
    tcsetattr(STDIN_FILENO, TCSANOW, &tTTYState);

	return 0;
}

/**********************************************************************
 * 函数名称： StdinDevExit
 * 功能描述： 标准输入模块的设备退出函数,恢复标准输入的原来属性
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int StdinDevExit(void)
{

    struct termios tTTYState;
 
    //get the terminal state
    tcgetattr(STDIN_FILENO, &tTTYState);
 
    //turn on canonical mode
    tTTYState.c_lflag |= ICANON;
	
    //set the terminal attributes.
    tcsetattr(STDIN_FILENO, TCSANOW, &tTTYState);	
	return 0;
}

/**********************************************************************
 * 函数名称： StdinGetInputEvent
 * 功能描述： 标准输入模块的读取数据函数,它在标准输入模块的子线程中被调用
 * 输入参数： 无
 * 输出参数： ptInputEvent - 内含得到的输入数据
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int StdinGetInputEvent(PT_InputEvent ptInputEvent)
{
	/* 如果有数据就读取、处理、返回
	 * 如果没有数据, 立刻返回, 不等待
	 */

	/* select, poll 可以参数 UNIX环境高级编程 */

 	char c;
	
		/* 处理数据 */
	ptInputEvent->iType = INPUT_TYPE_STDIN;
	
	c = fgetc(stdin);  /* 会休眠直到有输入 */
	c = c;
	gettimeofday(&ptInputEvent->tTime, NULL);

#if 0	
	if (c == 'u')
	{
		ptInputEvent->iVal = INPUT_VALUE_UP;
	}
	else if (c == 'n')
	{
		ptInputEvent->iVal = INPUT_VALUE_DOWN;
	}
	else if (c == 'q')
	{
		ptInputEvent->iVal = INPUT_VALUE_EXIT;
	}
	else
	{
		ptInputEvent->iVal = INPUT_VALUE_UNKNOWN;
	}		
#endif	
	return 0;
 }

static T_InputOpr g_tStdinOpr = {
	.name          = "stdin",
	.DeviceInit    = StdinDevInit,
	.DeviceExit    = StdinDevExit,
	.GetInputEvent = StdinGetInputEvent,
};


/**********************************************************************
 * 函数名称： StdinInit
 * 功能描述： 注册"标准输入模块"
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int StdinInit(void)
{
	return RegisterInputOpr(&g_tStdinOpr);
}

