#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include "commbase.h"

class crc
{
protected:
  unsigned crcreg;
  unsigned table[256];
public:
  crc();
  ~crc(){};
  void Init(unsigned = (unsigned)-1);
  void Increment(unsigned char*,int);
  void IncrementBR(unsigned char*,int);
  void Increment1(unsigned char);
  void IncrementU2v(unsigned short*,int);
  void IncrementChar4(const char*);
  void IncrementU4(unsigned);
  void IncrementU4BR(unsigned);
  unsigned Report();
  unsigned Reflect();
};

crc::crc()
{
  unsigned c;
  for(int n=0;n<256;++n)
  {
    c=(unsigned)n;
    for(int k=0;k<8;++k)
    {
      if(c&1)
	c=0xedb88320L^(c>>1);
      else
	c=c>>1;
    }
    table[n]=c;
  }
}

void crc::Init(unsigned x)
{
  crcreg=x;
}

void crc::Increment(unsigned char* buf,int len)
{
  for(int n=0;n<len;++n)
    crcreg=table[(crcreg^buf[n])&0xff]^(crcreg>>8);
}

unsigned char ReverseByte(unsigned char btr)
{
  unsigned char ByteRev[]={0x00,0x08,0x04,0x0C,0x02,0x0A,0x06,0x0E,
			   0x01,0x09,0x05,0x0D,0x03,0x0B,0x07,0x0F};
  return ByteRev[btr&0x0F]<<4|ByteRev[(btr&0xF0)>>4];
}

void crc::IncrementBR(unsigned char* buf,int len)
{
  for(int n=0;n<len;++n)
  {
    unsigned char rbval=ReverseByte(buf[n]);
    crcreg=table[(crcreg^rbval)&0xff]^(crcreg>>8);
  }
}

void crc::Increment1(unsigned char b)
{
  Increment(&b,1);
}

void crc::IncrementU2v(unsigned short* v,int n)
{
  for(int i=0;i<n;++i)
  {
    unsigned short tv=v[i];
    unsigned char* uc=(unsigned char*)&tv;
    IncrementBR(uc+1,1);
    IncrementBR(uc,1);
  }
}

void crc::IncrementChar4(const char* ca)
{
  unsigned char uc[4];
  for(int i=0;i<4;++i)
    uc[i]=(unsigned)ca[i];
  Increment(uc,4);
}

void crc::IncrementU4(unsigned q)
{
  unsigned char uc[4];
  uc[0]=(q&0xff000000)>>24;
  uc[1]=(q&0x00ff0000)>>16;
  uc[2]=(q&0x0000ff00)>>8;
  uc[3]=(q&0x000000ff);
  Increment(uc,4);
}

void crc::IncrementU4BR(unsigned q)
{
  unsigned char uc[4];
  uc[0]=(q&0xff000000)>>24;
  uc[1]=(q&0x00ff0000)>>16;
  uc[2]=(q&0x0000ff00)>>8;
  uc[3]=(q&0x000000ff);
  IncrementBR(uc,4);
}


unsigned crc::Report()
{
  return crcreg^0xffffffffL;
}

unsigned crc::Reflect()
{
  unsigned char rvb[]={0x00,0x08,0x04,0x0c,0x02,0x0a,0x06,0x0e,0x01,
		       0x09,0x05,0x0d,0x03,0x0b,0x07,0x0f};
  unsigned tmprev=crcreg;
  unsigned makerv=0;
  for(int i=0;i<8;++i)
  {
    unsigned char msk=(tmprev&(0xf<<(i*4)))>>(i*4);
    makerv|=rvb[msk]<<((7-i)*4);
  }
  return makerv;
}

void writeCRCwrappedMessage(char* msg,int port)
{
  crc* CRC=new crc;
  CRC->Init();
  CRC->IncrementBR((unsigned char*)msg,strlen(msg));
  if(strlen(msg)%2)
  {
    unsigned char q=0;
    CRC->IncrementBR(&q,1);
  }
  unsigned xx=CRC->Report();
  printf("%x ",xx);
  xx=CRC->Reflect();
  printf("%x\n",xx);fflush(stdout);
  unsigned flen=strlen(msg);
  int j;
  if((j=WriteComPort(port,sizeof(unsigned),(char*)&flen))>=0)
  {
    printf("Com port failure &d\n",j);
    exit(0);
  }
  WriteComPort(port,flen,msg);
  unsigned short crca=(unsigned short)((xx&0xffff0000)>>16);
  WriteComPort(port,2,(char*)&crca);
  crca=(unsigned short)(xx&0x0000ffff);
  WriteComPort(port,2,(char*)&crca);
  delete CRC;
}

int main(void)
{
	char msg[]=">++++[>++++[>++++[<<<+>>>-]<-]<-]++[>++[>++[<<<+>>>-]<-]<-]<.>++[>++++[>++++[<<<+>>>-]<-]<-]<---.+++++++..+++.[-]>++[>++++[>++++[<<<+>>>-]<-]<-]<.>+++[>++++[>++++[<<<+>>>-]<-]<-]<+++++++.>++++[>++++[<<+>>-]<-]<++++++++.+++.------.--------.[-]>+++[>++++[>++++[<<<+>>>-]<-]<-]<--.[-]>+++[>++++[<<+>>-]<-]<+.---.";
		//{0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x00};
//  char msg[]="This is a text message.";
  int h=EstablishComPort("/dev/cu.usbmodem00000001");
  if(!h)
  {
    printf("Port not opened\n");
    exit(0);
  }
  if(WriteComPort(h,1,"L")>=0)
  {
    printf("Com port failure\n");
    exit(0);
  }
  writeCRCwrappedMessage((char*)msg,h);
  int nbw;
  unsigned char* rcvbuf=new unsigned char[200];
  int ct=0;
  if((nbw=ReadComPort(h,1,rcvbuf))<1)
     printf("Communication error.  Read %d bytes of %d\n",nbw,1);
  else
  {
    printf("%c %d\n",*(rcvbuf),ct);
    crc* rcrc=new crc;
    rcrc->Init();
    ct=nbw;
    unsigned len;
    WriteComPort(h,1,"X");
    for(int i=0; i<13; i++){
	  unsigned char q;	
	  nbw=ReadComPort(h,1,&q);
    printf("%c",q);
	}
    /*while(ct<len+1 && (nbw=ReadComPort(h,1,rcvbuf+ct))>0)
    {
      printf("%x %c %d\n",*(rcvbuf+ct),*(rcvbuf+ct),ct);
      ct+=nbw;
    }
    rcrc->IncrementU2v((unsigned short*)(rcvbuf+1),len/2);
    unsigned xx=rcrc->Report();
    printf("%x ",xx^0xFFFFFFFF);
    xx=rcrc->Reflect();
    printf("%x\n",xx);
    printf("%d\n",nbw);
    unsigned crcval;
    unsigned char* qq=(unsigned char*)&crcval;
    for(int i=0;i<4;++i)
    {
      nbw=ReadComPort(h,1,qq);
      printf("%d %x\n",nbw,*qq);
      qq++;
    }
    printf("%d %x\n",nbw,crcval);
    rcrc->Increment((unsigned char*)&crcval,4);
    printf("%s\n",rcvbuf);
    xx=rcrc->Report();
    printf("%x ",xx^0xFFFFFFFF);
    xx=rcrc->Reflect();
    printf("%x\n",xx);
    delete rcrc;*/
  }
  CloseComPort(h);
  return EXIT_SUCCESS;
}
