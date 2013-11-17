
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

#ifndef UART_0_BUFFER_SIZE
#error Buffer size of UART 0 not defined. Please define UART_0_BUFFER_SIZE in UartConfig.h to desired buffer size.
#endif

#ifndef UART_0_BUFFER_WARNING_LIMIT
#warning Buffer warning limit of UART 0 not user-defined. Limit defaults to 1.
#define UART_0_BUFFER_WARNING_LIMIT 1
#endif


#define DIVISOR_LATCH_VALUE(PCLK,Baudrate)   ((PCLK) / 16 / (Baudrate))
#define DIVISOR_LATCH_LSB(PCLK,Baudrate)     (DIVISOR_LATCH_VALUE(PCLK,Baudrate) &0xFF)
#define DIVISOR_LATCH_MSB(PCLK,Baudrate)     ((DIVISOR_LATCH_VALUE(PCLK,Baudrate) >> 8)&0xFF)


static volatile uint8_t pu8Buffer[UART_0_BUFFER_SIZE];
static volatile uint16_t pu16BufferCount;
static volatile uint16_t pu16BufferStart;
static volatile uint16_t pu16BufferEnd;


void vBufferInit()
{
    pu16BufferStart = 0;
    pu16BufferCount = 0;
    pu16BufferEnd = 0;
}

xBufferState xBufferPush(uint8_t u8Byte)
{
    // Buffer is full, this byte would overflow the buffer: ignore it
    if (bBufferIsFull())
        return Overflow;

    // Store byte in buffer at end index
    pu8Buffer[pu16BufferEnd++] = u8Byte;

    ++pu16BufferCount;

    if (pu16BufferEnd >= UART_0_BUFFER_SIZE) // Ring buffer, back to zero if max size reached
        pu16BufferEnd = 0;

    // Buffer is now full
    if (bBufferIsFull())
        return Full;

    // Buffer is almost full
    if (pu16BufferCount == UART_0_BUFFER_SIZE - UART_0_BUFFER_WARNING_LIMIT)
        return AlmostFull;

    return Ok;
}

xBufferState xBufferPop(uint8_t *u8Byte)
{
    if (!u8Byte) return Error;

    if (bBufferIsEmpty())
        return Empty;

    // Get byte
    u8Byte = pu8Buffer[pu16BufferStart];

 #ifdef UART_0_BUFFER_RESET_ON_POP
    pu8Buffer[pu16BufferStart] = 0;
 #endif

    ++pu16BufferStart;

    --pu16BufferCount;

    if (pu16BufferStart >= UART_0_BUFFER_SIZE) // Ring buffer, back to zero if max size reached
        pu16BufferStart = 0;

    // Buffer is now empty
    if (bBufferIsEmpty())
        return NowEmpty;

    return Ok;
}

xBufferState xBufferPeek(uint8_t *u8Byte)
{
    if (!u8Byte) return Error;

    if (bBufferIsEmpty())
        return Empty;

    // Get byte
    u8Byte = pu8Buffer[pu16BufferStart];

    return Ok;
}


xBufferState xBufferState()
{
    if (bBufferIsEmpty())
        return Empty;
    if (bBufferIsFull())
        return Full;
    if (pu16BufferCount == UART_0_BUFFER_SIZE - UART_0_BUFFER_WARNING_LIMIT)
        return AlmostFull;

    return Ok;
}

uint16_t u16BufferCount()
{
    return pu16BufferCount;
}

bool bBufferIsEmpty()
{
    return (pu16BufferCount == 0);
}

bool bBufferIsFull()
{
    return (pu16BufferCount == UART_0_BUFFER_SIZE);
}




void vUart0Init()
{
    int pclk;

    vBufferInit();

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

    // Enable IRQ
    NVIC_EnableIRQ(UART0_IRQn);

    LPC_UART0->IER = IER_RBR | IER_THRE | IER_RLS;        /* Enable UART0 interrupt */
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



void UART0_IRQHandler (void) 
{
  uint8_t IIRValue, LSRValue;
  uint8_t u8Dummy;
        
  IIRValue = LPC_UART0->IIR;
    
  IIRValue >>= 1;                        // Skip pending bit in IIR
  IIRValue &= 0x07;                      // Check bit 1~3, interrupt identification

  if (IIRValue == IIR_RLS)                // Receive Line Status
  {
        LSRValue = LPC_UART0->LSR;

        // Receive Line Status
        if (LSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI))
        {
          // There are errors or break interrupt
          // Read LSR will clear the interrupt
          UART0Status = LSRValue;
          u8Dummy = LPC_UART0->RBR;                // Dummy read on RX to clear interrupt, then bail out
          return;
        }

        // Receive Data Ready
        if (LSRValue & LSR_RDR)
        {
          // If no error on RLS, normal ready, save into the data buffer.
          // Note: read RBR will clear the interrupt
          UART0Buffer[UART0Count] = LPC_UART0->RBR;
          UART0Count++;
          if (UART0Count == BUFSIZE)
          {
                UART0Count = 0;                // buffer overflow
          }        
        }
  }

  // Receive Data Available
  else if (IIRValue == IIR_RDA)
  {
        UART0Buffer[UART0Count] = LPC_UART0->RBR;
        UART0Count++;
        if ( UART0Count == BUFSIZE )
        {
          UART0Count = 0;                // buffer overflow
        }
  }

  // Character timeout indicator
  else if (IIRValue == IIR_CTI)
  {
        UART0Status |= 0x100;                // Bit 9 as the CTI error
  }

  // THRE, transmit holding register empty
  else if (IIRValue == IIR_THRE)
  {
        LSRValue = LPC_UART0->LSR;                // Check status in the LSR to see if valid data in U0THR or not

        if ( LSRValue & LSR_THRE )
        {
          UART0TxEmpty = 1;
        }
        else
        {
          UART0TxEmpty = 0;
        }
  }
    
}





