
#ifndef _UART0_H
#define _UART0_H

#include "lpc_types.h"


void vUart0_init(void);
bool bUart0_sendByte(const uint8 u8Byte);
bool bUart0_sendStr(char *pcString);
bool bUart0_sendConstStr(const char * const pcString);


#endif // _UART0_H
