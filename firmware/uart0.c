
#include <lpc17xx.h>
#include "uart0.h"
#include "uart_private.h"

#include "UartConfig.h"

#include "system_specific.h"


#ifndef UART_0_BAUDRATE
#error UART 0 baudrate is not specified. Please define UART_0_BAUDRATE in UartConfig.h to desired baudrate.
#endif

#ifndef OSC_CLK
#error System Core Clock is not specified. Please define OSC_CLK in system_specific.h.
#endif


#define DIVISOR_LATCH_VALUE(PCLK,Baudrate)   ((PCLK) / 16 / (Baudrate))
#define DIVISOR_LATCH_LSB(PCLK,Baudrate)     (DIVISOR_LATCH_VALUE(PCLK,Baudrate) &0xFF)
#define DIVISOR_LATCH_MSB(PCLK,Baudrate)     ((DIVISOR_LATCH_VALUE(PCLK,Baudrate) >> 8)&0xFF)


void vUart0Init()
{
    int pclk;

    // PCLK_UART0 is being set to 1/4 of SystemCoreClock
    pclk = OSC_CLK / 4;
    
    // Turn on power to UART0
    LPC_SC->PCONP |=  PCUART0_POWERON;
    
    // Turn on UART0 peripheral clock
    LPC_SC->PCLKSEL0 &= ~(PCLK_UART0_MASK);
    LPC_SC->PCLKSEL0 |=  (0 << PCLK_UART0);     // PCLK_periph = CCLK/4
    
    // Set PINSEL0 so that P0.2 = TXD0, P0.3 = RXD0
    LPC_PINCON->PINSEL0 &= ~0xf0;
    LPC_PINCON->PINSEL0 |= ((1 << 4) | (1 << 6));
    
    LPC_UART0->LCR = 0x83;      // 8 bits, no Parity, 1 Stop bit, DLAB=1

    // Set baudrate
    LPC_UART0->DLM = DIVISOR_LATCH_MSB(pclk, UART_0_BAUDRATE);                            
    LPC_UART0->DLL = DIVISOR_LATCH_LSB(pclk, UART_0_BAUDRATE); 

    LPC_UART0->LCR = 0x03;      // 8 bits, no Parity, 1 Stop bit DLAB = 0
    LPC_UART0->FCR = 0x07;      // Enable and reset TX and RX FIFO
}


/**
 * @brief Send a byte over UART
 * @todo Add timeout
 */
bool bUart0SendByte(const uint8 u8Byte)
{
    while ((LPC_UART0->LSR & LSR_THRE) == 0);  // Block until tx empty

    LPC_UART0->THR = u8Byte;

    return true;
}

/**
 * @brief Get a byte from UART
 * @todo Add timeout
 */
uint8 u8Uart0GetByte()
{
    while ((LPC_UART0->LSR & LSR_RDR) == 0);  // Nothing received so just block

    return LPC_UART0->RBR; // Return receiver buffer register
}

/**
 * @brief Send a string over UART
 * @todo Add timeout
 */
bool bUart0SendStr(char *pcString)
{
    for (; pcString[0] != '\0'; pcString++) // Loop through until reach string's zero terminator
        if (bUart0SendByte(pcString[0]) == false)
            return false;
	return true;
}

/**
 * @brief Send a string over UART
 * @todo Add timeout
 */
bool bUart0SendConstStr(const char * const pcString)
{
    for (uint32 i = 0; pcString[i] != '\0'; i++)
        if (bUart0SendByte(pcString[i]) == false)
            return false;
    return true;
}






