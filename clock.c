/*
 * clock.c
 *
 *  Created on: Jan 23, 2016
 *      Author: kevin
 */

#include "clock.h"

void SetClockFrequency(void)
{
	CSKEY = 0x695A;
	CSCTL0 = 0x00030000;
	CSCTL1 = 0x00000233;
	CSCLKEN = 0x0000800F;
	CSKEY = 0xA596;
}

unsigned int ReadClockFrequency(void)
{
	unsigned int qret=CSCTL0;
	return qret;
}




