//*****************************************************************************
//
// MSP432 main.c template - Empty main
//
//****************************************************************************

#include "msp.h"
#include "clock.h"
#include "uart.h"
#include "portfunc.h"
#include <string.h>

unsigned char memloc[10000];
unsigned char progLoc[10000];
unsigned char* currentAddress;
unsigned char* currentInstruction;
unsigned char* returnTo[20];
unsigned char* jumpTo[20];

extern struct UARTBuf TXBuf;
extern struct UARTBuf RXBuf;


void InitializeOutput(unsigned char n)
{
	P1DIR|=BIT(n);
	SelectPortFunction(1,n,0,0);
	P1OUT|=BIT(n);
}

void outputByte(unsigned char q)
{
	writeChar(q);
	ForceFirstChar();
}

void inputByte(unsigned char* q)
{
	while(getBufferLength(&RXBuf));
	q==(unsigned char*)TakeCharFromBuf(&RXBuf);
}

int ProcessChar(unsigned char cmd)
{
	int step=1;
	switch(cmd)
	{
	case '>':
		currentAddress++;
		break;
	case '<':
		currentAddress--;
		break;
	case '+':
		(*currentAddress)++;
		break;
	case '-':
		(*currentAddress)--;
		break;
	case '.':
		outputByte(*currentAddress);
		break;
	case ',':
		inputByte(currentAddress);
		break;
	case '[':
		if(*currentAddress==0)
		{
			unsigned char stackDepth=0;
			unsigned char*currentIP=currentInstruction+1;
			while(*currentIP!=']'||stackDepth>0)
			{
				if(*currentIP==']')
					stackDepth-=1;
				if(*currentIP=='[')
					stackDepth+=1;
				currentIP+=1;
			}
			currentInstruction=currentIP;
		}
		step=1;
		break;
	case ']':
	{
		if(*currentAddress!=0)
		{
			unsigned char stackDepth=0;
			unsigned char*currentIP=currentInstruction-1;
			while(*currentIP!='['||stackDepth>0)
			{
				if(*currentIP==']')
					stackDepth+=1;
				if(*currentIP=='[')
					stackDepth-=1;
				currentIP-=1;
			}
			currentInstruction=currentIP;
		}
		step=1;
		break;
	}
	case 0:
		step=-1;
	}
	return step;
}

int runBFprog()
{
  currentAddress=memloc;
  memset(memloc,0,10000);
  currentInstruction=progLoc;
  int rv;
  while((rv=ProcessChar(*currentInstruction))>-1)
  {
	  if(rv)
	  {
		  currentInstruction++;
	  }
  }
  return rv;
}

unsigned long len;
unsigned char* lp=(unsigned char*)&len;

unsigned long CRC(int len, unsigned short* data)
{
	int i;
	for(i=0; i<len; i++)
	{
		CRC32DIRB=data[i];
	}
	return (CRC32RESR_HI<<16)|CRC32RESR_LO;
}

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
			(void)CRC(1,&crctval);
		}
		else if(i==len-1)
		{
			crctval=((*lp)<<8);
			(void)CRC(1,&crctval);
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
	(void)CRC(1,csv);
	lp=(unsigned char*)(&csv[1]);
	while(!getBufferLength(&RXBuf));
	k=TakeCharFromBuf(&RXBuf);
	*lp=(unsigned char)k;
	lp++;
	while(!getBufferLength(&RXBuf));
	k=TakeCharFromBuf(&RXBuf);
	*lp=(unsigned char)k;

	return !!CRC(1,csv+1);
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
	WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer
    SetClockFrequency();
    InitializeOutput(2);
    InitializeOutput(3);
    ConfigureUART();
    NVIC_EnableIRQ(EUSCIA0_IRQn);

	for(;;)
    	{
		if(getBufferLength(&RXBuf))
		{
			int command=TakeCharFromBuf(&RXBuf); //read file
			switch(command)
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
			    			runBFprog();
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
			    			unsigned long crcret=CRC(ps/2,(unsigned short*)progLoc);
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
			    			break;
			    		}
			    		}
		}
    	}
}
