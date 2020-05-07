#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>

/* set_opt(fd,115200,8,'N',1) */
int set_opt(int fd,int nSpeed, int nBits, char nEvent, int nStop)
{
  struct termios newtio,oldtio;
  if ( tcgetattr( fd,&oldtio) != 0) { 
    perror("SetupSerial 1");
    return -1;
  }
  bzero( &newtio, sizeof( newtio ) );
  newtio.c_cflag |= CLOCAL | CREAD; 
  newtio.c_cflag &= ~CSIZE; 

  newtio.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);  /*Input*/
  newtio.c_oflag  &= ~OPOST;   /*Output*/

  switch( nBits )
  {
  case 7:
    newtio.c_cflag |= CS7;
    break;
  case 8:
    newtio.c_cflag |= CS8;
    break;
  }

  switch( nEvent )
  {
  case 'O':
    newtio.c_cflag |= PARENB;
    newtio.c_cflag |= PARODD;
    newtio.c_iflag |= (INPCK | ISTRIP);
    break;
  case 'E': 
    newtio.c_iflag |= (INPCK | ISTRIP);
    newtio.c_cflag |= PARENB;
    newtio.c_cflag &= ~PARODD;
    break;
  case 'N': 
    newtio.c_cflag &= ~PARENB;
    break;
  }

switch( nSpeed )
  {
  case 2400:
    cfsetispeed(&newtio, B2400);
    cfsetospeed(&newtio, B2400);
    break;
  case 4800:
    cfsetispeed(&newtio, B4800);
    cfsetospeed(&newtio, B4800);
    break;
  case 9600:
    cfsetispeed(&newtio, B9600);
    cfsetospeed(&newtio, B9600);
    break;
  case 115200:
    cfsetispeed(&newtio, B115200);
    cfsetospeed(&newtio, B115200);
    break;
  default:
    cfsetispeed(&newtio, B9600);
    cfsetospeed(&newtio, B9600);
    break;
  }
  if( nStop == 1 )
    newtio.c_cflag &= ~CSTOPB;
  else if ( nStop == 2 )
  newtio.c_cflag |= CSTOPB;
  newtio.c_cc[VTIME] = 0; /* 设置从获取到1个字节后开始计时的超时时间 */
  newtio.c_cc[VMIN] = 0;  /* 设置要求等待的最小字节数 */
  tcflush(fd,TCIFLUSH);
  if((tcsetattr(fd,TCSANOW,&newtio))!=0)
  {
    perror("com set error");
    return -1;
  }
  //printf("set done!\n");
  return 0;
}

int open_port(char *com)
{
	int fd;
	//fd = open(com, O_RDWR|O_NOCTTY|O_NDELAY);
	fd = open(com, O_RDWR|O_NOCTTY);
    if (-1 == fd){
		return(-1);
    }
	
  if(fcntl(fd, F_SETFL, 0)<0)
  {
		printf("fcntl failed!\n");
		return -1;
  }

  fcntl(fd, F_SETFL, 0);
  
  return fd;
}


int main(int argc, char **argv)
{
	int oldstate;
	int oldtype;
	int iRet;
	int fd = -1;
	char line[1024];
	int len;
	
	char *cmd = "uname\n";

	if (argc != 2)
	{
		printf("Usage: \n");
		printf("%s </dev/ttySAC1 or other>\n", argv[0]);
		return -1;
	}

	fd = open_port(argv[1]);
	if (fd < 0)
	{
		printf("open port err!\n");
		return -1;
	}

	iRet = set_opt(fd, 115200, 8, 'N', 1);
	if (iRet)
	{
		printf("set port err!\n");
		return -1;
	}

	
	write(fd, cmd, strlen(cmd));

	sleep(1);

	len = read(fd,line, 1024);
	if (len && strstr(line, "Linux"))
	{
		printf("Serial ok\n");
		return 0;
	}
	else
	{
		printf("Serial err \n ");

		return -1;
	}
}

