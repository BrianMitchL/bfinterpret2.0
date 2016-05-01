/*
 * portfunc.c
 *
 *  Created on: Mar 8, 2016
 *      Author: kevin
 */
#include "portfunc.h"

void SelectPortFunction(int port,int line,int sel0,int sel1)
{
	if(port==1)
	{
		if((P1SEL0 & BIT(line))!=sel0)
		{
			if((P1SEL1 & BIT(line))!=sel1)
				P1SELC|=BIT(line);
			else
				P1SEL0^=BIT(line);
		}
		else
		{
			if((P1SEL1 & BIT(line))!=sel1)
				P1SEL1^=BIT(line);
		}
	}
	else if(port==2)
	{
		if((P2SEL0 & BIT(line))!=sel0)
		{
			if((P2SEL1 & BIT(line))!=sel1)
				P2SELC|=BIT(line);
			else
				P2SEL0^=BIT(line);
		}
		else
		{
			if((P2SEL1 & BIT(line))!=sel1)
				P2SEL1^=BIT(line);
		}
	}
	else if(port==3)
	{
		if ((P3SEL0 & BIT(line)) != sel0) {
			if ((P3SEL1 & BIT(line)) != sel1)
				P3SELC |= BIT(line);
			else
				P3SEL0 ^= BIT(line);
		} else {
			if ((P3SEL1 & BIT(line)) != sel1)
				P3SEL1 ^= BIT(line);
		}
	}
	else if(port==4)
	{
		if ((P4SEL0 & BIT(line)) != sel0) {
			if ((P4SEL1 & BIT(line)) != sel1)
				P4SELC |= BIT(line);
			else
				P4SEL0 ^= BIT(line);
		} else {
			if ((P4SEL1 & BIT(line)) != sel1)
				P4SEL1 ^= BIT(line);
		}
	}
	else if(port==5)
	{
		if ((P5SEL0 & BIT(line)) != sel0) {
			if ((P5SEL1 & BIT(line)) != sel1)
				P5SELC |= BIT(line);
			else
				P5SEL0 ^= BIT(line);
		} else {
			if ((P5SEL1 & BIT(line)) != sel1)
				P5SEL1 ^= BIT(line);
		}
	}
	else if(port==6)
	{
		if ((P6SEL0 & BIT(line)) != sel0) {
			if ((P6SEL1 & BIT(line)) != sel1)
				P6SELC |= BIT(line);
			else
				P6SEL0 ^= BIT(line);
		} else {
			if ((P6SEL1 & BIT(line)) != sel1)
				P6SEL1 ^= BIT(line);
		}
	}
}



