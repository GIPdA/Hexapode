
#ifndef _UART0_H
#define _UART0_H

#include <stdint.h>
#include "lpc_types.h"
#include "FreeRTOS.h"


portBASE_TYPE xUart0_init(void);

portBASE_TYPE xUart0_getByte(uint8_t *pu8Byte, portTickType xTicksToWait);
portBASE_TYPE xUart0_sendByte(uint8_t u8Byte, portTickType xTicksToWait);

portBASE_TYPE xUart0_sendString(char *pcString, unsigned int *pu32BytesWritten, portTickType xTicksToWait);
portBASE_TYPE xUart0_sendData(unsigned char *pu8Data, unsigned int u32DataSize, 
                              unsigned int *pu32BytesWritten, portTickType xTicksToWait);

#endif // _UART0_H
