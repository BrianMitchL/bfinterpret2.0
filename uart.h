/*
 * uart.h
 *
 *  Created on: Jan 24, 2016
 *      Author: kevin
 */

#ifndef BFINTERPRET2_0_0_UART_H_
#define BFINTERPRET2_0_0_UART_H_

#include "msp.h"

#define BUFFERLENGTH 190

void ConfigureUART(void);
void ReadUARTPortSettings(void);
struct UARTBuf
{
  unsigned char First;
  unsigned char Last;
  unsigned char Data[BUFFERLENGTH];
};
void AddCharToBuf(struct UARTBuf*,unsigned char);
int TakeCharFromBuf(struct UARTBuf*);
unsigned char getBufferLength(struct UARTBuf*);
void FlushBuf(struct UARTBuf*);
void ForceFirstChar();
void writeChar(unsigned char);
enum OutRadix{DEC,HEX};
void writeValue(unsigned char,enum OutRadix);
void writeULong(unsigned long);
void dumpULong(unsigned long);
void writeUShort(unsigned short);
void writeMValue(unsigned char*,unsigned char);
void writeRegValue(unsigned char,unsigned char);
void write3RegValue(unsigned char,unsigned char*);
void writeString(const char*);
void UARTInterrupt(void);

#endif /* BFINTERPRET2_0_0_UART_H_ */
