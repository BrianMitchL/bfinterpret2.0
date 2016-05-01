/*
 * uart.c
 *
 *  Created on: Jan 24, 2016
 *      Author: kevin
 */

#include "uart.h"

static unsigned char ForceNeeded=0;


void ConfigureUART(void)
{
	UCA0CTLW0|=(unsigned short)0x0001;
	UCA0CTLW0=(unsigned short)0x0081;
	unsigned int cctl=UCA0CTLW0;
	UCA0BRW=(unsigned short)0x0006;
	UCA0MCTLW=(unsigned short)0x2081;
	P1SEL0|=(BIT2|BIT3);
	P1SEL1&=~(BIT2|BIT3);
	UCA0CTLW0&=(unsigned short)~0x0001;
	UCA0IE=0x0003;
	cctl=UCA0CTLW0;
	ForceNeeded=1;
}

void ReadUARTPortSettings(void)
{
	unsigned int ps1=P1SEL0;
	unsigned int ps2=P1SEL1;
	unsigned int ctl=UCA0CTLW0;
	unsigned int brw=UCA0BRW;
	unsigned int mctl=UCA0MCTLW;
	ps1=ps2;
}

struct UARTBuf TXBuf={0};
struct UARTBuf RXBuf={0};
struct UARTBuf CmdBuf={0};

void AddCharToBuf(struct UARTBuf* buf,unsigned char c)
{
	buf->Data[buf->Last]=c;
	buf->Last=buf->Last<BUFFERLENGTH-1?buf->Last+1:0;
	if(buf==&TXBuf && buf->Last==buf->First+1)
		ForceNeeded=1;
}

int TakeCharFromBuf(struct UARTBuf* buf)
{
	if(buf->First==buf->Last)
		return -1;
	int rv=buf->Data[buf->First];
	buf->First=buf->First<BUFFERLENGTH-1?buf->First+1:0;
	return rv;
}

unsigned char getBufferLength(struct UARTBuf* buf)
{
	if(buf->First==buf->Last)
		return 0;
	else if(buf->Last>buf->First)
		return buf->Last-buf->First;
	else
		return BUFFERLENGTH-(buf->First-buf->Last);
}

void FlushBuf(struct UARTBuf* buf)
{
	buf->First=buf->Last=0;
}

void ForceFirstChar()
{
	if(ForceNeeded && getBufferLength(&TXBuf))
	{
		ForceNeeded=0;
		int nc=TakeCharFromBuf(&TXBuf);
		unsigned char n=(unsigned char)nc;
		UCA0TXBUF=n;
	}
}

void writeChar(unsigned char c)
{
	AddCharToBuf(&TXBuf,c);
}

void writeValue(unsigned char c,enum OutRadix rx)
{
	if(rx==DEC)
	{
		if(c>100)
		{
			AddCharToBuf(&TXBuf,(c/100)+'0');
			c%=100;
		}
		else
			AddCharToBuf(&TXBuf,'0');
		if(c>10)
		{
			AddCharToBuf(&TXBuf,(c/10)+'0');
			c%=10;
		}
		else
			AddCharToBuf(&TXBuf,'0');
		AddCharToBuf(&TXBuf,c);
	}
	else
	{
		AddCharToBuf(&TXBuf,'0');
		AddCharToBuf(&TXBuf,'x');
		unsigned char q=c/16;
		if(q<=9)
			AddCharToBuf(&TXBuf,q+'0');
		else
			AddCharToBuf(&TXBuf,q+'W');
		q=c%16;
		if(q<=9)
			AddCharToBuf(&TXBuf,q+'0');
		else
			AddCharToBuf(&TXBuf,q+'W');
	}
}

void writeULong(unsigned long v)
{
	unsigned char* q=(unsigned char*)&v;
	int i;
	for(i=0;i<4;++i)
		writeValue(*(q+i),HEX);
}

void writeUShort(unsigned short v)
{
	unsigned char* q=(unsigned char*)&v;
	int i;
	for(i=0;i<2;++i)
		writeValue(*(q+i),HEX);
	writeChar(0x20);
}

void dumpULong(unsigned long v)
{
	unsigned char* q=(unsigned char*)&v;
	int i;
	for(i=0;i<4;++i)
		writeChar(*(q+i));
}

void writeMValue(unsigned char* b,unsigned char len)
{
	AddCharToBuf(&TXBuf,'0');
	AddCharToBuf(&TXBuf,'x');
	unsigned char i;
	for(i=0;i<len;++i)
	{
		unsigned char q=b[i]/16;
		if(q<=9)
			AddCharToBuf(&TXBuf,q+'0');
		else
			AddCharToBuf(&TXBuf,q+'W');
		q=b[i]%16;
		if(q<=9)
			AddCharToBuf(&TXBuf,q+'0');
		else
			AddCharToBuf(&TXBuf,q+'W');
	}
}

void writeRegValue(unsigned char reg,unsigned char rv)
{
	AddCharToBuf(&TXBuf,'%');
	AddCharToBuf(&TXBuf,reg);
	AddCharToBuf(&TXBuf,'@');
	AddCharToBuf(&TXBuf,rv);
}

void write3RegValue(unsigned char reg,unsigned char* rv)
{
	AddCharToBuf(&TXBuf,'%');
	AddCharToBuf(&TXBuf,reg);
	AddCharToBuf(&TXBuf,'#');
	int i;
	for(i=0;i<3;++i)
		AddCharToBuf(&TXBuf,rv[i]);
}


void writeString(const char* ss)
{
	const char* s1=ss;
	while(*s1)
	{
		AddCharToBuf(&TXBuf,*s1);
		s1++;
	}
}

void UARTInterrupt(void)
{
	unsigned short qq=UCA0IV;
	if(qq&0x02)
	{
		unsigned char rc=UCA0RXBUF;
		AddCharToBuf(&RXBuf,rc);
	}
	else if(qq==0x04)
	{
		int nchar=TakeCharFromBuf(&TXBuf);
		if(nchar>=0)
		{
			unsigned char tc=(unsigned char)nchar;
			//      while(!(UCA1IFG&UCTXIFG));
			UCA0TXBUF=tc;
		}
		else
			ForceNeeded=1;
	}
	/*else
	{
		if(UCA0IFG)
	    {
	      while(!(UCA0IFG&UCTXIFG));
	      UCA0TXBUF=qq+'0';
	    }
	}*/
}
