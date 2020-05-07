
#ifndef _CONFIG_H
#define _CONFIG_H

#include <stdio.h>
#include <debug_manager.h>

#define FB_DEVICE_NAME "/dev/fb0"
#define DEFAULT_DIR "/"

#define COLOR_BACKGROUND   0xE7DBB5  /* 泛黄的纸 */
#define COLOR_FOREGROUND   0x514438  /* 褐色字体 */

//#define DBG_PRINTF(...)  
#define DBG_PRINTF DebugPrint

/* 图标所在目录 */
#define ICON_PATH  "/etc/test_gui/icons"

#endif /* _CONFIG_H */
