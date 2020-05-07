
#include <config.h>
#include <file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


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
int MapFile(PT_FileMap ptFileMap)
{
	int iFd;
    FILE *tFp;
	struct stat tStat;
	
	/* 打开文件 */
	tFp = fopen(ptFileMap->strFileName, "r+");
	if (tFp == NULL)
	{
		DBG_PRINTF("can't open %s\n", ptFileMap->strFileName);
		return -1;
	}
	ptFileMap->tFp = tFp;
    iFd = fileno(tFp);

	fstat(iFd, &tStat);
	ptFileMap->iFileSize = tStat.st_size;
	ptFileMap->pucFileMapMem = (unsigned char *)mmap(NULL , tStat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, iFd, 0);
	if (ptFileMap->pucFileMapMem == (unsigned char *)-1)
	{
		DBG_PRINTF("mmap error!\n");
		return -1;
	}
	return 0;
}

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
void UnMapFile(PT_FileMap ptFileMap)
{
	munmap(ptFileMap->pucFileMapMem, ptFileMap->iFileSize);
	fclose(ptFileMap->tFp);
}

/**********************************************************************
 * 函数名称： isDir
 * 功能描述： 判断一个文件是否为目录
 * 输入参数： strFilePath - 文件的路径
 *            strFileName - 文件的名字
 * 输出参数： 无
 * 返 回 值： 0 - 不是目录
 *            1 - 是目录
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int isDir(char *strFilePath, char *strFileName)
{
    char strTmp[256];
    struct stat tStat;

    snprintf(strTmp, 256, "%s/%s", strFilePath, strFileName);
    strTmp[255] = '\0';

    if ((stat(strTmp, &tStat) == 0) && S_ISDIR(tStat.st_mode))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**********************************************************************
 * 函数名称： isRegDir
 * 功能描述： 判断一个目录是否常规的目录,在本程序中把sbin等目录当作特殊目录来对待
 * 输入参数： strDirPath    - 目录的路径
 *            strSubDirName - 目录的名字
 * 输出参数： 无
 * 返 回 值： 0 - 不是常规目录
 *            1 - 是常规目录
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int isRegDir(char *strDirPath, char *strSubDirName)
{
    static const char *astrSpecailDirs[] = {"sbin", "bin", "usr", "lib", "proc", "tmp", "dev", "sys", NULL};
    int i = 0;
    
    /* 如果目录名含有"astrSpecailDirs"中的任意一个, 则返回0 */
    if (0 == strcmp(strDirPath, "/"))
    {
        while (astrSpecailDirs[i])
        {
            if (0 == strcmp(strSubDirName, astrSpecailDirs[i]))
                return 0;
            i++;
        }
    }
    return 1;    
}

/**********************************************************************
 * 函数名称： isRegFile
 * 功能描述： 判断一个文件是否常规的文件,设备节点/链接文件/FIFO文件等是特殊文件
 * 输入参数： strFilePath - 文件的路径
 *            strFileName - 文件的名字
 * 输出参数： 无
 * 返 回 值： 0 - 不是常规文件
 *            1 - 是常规文件
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int isRegFile(char *strFilePath, char *strFileName)
{
    char strTmp[256];
    struct stat tStat;

    snprintf(strTmp, 256, "%s/%s", strFilePath, strFileName);
    strTmp[255] = '\0';

    if ((stat(strTmp, &tStat) == 0) && S_ISREG(tStat.st_mode))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}



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
int GetDirContents(char *strDirName, PT_DirContent **pptDirContents, int *piNumber)	
{
    PT_DirContent *aptDirContents;
	struct dirent **aptNameList;
	int iNumber;
	int i;
	int j;

	/* 扫描目录,结果按名字排序,存在aptNameList[0],aptNameList[1],... */
	iNumber = scandir(strDirName, &aptNameList, 0, alphasort);
	if (iNumber < 0)
	{
		DBG_PRINTF("scandir error : %s!\n", strDirName);
		return -1;
	}

	/* 忽略".", ".."这两个目录 */
	aptDirContents = malloc(sizeof(PT_DirContent) * (iNumber - 2));
	if (NULL == aptDirContents)
	{
		DBG_PRINTF("malloc error!\n");
		return -1;
	}
    *pptDirContents = aptDirContents;

	for (i = 0; i < iNumber - 2; i++)
	{
		aptDirContents[i] = malloc(sizeof(T_DirContent));
		if (NULL == aptDirContents)
		{
			DBG_PRINTF("malloc error!\n");
			return -1;
		}
	}

	/* 先把目录挑出来存入aptDirContents */
	for (i = 0, j = 0; i < iNumber; i++)
	{
		/* 忽略".",".."这两个目录 */
		if ((0 == strcmp(aptNameList[i]->d_name, ".")) || (0 == strcmp(aptNameList[i]->d_name, "..")))
			continue;
        /* 并不是所有的文件系统都支持d_type, 所以不能直接判断d_type */
		/* if (aptNameList[i]->d_type == DT_DIR) */
        if (isDir(strDirName, aptNameList[i]->d_name))
		{
			strncpy(aptDirContents[j]->strName, aptNameList[i]->d_name, 256);
			aptDirContents[j]->strName[255] = '\0';
			aptDirContents[j]->eFileType    = FILETYPE_DIR;
            free(aptNameList[i]);
            aptNameList[i] = NULL;
			j++;
		}
	}

	/* 再把常规文件挑出来存入aptDirContents */
	for (i = 0; i < iNumber; i++)
	{
        if (aptNameList[i] == NULL)
            continue;
        
		/* 忽略".",".."这两个目录 */
		if ((0 == strcmp(aptNameList[i]->d_name, ".")) || (0 == strcmp(aptNameList[i]->d_name, "..")))
			continue;
        /* 并不是所有的文件系统都支持d_type, 所以不能直接判断d_type */
		/* if (aptNameList[i]->d_type == DT_REG) */
        if (isRegFile(strDirName, aptNameList[i]->d_name))
		{
			strncpy(aptDirContents[j]->strName, aptNameList[i]->d_name, 256);
			aptDirContents[j]->strName[255] = '\0';
			aptDirContents[j]->eFileType    = FILETYPE_FILE;
            free(aptNameList[i]);
            aptNameList[i] = NULL;
			j++;
		}
	}

	/* 释放aptDirContents中未使用的项 */
	for (i = j; i < iNumber - 2; i++)
	{
		free(aptDirContents[i]);
	}

	/* 释放scandir函数分配的内存 */
	for (i = 0; i < iNumber; i++)
	{
        if (aptNameList[i])
        {
    		free(aptNameList[i]);
        }
	}
	free(aptNameList);

	*piNumber = j;
	
	return 0;
}

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
void FreeDirContents(PT_DirContent *aptDirContents, int iNumber)
{
	int i;
	for (i = 0; i < iNumber; i++)
	{
		free(aptDirContents[i]);
	}
	free(aptDirContents);
}

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
int GetFilesIndir(char *strDirName, int *piStartNumberToRecord, int *piCurFileNumber, int *piFileCountHaveGet, int iFileCountTotal, char apstrFileNames[][256])
{
    int iError;
    PT_DirContent *aptDirContents;  /* 数组:存有目录下"顶层子目录","文件"的名字 */
    int iDirContentsNumber;         /* aptDirContents数组有多少项 */
    int i;
    char strSubDirName[256];

    /* 为避免访问的目录互相嵌套, 设置能访问的目录深度为10 */
#define MAX_DIR_DEEPNESS 10    
    static int iDirDeepness = 0;

    if (iDirDeepness > MAX_DIR_DEEPNESS)
    {
        return -1;
    }

    iDirDeepness++;    
    
    iError = GetDirContents(strDirName, &aptDirContents, &iDirContentsNumber);    
    if (iError)
    {
        DBG_PRINTF("GetDirContents error!\n");
        iDirDeepness--;
        return -1;
    }

    /* 先记录文件 */
    for (i = 0; i < iDirContentsNumber; i++)
    {
        if (aptDirContents[i]->eFileType == FILETYPE_FILE)
        {
            if (*piCurFileNumber >= *piStartNumberToRecord)
            {
                snprintf(apstrFileNames[*piFileCountHaveGet], 256, "%s/%s", strDirName, aptDirContents[i]->strName);
                (*piFileCountHaveGet)++;
                (*piCurFileNumber)++;
                (*piStartNumberToRecord)++;
                if (*piFileCountHaveGet >= iFileCountTotal)
                {
                    FreeDirContents(aptDirContents, iDirContentsNumber);
                    iDirDeepness--;
                    return 0;
                }
            }
            else
            {
                (*piCurFileNumber)++;
            }
        }
    }

    /* 递归处理目录 */
    for (i = 0; i < iDirContentsNumber; i++)
    {
        if ((aptDirContents[i]->eFileType == FILETYPE_DIR) && isRegDir(strDirName, aptDirContents[i]->strName))
        {
            snprintf(strSubDirName, 256, "%s/%s", strDirName, aptDirContents[i]->strName);
            GetFilesIndir(strSubDirName, piStartNumberToRecord, piCurFileNumber, piFileCountHaveGet, iFileCountTotal, apstrFileNames);
            if (*piFileCountHaveGet >= iFileCountTotal)
            {
                FreeDirContents(aptDirContents, iDirContentsNumber);
                iDirDeepness--;
                return 0;
            }
        }
    }

    FreeDirContents(aptDirContents, iDirContentsNumber);
    iDirDeepness--;
    return 0;
}


