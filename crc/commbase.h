#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <unistd.h>

#ifndef COMMBASE_INCLUDED
#define COMMBASE_INCLUDED

int EstablishComPort(const char*);
int WriteComPort(int,int,const char*);
int ReadComPort(int,int,unsigned char*);
void CloseComPort(int);

#endif
