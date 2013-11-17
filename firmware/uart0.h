
#ifndef _UART0_H
#define _UART0_H

#include "lpc_types.h"

void vUart0Init(void);
bool bUart0SendByte(const uint8 u8Byte);
bool bUart0SendStr(char *pcString);
bool bUart0SendConstStr(const char * const pcString);


#endif // _UART0_H
