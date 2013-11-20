
#ifndef _UART0_H
#define _UART0_H

#include <stdint.h>
#include "lpc_types.h"


void vUart0_init(void);

bool u8Uart0_getByte(uint8_t *pu8Byte);
bool bUart0_sendByte(const uint8_t u8Byte);



bool bUart0_isRxBufferEmpty(void);

uint16_t u16Uart0_emptyTxBufferCount(void);
uint16_t u16Uart0_txBufferCount(void);
uint16_t u16Uart0_emptyRxBufferCount(void);
uint16_t u16Uart0_rxBufferCount(void);

#endif // _UART0_H
