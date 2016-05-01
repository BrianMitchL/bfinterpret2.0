/*
 * clock.c
 *
 *  Created on: Jan 23, 2016
 *      Author: kevin
 */

#include "clock.h"

// Use this for the older version of CCStudio
//void SetClockFrequency(void)
//{
//	CSKEY = 0x695A;
//	CSCTL0 = 0x00030000;
//	CSCTL1 = 0x00000233;
//	CSCLKEN = 0x0000800F;
//	CSKEY = 0xA596;
//}
//
//unsigned int ReadClockFrequency(void)
//{
//	unsigned int qret=CSCTL0;
//	return qret;
//}

// Use this for the newer version of CCStudio
void SetClockFrequency(void)
{
	CS->KEY = 0x695A;
	CS->CTL0 = 0x00030000;
	CS->CTL1 = 0x00000233;
	CS->CLKEN = 0x0000800F;
	CS->KEY = 0xA596;
}

unsigned int ReadClockFrequency(void)
{
	unsigned int qret=CS->CTL0;
	return qret;
}


