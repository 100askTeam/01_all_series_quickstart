#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <config.h>
#include <encoding_manager.h>
#include <fonts_manager.h>
#include <disp_manager.h>
#include <input_manager.h>
#include <pic_operation.h>
#include <render.h>
#include <string.h>
#include <picfmt_manager.h>
#include <test_items.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>


/* digitpic <freetype_file> */
int main(int argc, char **argv)
{	
	int iError;

	/* 初始化调试模块: 可以通过"标准输出"也可以通过"网络"打印调试信息
	 * 因为下面马上就要用到DBG_PRINTF函数, 所以先初始化调试模块
	 */

	/* 注册调试通道 */
	DebugInit();

	if (ParseConfig())
		return -1;

	/* 初始化调试通道 */
	InitDebugChanel();
#if 1
	if (argc != 2)
	{
		DBG_PRINTF("Usage:\n");
		DBG_PRINTF("%s <freetype_file>\n", argv[0]);
		return 0;
	}
#endif
	/* 注册显示设备 */
	DisplayInit();
	/* 可能可支持多个显示设备: 选择和初始化指定的显示设备 */
	SelectAndInitDefaultDispDev("fb");

	/* 
	 * VideoMem: 为加快显示速度,我们事先在内存中构造好显示的页面的数据,
	             (这个内存称为VideoMem)
	 *           显示时再把VideoMem中的数据复制到设备的显存上
	 * 参数的含义就是分配的多少个VideoMem
	 * 参数可取为0, 这意味着所有的显示数据都是在显示时再现场生成,然后写入显存
	 */
	AllocVideoMem(5);

    /* 注册输入设备 */
	InputInit();
    /* 调用所有输入设备的初始化函数 */
	AllInputDevicesInit();

    /* 注册编码模块 */
    EncodingInit();

    /* 注册字库模块 */
	iError = FontsInit();
	if (iError)
	{
		DBG_PRINTF("FontsInit error!\n");
	}

#if 1
    /* 设置freetype字库所用的文件和字体尺寸 */
	iError = SetFontsDetail("freetype", argv[1], 24);
	if (iError)
	{
		DBG_PRINTF("SetFontsDetail error!\n");
	}
#endif
    /* 注册图片文件解析模块 */
    PicFmtsInit();

    /* 注册页面 */
	PagesInit();

    /* 运行主页面 */
	Page("main")->Run(NULL);
		
	return 0;
}

