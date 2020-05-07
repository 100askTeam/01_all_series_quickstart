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

	/* ��ʼ������ģ��: ����ͨ��"��׼���"Ҳ����ͨ��"����"��ӡ������Ϣ
	 * ��Ϊ�������Ͼ�Ҫ�õ�DBG_PRINTF����, �����ȳ�ʼ������ģ��
	 */

	/* ע�����ͨ�� */
	DebugInit();

	if (ParseConfig())
		return -1;

	/* ��ʼ������ͨ�� */
	InitDebugChanel();
#if 1
	if (argc != 2)
	{
		DBG_PRINTF("Usage:\n");
		DBG_PRINTF("%s <freetype_file>\n", argv[0]);
		return 0;
	}
#endif
	/* ע����ʾ�豸 */
	DisplayInit();
	/* ���ܿ�֧�ֶ����ʾ�豸: ѡ��ͳ�ʼ��ָ������ʾ�豸 */
	SelectAndInitDefaultDispDev("fb");

	/* 
	 * VideoMem: Ϊ�ӿ���ʾ�ٶ�,�����������ڴ��й������ʾ��ҳ�������,
	             (����ڴ��ΪVideoMem)
	 *           ��ʾʱ�ٰ�VideoMem�е����ݸ��Ƶ��豸���Դ���
	 * �����ĺ�����Ƿ���Ķ��ٸ�VideoMem
	 * ������ȡΪ0, ����ζ�����е���ʾ���ݶ�������ʾʱ���ֳ�����,Ȼ��д���Դ�
	 */
	AllocVideoMem(5);

    /* ע�������豸 */
	InputInit();
    /* �������������豸�ĳ�ʼ������ */
	AllInputDevicesInit();

    /* ע�����ģ�� */
    EncodingInit();

    /* ע���ֿ�ģ�� */
	iError = FontsInit();
	if (iError)
	{
		DBG_PRINTF("FontsInit error!\n");
	}

#if 1
    /* ����freetype�ֿ����õ��ļ�������ߴ� */
	iError = SetFontsDetail("freetype", argv[1], 24);
	if (iError)
	{
		DBG_PRINTF("SetFontsDetail error!\n");
	}
#endif
    /* ע��ͼƬ�ļ�����ģ�� */
    PicFmtsInit();

    /* ע��ҳ�� */
	PagesInit();

    /* ������ҳ�� */
	Page("main")->Run(NULL);
		
	return 0;
}

