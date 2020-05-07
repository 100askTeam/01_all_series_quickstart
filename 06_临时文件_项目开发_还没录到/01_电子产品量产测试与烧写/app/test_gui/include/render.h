
#ifndef _RENDER_H
#define _RENDER_H

#include <pic_operation.h>
#include <disp_manager.h>
#include <page_manager.h>

/**********************************************************************
 * 函数名称： PicZoom
 * 功能描述： 近邻取样插值方法缩放图片
 *            注意该函数会分配内存来存放缩放后的图片,用完后要用free函数释放掉
 *            "近邻取样插值"的原理请参考网友"lantianyu520"所著的"图像缩放算法"
 * 输入参数： ptOriginPic - 内含原始图片的象素数据
 *            ptBigPic    - 内含缩放后的图片的象素数据
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int PicZoom(PT_PixelDatas ptOriginPic, PT_PixelDatas ptZoomPic);

/**********************************************************************
 * 函数名称： PicMerge
 * 功能描述： 把小图片合并入大图片里
 * 输入参数： iX,iY      - 小图片合并入大图片的某个区域, iX/iY确定这个区域的左上角座标
 *            ptSmallPic - 内含小图片的象素数据
 *            ptBigPic   - 内含大图片的象素数据
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int PicMerge(int iX, int iY, PT_PixelDatas ptSmallPic, PT_PixelDatas ptBigPic);

/**********************************************************************
 * 函数名称： PicMergeRegion
 * 功能描述： 把新图片的某部分, 合并入老图片的指定区域
 * 输入参数： iStartXofNewPic, iStartYofNewPic : 从新图片的(iStartXofNewPic, iStartYofNewPic)座标处开始读出数据用于合并
 *            iStartXofOldPic, iStartYofOldPic : 合并到老图片的(iStartXofOldPic, iStartYofOldPic)座标去
 *            iWidth, iHeight                  : 合并区域的大小
 *            ptNewPic                         : 新图片
 *            ptOldPic                         : 老图片
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人          修改内容
 * -----------------------------------------------
 * 2013/02/08        V1.0     韦东山          创建
 ***********************************************************************/
int PicMergeRegion(int iStartXofNewPic, int iStartYofNewPic, int iStartXofOldPic, int iStartYofOldPic, int iWidth, int iHeight, PT_PixelDatas ptNewPic, PT_PixelDatas ptOldPic);

/**********************************************************************
 * 函数名称： FlushVideoMemToDev
 * 功能描述： 把缓冲区中的数据刷到显示设备上去,即在显示设备上显示缓冲区中的图像
 * 输入参数： ptVideoMem - 缓冲区,内含象素数据
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
void FlushVideoMemToDev(PT_VideoMem ptVideoMem);

/**********************************************************************
 * 函数名称： GetPixelDatasForIcon
 * 功能描述： 取出BMP格式的图标文件中的象素数据
 * 输入参数： strFileName  - BMP格式的图标文件名,它位于/etc/digitpic/icons目录下
 * 输出参数： ptPixelDatas - 内含象素数据,它所占的空间是通过malloc分配的,
 *                           不用时需要用FreePixelDatasForIcon来释放
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int GetPixelDatasForIcon(char *strFileName, PT_PixelDatas ptPixelDatas);

/**********************************************************************
 * 函数名称： FreePixelDatasForIcon
 * 功能描述： 释放图像数据所占缓冲区
 * 输入参数： ptPixelDatas - 内含象素数据,它所占的空间是通过malloc分配的
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
void FreePixelDatasForIcon(PT_PixelDatas ptPixelDatas);

/**********************************************************************
 * 函数名称： GetPixelDatasFrmFile
 * 功能描述： 从图片文件中取出象素数据
 * 输入参数： strFileName - 文件名,含绝对路径
 * 输出参数： ptPixelDatas - 内含象素数据
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int GetPixelDatasFrmFile(char *strFileName, PT_PixelDatas ptPixelDatas);

/**********************************************************************
 * 函数名称： FreePixelDatasFrmFile
 * 功能描述： GetPixelDatasFrmFile从图片文件中取出象素数据时是动态分配内存的,
 *            FreePixelDatasFrmFile把分配的内存释放掉
 * 输入参数： ptPixelDatas - 内含象素数据
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
void FreePixelDatasFrmFile(PT_PixelDatas ptPixelDatas);

/**********************************************************************
 * 函数名称： ReleaseButton
 * 功能描述： 松开图标,只是改变显示设备上的图标按钮颜色
 * 输入参数： ptLayout   - 图标所在矩形区域
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人          修改内容
 * -----------------------------------------------
 * 2013/02/08        V1.0     韦东山          创建
 ***********************************************************************/
void ReleaseButton(PT_Layout ptLayout);

/**********************************************************************
 * 函数名称： PressButton
 * 功能描述： 按下图标,只是改变显示设备上的图标按钮颜色
 * 输入参数： ptLayout   - 图标所在矩形区域
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人          修改内容
 * -----------------------------------------------
 * 2013/02/08        V1.0     韦东山          创建
 ***********************************************************************/
void PressButton(PT_Layout ptLayout);

/**********************************************************************
 * 函数名称： MergerStringToCenterOfRectangleInVideoMem
 * 功能描述： 在VideoMem的指定矩形居中显示字符串
 *            参考: 03.freetype\02th_arm\06th_show_lines_center
 * 输入参数： iTopLeftX,iTopLeftY   - 矩形区域的左上角座标
 *            iBotRightX,iBotRightY - 矩形区域的右下角座标
 *            pucTextString         - 要显示的字符串
 *            ptVideoMem            - VideoMem
 * 输出参数： 无
 * 返 回 值： 0 - 成功,  其他值 - 失败
 * 修改日期        版本号     修改人          修改内容
 * -----------------------------------------------
 * 2013/02/08        V1.0     韦东山          创建
 ***********************************************************************/
int MergerStringToCenterOfRectangleInVideoMem(int iTopLeftX, int iTopLeftY, int iBotRightX, int iBotRightY, unsigned char *pucTextString, unsigned int dwBackgroudColor, PT_VideoMem ptVideoMem);

/**********************************************************************
 * 函数名称： ClearRectangleInVideoMem
 * 功能描述： 清除VideoMem中某个矩形区域,设为某颜色
 * 输入参数： iTopLeftX,iTopLeftY   - 矩形区域的左上角座标
 *            iBotRightX,iBotRightY - 矩形区域的右下角座标
 *            ptVideoMem            - 设置VideoMem中的矩形区域
 *            dwColor               - 设置为这个颜色,颜色格式为0x00RRGGBB
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
void ClearRectangleInVideoMem(int iTopLeftX, int iTopLeftY, int iBotRightX, int iBotRightY, PT_VideoMem ptVideoMem, unsigned int dwColor);

/**********************************************************************
 * 函数名称： isPictureFileSupported
 * 功能描述： 判断本程序能否支持该图片文件,目前只能支持BMP/JPG格式的文件
 * 输入参数： strFileName - 文件名,含绝对路径
 * 输出参数： 无
 * 返 回 值： 0 - 不支持, 1 - 支持
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int isPictureFileSupported(char *strFileName);


#endif /* _RENDER_H */

