#include <config.h>
#include <pic_operation.h>
#include <picfmt_manager.h>
#include <file.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <jpeglib.h>

typedef struct MyErrorMgr
{
	struct jpeg_error_mgr pub;
	jmp_buf setjmp_buffer;
}T_MyErrorMgr, *PT_MyErrorMgr;

static int isJPGFormat(PT_FileMap ptFileMap);
static int GetPixelDatasFrmJPG(PT_FileMap ptFileMap, PT_PixelDatas ptPixelDatas);
static int FreePixelDatasForJPG(PT_PixelDatas ptPixelDatas);

static T_PicFileParser g_tJPGParser = {
	.name           = "jpg",
	.isSupport      = isJPGFormat,
	.GetPixelDatas  = GetPixelDatasFrmJPG,
	.FreePixelDatas = FreePixelDatasForJPG,	
};

/**********************************************************************
 * �������ƣ� MyErrorExit
 * ���������� �Զ����libjpeg���������
 *            Ĭ�ϵĴ����������ó����˳�,���ǵ�Ȼ����ʹ����
 *            �ο�libjpeg���bmp.c��д�������������
 * ��������� ptCInfo - libjpeg����������ͨ�ýṹ��
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static void MyErrorExit(j_common_ptr ptCInfo)
{
    static char errStr[JMSG_LENGTH_MAX];
    
	PT_MyErrorMgr ptMyErr = (PT_MyErrorMgr)ptCInfo->err;

    /* Create the message */
    (*ptCInfo->err->format_message) (ptCInfo, errStr);
    DBG_PRINTF("%s\n", errStr);

	longjmp(ptMyErr->setjmp_buffer, 1);
}

/**********************************************************************
 * �������ƣ� isJPGFormat
 * ���������� JPGģ���Ƿ�֧�ָ��ļ�,�����ļ��Ƿ�ΪJPG�ļ�
 * ��������� ptFileMap - �ں��ļ���Ϣ
 * ��������� ��
 * �� �� ֵ�� 0 - ��֧��, 1 - ֧��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static int isJPGFormat(PT_FileMap ptFileMap)
{
	struct jpeg_decompress_struct tDInfo;

    /* Ĭ�ϵĴ����������ó����˳�
     * ���ǲο�libjpeg���bmp.c��д�Լ��Ĵ�������
     */
	//struct jpeg_error_mgr tJErr;   
	T_MyErrorMgr tJerr;
    int iRet;

    fseek(ptFileMap->tFp, 0, SEEK_SET);

	// ����ͳ�ʼ��һ��decompression�ṹ��
	// tDInfo.err = jpeg_std_error(&tJErr);
	tDInfo.err               = jpeg_std_error(&tJerr.pub);
	tJerr.pub.error_exit     = MyErrorExit;

	if(setjmp(tJerr.setjmp_buffer))
	{
		/* ������������е�����, ��ʾJPEG������� */
        jpeg_destroy_decompress(&tDInfo);
		return 0;;
	}
	
	jpeg_create_decompress(&tDInfo);

	// ��jpeg_read_header���jpg��Ϣ
	jpeg_stdio_src(&tDInfo, ptFileMap->tFp);

    iRet = jpeg_read_header(&tDInfo, TRUE);
	jpeg_abort_decompress(&tDInfo);

    return (iRet == JPEG_HEADER_OK);
}

/**********************************************************************
 * �������ƣ� CovertOneLine
 * ���������� ���Ѿ���JPG�ļ�ȡ����һ����������,ת��Ϊ������ʾ�豸��ʹ�õĸ�ʽ
 * ��������� iWidth      - ���,�����ٸ�����
 *            iSrcBpp     - �Ѿ���JPG�ļ�ȡ����һ��������������,һ�������ö���λ����ʾ
 *            iDstBpp     - ��ʾ�豸��һ�������ö���λ����ʾ
 *            pudSrcDatas - �Ѿ���JPG�ļ�ȡ����һ�����������洢��λ��
 *            pudDstDatas - ת���������ݴ洢��λ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static int CovertOneLine(int iWidth, int iSrcBpp, int iDstBpp, unsigned char *pudSrcDatas, unsigned char *pudDstDatas)
{
	unsigned int dwRed;
	unsigned int dwGreen;
	unsigned int dwBlue;
	unsigned int dwColor;

	unsigned short *pwDstDatas16bpp = (unsigned short *)pudDstDatas;
	unsigned int   *pwDstDatas32bpp = (unsigned int *)pudDstDatas;

	int i;
	int pos = 0;

	if (iSrcBpp != 24)
	{
		return -1;
	}

	if (iDstBpp == 24)
	{
		memcpy(pudDstDatas, pudSrcDatas, iWidth*3);
	}
	else
	{
		for (i = 0; i < iWidth; i++)
		{
			dwRed   = pudSrcDatas[pos++];
			dwGreen = pudSrcDatas[pos++];
			dwBlue  = pudSrcDatas[pos++];
			if (iDstBpp == 32)
			{
				dwColor = (dwRed << 16) | (dwGreen << 8) | dwBlue;
				*pwDstDatas32bpp = dwColor;
				pwDstDatas32bpp++;
			}
			else if (iDstBpp == 16)
			{
				/* 565 */
				dwRed   = dwRed >> 3;
				dwGreen = dwGreen >> 2;
				dwBlue  = dwBlue >> 3;
				dwColor = (dwRed << 11) | (dwGreen << 5) | (dwBlue);
				*pwDstDatas16bpp = dwColor;
				pwDstDatas16bpp++;
			}
		}
	}
	return 0;
}


/**********************************************************************
 * �������ƣ� GetPixelDatasFrmJPG
 * ���������� ��JPG�ļ��е�ͼ������,ȡ����ת��Ϊ������ʾ�豸��ʹ�õĸ�ʽ
 * ��������� ptFileMap    - �ں��ļ���Ϣ
 * ��������� ptPixelDatas - �ں���������
 *            ptPixelDatas->iBpp ������Ĳ���, ��ȷ����JPG�ļ��õ�������Ҫת��Ϊ��BPP
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���          �޸�����
 * -----------------------------------------------
 * 2013/02/08        V1.0     Τ��ɽ          ����
 ***********************************************************************/
static int GetPixelDatasFrmJPG(PT_FileMap ptFileMap, PT_PixelDatas ptPixelDatas)
{
	struct jpeg_decompress_struct tDInfo;
	//struct jpeg_error_mgr tJErr;
    int iRet;
    int iRowStride;
    unsigned char *aucLineBuffer = NULL;
    unsigned char *pucDest;
	T_MyErrorMgr tJerr;

    fseek(ptFileMap->tFp, 0, SEEK_SET);

	// ����ͳ�ʼ��һ��decompression�ṹ��
	//tDInfo.err = jpeg_std_error(&tJErr);

	tDInfo.err               = jpeg_std_error(&tJerr.pub);
	tJerr.pub.error_exit     = MyErrorExit;

	if(setjmp(tJerr.setjmp_buffer))
	{
		/* ������������е�����, ��ʾJPEG������� */
        jpeg_destroy_decompress(&tDInfo);
        if (aucLineBuffer)
        {
            free(aucLineBuffer);
        }
        if (ptPixelDatas->aucPixelDatas)
        {
            free(ptPixelDatas->aucPixelDatas);
        }
		return -1;
	}

	jpeg_create_decompress(&tDInfo);

	// ��jpeg_read_header���jpg��Ϣ
	jpeg_stdio_src(&tDInfo, ptFileMap->tFp);

    iRet = jpeg_read_header(&tDInfo, TRUE);

	// ���ý�ѹ����,����Ŵ���С
    tDInfo.scale_num = tDInfo.scale_denom = 1;
    
	// ������ѹ��jpeg_start_decompress	
	jpeg_start_decompress(&tDInfo);
    
	// һ�е����ݳ���
	iRowStride = tDInfo.output_width * tDInfo.output_components;
	aucLineBuffer = malloc(iRowStride);

    if (NULL == aucLineBuffer)
    {
        return -1;
    }

	ptPixelDatas->iWidth  = tDInfo.output_width;
	ptPixelDatas->iHeight = tDInfo.output_height;
	//ptPixelDatas->iBpp    = iBpp;
	ptPixelDatas->iLineBytes    = ptPixelDatas->iWidth * ptPixelDatas->iBpp / 8;
    ptPixelDatas->iTotalBytes   = ptPixelDatas->iHeight * ptPixelDatas->iLineBytes;
	ptPixelDatas->aucPixelDatas = malloc(ptPixelDatas->iTotalBytes);
	if (NULL == ptPixelDatas->aucPixelDatas)
	{
		return -1;
	}

    pucDest = ptPixelDatas->aucPixelDatas;

	// ѭ������jpeg_read_scanlines��һ��һ�еػ�ý�ѹ������
	while (tDInfo.output_scanline < tDInfo.output_height) 
	{
        /* �õ�һ������,�������ɫ��ʽΪ0xRR, 0xGG, 0xBB */
		(void) jpeg_read_scanlines(&tDInfo, &aucLineBuffer, 1);

		// ת��ptPixelDatasȥ
		CovertOneLine(ptPixelDatas->iWidth, 24, ptPixelDatas->iBpp, aucLineBuffer, pucDest);
		pucDest += ptPixelDatas->iLineBytes;
	}
	
	free(aucLineBuffer);
	jpeg_finish_decompress(&tDInfo);
	jpeg_destroy_decompress(&tDInfo);

    return 0;
}

/**********************************************************************
 * �������ƣ� FreePixelDatasForJPG
 * ���������� GetPixelDatasFrmJPG�ķ�����,������������ռ�ڴ��ͷŵ�
 * ��������� ptPixelDatas - �ں���������
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���          �޸�����
 * -----------------------------------------------
 * 2013/02/08        V1.0     Τ��ɽ          ����
 ***********************************************************************/
static int FreePixelDatasForJPG(PT_PixelDatas ptPixelDatas)
{
	free(ptPixelDatas->aucPixelDatas);
	return 0;
}

/**********************************************************************
 * �������ƣ� JPGParserInit
 * ���������� ע��"JPG�ļ�����ģ��"
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int JPGParserInit(void)
{
	return RegisterPicFileParser(&g_tJPGParser);
}


