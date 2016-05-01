#include "commbase.h"

#define BAUDRATE B115200
#define LAUNCHPAD "/dev/ttyACM0"

int fd;  //Designator for comm channel
struct termios oldtio;
struct termios newtio;

int EstablishComPort(const char* name)
{
  char portname[100];
  int fail=0;
  if(!name)
    sprintf(portname,"%s",LAUNCHPAD);
  else
    sprintf(portname,"%s",name);
  printf("Opening Serial port %s\n",portname);
  fd=open(portname,O_RDWR|O_NOCTTY);
  if(fd>0)
  {
    fcntl(fd,F_SETFL,0);
    printf("Open command returned %d\n",fd);
    tcgetattr(fd,&oldtio);
    bzero(&newtio,sizeof(newtio));
    cfsetispeed(&newtio,BAUDRATE);
    cfsetospeed(&newtio,BAUDRATE);
    newtio.c_cflag = CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR | ICRNL;
    newtio.c_oflag = 0;
    newtio.c_lflag = 0;
    newtio.c_cc[VINTR]    = 0;     /* Ctrl-c */ 
    newtio.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
    newtio.c_cc[VERASE]   = 0;     /* del */
    newtio.c_cc[VKILL]    = 0;     /* @ */
    newtio.c_cc[VEOF]     = 4;     /* Ctrl-d */
    newtio.c_cc[VTIME]    = 0;     /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */
//    newtio.c_cc[VSWTC]    = 0;     /* '\0' */
    newtio.c_cc[VSTART]   = 0;     /* Ctrl-q */ 
    newtio.c_cc[VSTOP]    = 0;     /* Ctrl-s */
    newtio.c_cc[VSUSP]    = 0;     /* Ctrl-z */
    newtio.c_cc[VEOL]     = 0;     /* '\0' */
    newtio.c_cc[VREPRINT] = 0;     /* Ctrl-r */
    newtio.c_cc[VDISCARD] = 0;     /* Ctrl-u */
    newtio.c_cc[VWERASE]  = 0;     /* Ctrl-w */
    newtio.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
    newtio.c_cc[VEOL2]    = 0;     /* '\0' */
    tcflush(fd, TCIFLUSH);
    if(tcsetattr(fd,TCSANOW,&newtio))
    {
      printf("Error setting port attributes %d\n",errno);
      fail=1;
    }
  }
  else
    fail=1;
  return fail?-1:fd;
}

int WriteComPort(int portHandle,int ncw,const char* wbuff)
{
  int rwcode = write(portHandle,wbuff,ncw);
  printf("%d\n",rwcode);
  return -1;
}

int ReadComPort(int portHandle,int nbx,unsigned char* wbuff)
{
  int bytesRead=0;
  int res;
  while(bytesRead<nbx)
  {
  	res=ioctl(portHandle,FIONREAD,&bytesRead);
//        printf("Read %d bytes of expected %d from port %d  Return status is %d\n",bytesRead,nbx,portHandle,res);
  }
  int nbRead=read(portHandle,wbuff,nbx);
  wbuff[nbx]=0;
  printf("%d %d %s\n",nbRead,bytesRead,wbuff);
  wbuff[nbRead]=0;
  return nbRead;
}

void CloseComPort(int portHandle)
{
}
