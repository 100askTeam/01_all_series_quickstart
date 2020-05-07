#ifndef _SERIAL_H
#define _SERIAL_H

int open_port(char *com);
int set_opt(int fd,int nSpeed, int nBits, char nEvent, int nStop);

#endif
