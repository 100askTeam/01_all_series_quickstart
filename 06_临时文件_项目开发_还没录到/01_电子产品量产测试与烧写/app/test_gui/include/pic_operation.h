
#ifndef _PIC_OPERATION_H
#define _PIC_OPERATION_H

#include <file.h>

/* ͼƬ���������� */
typedef struct PixelDatas {
	int iWidth;   /* ���: һ���ж��ٸ����� */
	int iHeight;  /* �߶�: һ���ж��ٸ����� */
	int iBpp;     /* һ�������ö���λ����ʾ */
	int iLineBytes;  /* һ�������ж����ֽ� */
	int iTotalBytes; /* �����ֽ��� */ 
	unsigned char *aucPixelDatas;  /* �������ݴ洢�ĵط� */
}T_PixelDatas, *PT_PixelDatas;


typedef struct PicFileParser {
	char *name;                     /* ͼƬ�ļ�����ģ������� */
	int (*isSupport)(PT_FileMap ptFileMap);  /* �Ƿ�֧��ĳ�ļ� */
	int (*GetPixelDatas)(PT_FileMap ptFileMap, PT_PixelDatas ptPixelDatas);  /* ���ļ��н�����ͼ����������� */
	int (*FreePixelDatas)(PT_PixelDatas ptPixelDatas);  /* �ͷ�ͼ�������������ռ�ڴ� */
    struct PicFileParser *ptNext;  /* ���� */
}T_PicFileParser, *PT_PicFileParser;

#endif /* _PIC_OPERATION_H */

