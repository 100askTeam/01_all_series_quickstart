
#ifndef _PIC_OPERATION_H
#define _PIC_OPERATION_H

#include <file.h>

/* 图片的象素数据 */
typedef struct PixelDatas {
	int iWidth;   /* 宽度: 一行有多少个象素 */
	int iHeight;  /* 高度: 一列有多少个象素 */
	int iBpp;     /* 一个象素用多少位来表示 */
	int iLineBytes;  /* 一行数据有多少字节 */
	int iTotalBytes; /* 所有字节数 */ 
	unsigned char *aucPixelDatas;  /* 象素数据存储的地方 */
}T_PixelDatas, *PT_PixelDatas;


typedef struct PicFileParser {
	char *name;                     /* 图片文件解析模块的名字 */
	int (*isSupport)(PT_FileMap ptFileMap);  /* 是否支持某文件 */
	int (*GetPixelDatas)(PT_FileMap ptFileMap, PT_PixelDatas ptPixelDatas);  /* 从文件中解析出图像的象素数据 */
	int (*FreePixelDatas)(PT_PixelDatas ptPixelDatas);  /* 释放图像的象素数据所占内存 */
    struct PicFileParser *ptNext;  /* 链表 */
}T_PicFileParser, *PT_PicFileParser;

#endif /* _PIC_OPERATION_H */

