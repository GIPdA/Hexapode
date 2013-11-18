
#ifndef _UART0_H
#define _UART0_H

#include "lpc_types.h"


void vUart0_init(void);
bool bUart0_sendByte(const uint8 u8Byte);
bool bUart0_sendStr(char *pcString);
bool bUart0_sendConstStr(const char * const pcString);

/// @brief Ring buffer states
typedef
enum _BufferState { 
	Error = 0, 			// Returned in case of null pointer
	Ok = 1, 			// Buffer contains data but not empty or (almost) full
	Empty = 2, 			// Buffer is empty
	NowEmpty = 3, 		// The read emptied the buffer
	//AlmostFull = 4, 	// The buffer is almost full (limit defined with UART_0_BUFFER_WARNING_LIMIT)
	Full = -1, 			// Buffer full
	Overflow = -2 		// Buffer push overflowed (the byte is not written)
} xUartBufferState;

#endif // _UART0_H
