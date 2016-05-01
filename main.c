//*****************************************************************************
//
// MSP432 main.c template - Empty main
//
//****************************************************************************

#include "msp.h"
#include "portfunc.h"
#include "clock.h"
#include "uart.h"

unsigned char memloc[10000];
unsigned char progLoc[10000];

unsigned long testCRC(int nw,unsigned short* data)
{
	int i;
	int rv=1;
	if(nw<0)
	{
		rv=0;
		nw=-nw;
	}
	for(i=0;i<nw;i++)
	{
		if(rv)
			CRC32DIRB=data[i];
		else
			CRC32DI=data[i];
	}
	return CRC32RESR_LO|(CRC32RESR_HI<<16);
}

extern struct UARTBuf TXBuf;
extern struct UARTBuf RXBuf;

int loadBFprog(void)
{
	CRC32INIRES_LO=0xFFFF;
	CRC32INIRES_HI=0xFFFF;
	unsigned long len=0;
	unsigned char* lp=(unsigned char*)&len;
	int i;
	for(i=0;i<4;++i)
	{
		while(!getBufferLength(&RXBuf));
		int k=TakeCharFromBuf(&RXBuf);
		*lp=(unsigned char)k;
		lp++;
	}
	lp=progLoc;
	unsigned short crctval=0;
	for(i=0;i<len;++i)
	{
		while(!getBufferLength(&RXBuf));
		int k=TakeCharFromBuf(&RXBuf);
		*lp=(unsigned char)k;
		if(i%2)
		{
			crctval=(*lp)|(*(lp-1)<<8);
			(void)testCRC(1,&crctval);
		}
		else if(i==len-1)
		{
			crctval=((*lp)<<8);
			(void)testCRC(1,&crctval);
		}
		lp++;
	}

	unsigned short csv[2];
	lp=(unsigned char*)csv;
	while(!getBufferLength(&RXBuf));
	int k=TakeCharFromBuf(&RXBuf);
	*lp=(unsigned char)k;
	lp++;
	while(!getBufferLength(&RXBuf));
	k=TakeCharFromBuf(&RXBuf);
	*lp=(unsigned char)k;
	(void)testCRC(1,csv);
	lp=(unsigned char*)(&csv[1]);
	while(!getBufferLength(&RXBuf));
	k=TakeCharFromBuf(&RXBuf);
	*lp=(unsigned char)k;
	lp++;
	while(!getBufferLength(&RXBuf));
	k=TakeCharFromBuf(&RXBuf);
	*lp=(unsigned char)k;

	return !!testCRC(1,csv+1);
}

unsigned long progSize(int* odd)
{
	unsigned char* lp=progLoc;
	unsigned long ps=0;
	*odd=0;
	while(*lp)
	{
		ps+=1;
		lp++;
	}
	if(ps%2)
	{
		*odd=1;
		ps++;
	}
	return ps;
}

void main(void)
{

	WDTCTL = WDTPW | WDTHOLD;           // Stop watchdog timer
	SetClockFrequency();
	ConfigureUART();
	NVIC_EnableIRQ(EUSCIA0_IRQn);
	for(;;)
	{
		if(getBufferLength(&RXBuf))
		{
			int cmd=TakeCharFromBuf(&RXBuf);
			switch(cmd)
			{
			case 'L':
			case 'l':
				if(!loadBFprog())
				{
					writeChar('K');
					ForceFirstChar();
				}
				else
				{
					writeChar('F');
					ForceFirstChar();
				}
				break;
			case 'X':
			case 'x':
				break;
			case 'S':
			case 's':
			{
				CRC32INIRES_LO=0xFFFF;
				CRC32INIRES_HI=0xFFFF;
				int isOdd;
				unsigned long ps=progSize(&isOdd);
				dumpULong(ps);
				ForceFirstChar();
				unsigned long crcret=testCRC(ps/2,(unsigned short*)progLoc);
				unsigned char* lp=progLoc;
				while(*lp)
				{
					writeChar(*lp);
					ForceFirstChar();
					lp++;
				}
				if(isOdd)
				{
					writeChar(*lp);
				}
				dumpULong(crcret);
				//ForceFirstChar();
				break;
			}
			}
		}
	}
}

