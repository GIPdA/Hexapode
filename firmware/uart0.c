/**
 * @brief -FILL-
 *
 * @todo Fill brief.
 * @todo update sendStr/sendData with buffer
 * @todo use ringbuffer in IT
 * @todo use semaphore
 * @todo make getData(ptr, size) with FreeRTOS semaphore
 */


#include <lpc17xx.h>
#include "uart0.h"
#include "uart_private.h"

#include "SizedRingBuffer.h"

#include "UartConfig.h"

#include "system_specific.h"


#ifndef UART0_BAUDRATE
#error UART 0 baudrate is not specified. Please define UART0_BAUDRATE in UartConfig.h to desired baudrate.
#endif

#ifndef OSC_CLK
#error System Core Clock is not specified. Please define OSC_CLK in system_specific.h.
#endif

#ifndef UART0_RX_BUFFER_SIZE
#error Rx buffer size of UART 0 not defined. Please define UART0_RX_BUFFER_SIZE in UartConfig.h to desired buffer size.
#endif

#ifndef UART0_TX_BUFFER_SIZE
#error Tx buffer size of UART 0 not defined. Please define UART0_TX_BUFFER_SIZE in UartConfig.h to desired buffer size.
#endif


#define DIVISOR_LATCH_VALUE(PCLK,BAUDRATE)   ((PCLK) / 16 / (BAUDRATE))
#define DIVISOR_LATCH_LSB(PCLK,BAUDRATE)     (DIVISOR_LATCH_VALUE(PCLK,BAUDRATE) &0xFF)
#define DIVISOR_LATCH_MSB(PCLK,BAUDRATE)     ((DIVISOR_LATCH_VALUE(PCLK,BAUDRATE) >> 8)&0xFF)


static uint8_t u8RxBuffer[UART0_RX_BUFFER_SIZE];
static uint8_t u8TxBuffer[UART0_TX_BUFFER_SIZE];

static xSizedRingBuffer xRxRingBuffer = RINGBUFFER_COMPILETIME_INIT(u8RxBuffer,UART0_RX_BUFFER_SIZE);
static xSizedRingBuffer xTxRingBuffer = RINGBUFFER_COMPILETIME_INIT(u8TxBuffer,UART0_TX_BUFFER_SIZE);

#define RXRB  &xRxRingBuffer
#define TXRB  &xTxRingBuffer

static xBufferState xLastRxBufferState = Ok;
static xBufferState xLastTxBufferState = Ok;


/**
 * @brief Init UART 0 to 8-bit, no parity, 1 stop
 *  Baudrate is defined by UART_0_BAUDRATE
 *
 * @return None
 */
void vUart0_init()
{
    int pclk;

    // Buffers are already initialized at compile-time.

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
    LPC_UART0->DLM = DIVISOR_LATCH_MSB(pclk, UART0_BAUDRATE);                            
    LPC_UART0->DLL = DIVISOR_LATCH_LSB(pclk, UART0_BAUDRATE); 

    LPC_UART0->LCR = 0x03;      // 8 bits, no Parity, 1 Stop bit DLAB = 0
    LPC_UART0->FCR = 0x07;      // Enable and reset TX and RX FIFO

    // Enable IRQ
    NVIC_EnableIRQ(UART0_IRQn);

    LPC_UART0->IER = IER_RBR | IER_THRE | IER_RLS;        /* Enable UART0 interrupt */
}


bool bUart0_isRxBufferEmpty()
{
    return bSRB_isEmpty(RXRB);
}

static inline
bool bUart0_isTxFIFOFull()
{
    // THRE = 1 => data can be pushed in UART0 FIFO
    return ((LPC_UART0->LSR & LSR_THRE) == 0);
}

xUartBufferState xUart0_lastRxBufferError()
{
    return xLastRxBufferState;
}

xUartBufferState xUart0_lastTxBufferError()
{
    return xLastTxBufferState;
}

uint16_t u16Uart0_emptyTxBufferCount()
{
    return xTxRingBuffer.u16MaxSize - xTxRingBuffer.u16Count;
}

uint16_t u16Uart0_txBufferCount()
{
    return u16SRB_count(TXRB);
}

uint16_t u16Uart0_emptyRxBufferCount()
{
    return xRxRingBuffer.u16MaxSize - xRxRingBuffer.u16Count;
}

uint16_t u16Uart0_rxBufferCount()
{
    return u16SRB_count(RXRB);
}


/**
 * @brief Get a byte from UART
 * @sa xUart0_lastRxBufferError
 *
 * @param pu8Byte pointer where byte read will be stored. Must not be null.
 * @return true on success, false otherwise
 */
bool u8Uart0_getByte(uint8_t *pu8Byte)
{
    // No need to check buffer state or pointer, xSRB_pop do it.

    xLastRxBufferState = xSRB_pop(RXRB, pu8Byte);

    if (xLastRxBufferState <= 0)
        return false; // Error occured

    //while ((LPC_UART0->LSR & LSR_RDR) == 0);  // Nothing received so just block
    //return LPC_UART0->RBR; // Return receiver buffer register
    return true;
}



/**
 * @brief Using IT with Tx FIFO empty, put a char into Tx FIFO to initiate sending
 */
static
void vUart0_initSending()
{
    uint8_t u8ByteToSend;

    // Fill Tx FIFO
    while(!bUart0_isTxFIFOFull() && !bSRB_isFull(TXRB))
    {
        if (xSRB_pop(TXRB, &u8ByteToSend) <= 0)
            return; // Return if error

        LPC_UART0->THR = u8ByteToSend;
    }
}


/**
 * @brief Send a byte over UART
 * @sa xUart0_lastTxBufferError
 *
 * @param u8Byte Byte to send
 * @return true if byte successfully pushed into buffer, false otherwise.
 */
bool bUart0_sendByte(const uint8_t u8Byte)
{
    xLastTxBufferState = xSRB_push(TXRB, u8Byte);

    if (xLastTxBufferState <= 0)
        return false;   // Pushing byte in buffer failed (usually because buffer is full)

    if (!bUart0_isTxFIFOFull())
        vUart0_initSending();

    //while ((LPC_UART0->LSR & LSR_THRE) == 0);  // Block until tx empty
    //LPC_UART0->THR = u8Byte;

    return true;
}


/**
 * @brief Send a string over UART
 * @todo Add timeout
 * @todo Use ringbuffer
 */
bool bUart0_sendStr(char *pcString)
{
    for (; pcString[0] != '\0'; pcString++) // Loop through until reach string's zero terminator
        if (bUart0_sendByte(pcString[0]) == false)
            return false;
    return true;
}

/**
 * @brief Send a string over UART
 * @todo Add timeout
 * @todo Use ringbuffer
 */
bool bUart0_sendConstStr(const char * const pcString)
{
    uint32_t i;
    for (i = 0; pcString[i] != '\0'; i++)
        if (bUart0_sendByte(pcString[i]) == false)
            return false;
    return true;
}



/**
 * @brief UART 0 IRQ handler
 * @todo Use ring buffer
 */
void UART0_IRQHandler (void) 
{
  uint8_t IIRValue, LSRValue;
  uint8_t u8Dummy = u8Dummy;    // Avoid warning
        
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
          //UART0Status = LSRValue;
          u8Dummy = LPC_UART0->RBR;                // Dummy read on RX to clear interrupt, then bail out
          return;
        }

        // Receive Data Ready
        if (LSRValue & LSR_RDR)
        {
          // If no error on RLS, normal ready, save into the data buffer.
          // Note: read RBR will clear the interrupt
          /*UART0Buffer[UART0Count] = LPC_UART0->RBR;
          UART0Count++;
          if (UART0Count == BUFSIZE)
          {
                UART0Count = 0;                // buffer overflow
          } // */       
        }
  }

  // Receive Data Available
  else if (IIRValue == IIR_RDA)
  {
        /*UART0Buffer[UART0Count] = LPC_UART0->RBR;
        UART0Count++;
        if ( UART0Count == BUFSIZE )
        {
          UART0Count = 0;                // buffer overflow
        }// */
  }

  // Character timeout indicator
  else if (IIRValue == IIR_CTI)
  {
        //UART0Status |= 0x100;                // Bit 9 as the CTI error
  }

  // THRE, transmit holding register empty
  else if (IIRValue == IIR_THRE)
  {
        LSRValue = LPC_UART0->LSR;                // Check status in the LSR to see if valid data in U0THR or not

        if ( LSRValue & LSR_THRE )
        {
          //UART0TxEmpty = 1;
        }
        else
        {
          //UART0TxEmpty = 0;
        }
  }
    
}





