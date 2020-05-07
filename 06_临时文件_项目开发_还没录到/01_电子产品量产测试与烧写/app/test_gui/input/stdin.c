
#include <input_manager.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>


/**********************************************************************
 * �������ƣ� StdinDevInit
 * ���������� ��׼����ģ����豸��ʼ������,�������ñ�׼���������,
 *            ����Ĭ�ϵı�׼�����ǽ��յ��س����з�ʱ�ŷ�������,
 *            �ڱ������������Ϊ"���յ�����һ���ַ�����������"
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static int StdinDevInit(void)
{
    struct termios tTTYState;
 
    //get the terminal state
    tcgetattr(STDIN_FILENO, &tTTYState);
 
    //turn off canonical mode
    tTTYState.c_lflag &= ~ICANON;
    //minimum of number input read.
    tTTYState.c_cc[VMIN] = 1;   /* ��һ������ʱ�����̷��� */

    //set the terminal attributes.
    tcsetattr(STDIN_FILENO, TCSANOW, &tTTYState);

	return 0;
}

/**********************************************************************
 * �������ƣ� StdinDevExit
 * ���������� ��׼����ģ����豸�˳�����,�ָ���׼�����ԭ������
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
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
 * �������ƣ� StdinGetInputEvent
 * ���������� ��׼����ģ��Ķ�ȡ���ݺ���,���ڱ�׼����ģ������߳��б�����
 * ��������� ��
 * ��������� ptInputEvent - �ں��õ�����������
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static int StdinGetInputEvent(PT_InputEvent ptInputEvent)
{
	/* ��������ݾͶ�ȡ����������
	 * ���û������, ���̷���, ���ȴ�
	 */

	/* select, poll ���Բ��� UNIX�����߼���� */

 	char c;
	
		/* �������� */
	ptInputEvent->iType = INPUT_TYPE_STDIN;
	
	c = fgetc(stdin);  /* ������ֱ�������� */
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
 * �������ƣ� StdinInit
 * ���������� ע��"��׼����ģ��"
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
int StdinInit(void)
{
	return RegisterInputOpr(&g_tStdinOpr);
}

