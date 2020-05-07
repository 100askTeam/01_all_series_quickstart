
#ifndef _FILE_H
#define _FILE_H

#include <stdio.h>

typedef struct FileMap {
	char strFileName[256];   /* 文件名 */
	// int iFd; 
	FILE * tFp;              /* 文件句柄 */
	int iFileSize;           /* 文件大小 */
	unsigned char *pucFileMapMem;  /* 使用mmap函数映射文件得到的内存 */
}T_FileMap, *PT_FileMap;

/* 文件类别 */
typedef enum {
	FILETYPE_DIR = 0,  /* 目录 */
	FILETYPE_FILE,     /* 文件 */
}E_FileType;

/* 目录里的内容 */
typedef struct DirContent {
	char strName[256];     /* 名字 */
	E_FileType eFileType;  /* 类别 */	
}T_DirContent, *PT_DirContent;

/**********************************************************************
 * 函数名称： MapFile
 * 功能描述： 使用mmap函数映射一个文件到内存,以后就可以直接通过内存来访问文件
 * 输入参数： ptFileMap - 内含文件名strFileName
 * 输出参数： ptFileMap - tFp           : 所打开的文件句柄
 *                        iFileSize     : 文件大小
 *                        pucFileMapMem : 映射内存的首地址
 * 返 回 值： 0      - 成功
 *            其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int MapFile(PT_FileMap ptFileMap);

/**********************************************************************
 * 函数名称： UnMapFile
 * 功能描述： MapFile函数的清理函数
 * 输入参数： ptFileMap - 内含所打开/映射的文件信息
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
void UnMapFile(PT_FileMap ptFileMap);

/**********************************************************************
 * 函数名称： GetDirContents
 * 功能描述： 把某目录下所含的顶层子目录、顶层文件都记录下来,并且按名字排序
 * 输入参数： strDirName - 目录名(含绝对路径)
 * 输出参数： pptDirContents - (*pptDirContents)指向一个PT_DirContent数组,
 *                             (*pptDirContents)[0,1,...]指向T_DirContent结构体,
 *                             T_DirContent中含有"目录/文件"的名字等信息
 *            piNumber       - strDirName下含有多少个"顶层子目录/顶层文件",
 *                             即数组(*pptDirContents)[0,1,...]有多少项
 * 返 回 值： 0 - 成功
 *            1 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int GetDirContents(char *strDirName, PT_DirContent **pptDirContents, int *piNumber);

/**********************************************************************
 * 函数名称： FreeDirContents
 * 功能描述： GetDirContents的清理函数,用来释放内存
 * 输入参数： aptDirContents - 指向PT_DirContent数组
 *            iNumber        - 有多少数组项
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
void FreeDirContents(PT_DirContent *aptDirContents, int iNumber);

/**********************************************************************
 * 函数名称： GetFilesIndir
 * 功能描述： 以深度优先的方式获得目录下的文件 
 *            即: 先获得顶层目录下的文件, 再进入一级子目录A
 *                再获得一级子目录A下的文件, 再进入二级子目录AA, ...
 *                处理完一级子目录A后, 再进入一级子目录B
 *
 * "连播模式"下调用该函数获得要显示的文件
 * 有两种方法获得这些文件:
 * 1. 事先把所有文件的名字保存到某个缓冲区中
 * 2. 用到时再去搜索取出若干个文件名
 * 第1种方法比较简单,但是当文件很多时有可能导致内存不足.
 * 我们使用第2种方法:
 * 假设某目录(包括所有子目录)下所有的文件都给它编一个号
 *
 * 输入参数：strDirName            : 要获得哪个目录下的内容 
 *           piStartNumberToRecord : 从第几个文件开始取出它们的名字
 *           iFileCountTotal       : 总共要取出多少个文件的名字
 * 输出参数：piFileCountHaveGet    : 已经得到了多少个文件的名字
 *           apstrFileNames[][256] : 用来存储搜索到的文件名
 * 输出/输出参数：
 *           piCurFileNumber       : 当前搜索到的文件编号
 * 返 回 值：0 - 成功
 *           1 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int GetFilesIndir(char *strDirName, int *piStartNumberToRecord, int *piCurFileNumber, int *piFileCountHaveGet, int iFileCountTotal, char apstrFileNames[][256]);

#endif /* _FILE_H */

