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

#include "FreeRTOS.h"
#include "queue.h"

#include "UartConfig.h"

//#include "system_specific.h"


#ifndef UART0_BAUDRATE
#error UART 0 baudrate is not specified. Please define UART0_BAUDRATE in UartConfig.h to desired baudrate.
#endif

/*#ifndef OSC_CLK
#error System Core Clock is not specified. Please define OSC_CLK in system_specific.h.
#endif // */

#ifndef UART0_RX_QUEUE_SIZE
#error Rx queue size of UART 0 not defined. Please define UART0_RX_QUEUE_SIZE in UartConfig.h to desired queue size.
#endif

#ifndef UART0_TX_QUEUE_SIZE
#error Tx queue size of UART 0 not defined. Please define UART0_TX_QUEUE_SIZE in UartConfig.h to desired queue size.
#endif


#define DIVISOR_LATCH_VALUE(PCLK,BAUDRATE)   ((PCLK) / 16 / (BAUDRATE))
#define DIVISOR_LATCH_LSB(PCLK,BAUDRATE)     (DIVISOR_LATCH_VALUE(PCLK,BAUDRATE) &0xFF)
#define DIVISOR_LATCH_MSB(PCLK,BAUDRATE)     ((DIVISOR_LATCH_VALUE(PCLK,BAUDRATE) >> 8)&0xFF)


/** UART Queues handles */
xQueueHandle xRxQueue;
xQueueHandle xTxQueue;


/**
 * @brief Init UART 0 to 8-bit, no parity, 1 stop
 *  Baudrate is defined by UART_0_BAUDRATE
 *
 * @return None
 */
portBASE_TYPE xUart0_init()
{
    int pclk;

    // Init FreeRTOS Queues
    xRxQueue = xQueueCreate(UART0_RX_QUEUE_SIZE, sizeof(unsigned char));
    xTxQueue = xQueueCreate(UART0_TX_QUEUE_SIZE, sizeof(unsigned char));

    if (!xRxQueue || !xTxQueue)
        return pdFALSE;

    // PCLK_UART0 is being set to 1/4 of SystemCoreClock
    pclk = SystemCoreClock / 4;
    
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

    LPC_UART0->IER = IER_RBR | IER_THRE | IER_RLS;        // Enable UART0 interrupt
    
    return pdTRUE;
}


/**
 * @brief Get a byte from UART, read from queue
 *
 * @param pu8Byte pointer where byte read will be stored. Must not be null.
 * @param xTicksToWait see xQueueReceive
 * @return xQueueReceive() return value
 */
portBASE_TYPE xUart0_getByte(uint8_t *pu8Byte, portTickType xTicksToWait)
{
    return xQueueReceive(xRxQueue, (void*)pu8Byte, xTicksToWait);
}



/**
 * @brief Using IT with Tx FIFO empty, put a char into Tx FIFO to initiate sending
 */
static
void vUart0_initSending()
{
    uint8_t u8ByteToSend;

    if (!(LPC_UART0->LSR & LSR_THRE))
        return;

    // Fill Tx FIFO
    while ((LPC_UART0->LSR & LSR_THRE) && xQueueReceive(xTxQueue, (void*)&u8ByteToSend, 0))
    {
        LPC_UART0->THR = u8ByteToSend;
    }
}


/**
 * @brief Send a byte over UART by queue
 *
 * @param u8Byte Byte to send
 * @param xTicksToWait see xQueueSendToBack
 * @return xQueueSendToBack() return value
 */
portBASE_TYPE xUart0_sendByte(uint8_t u8Byte, portTickType xTicksToWait)
{
    portBASE_TYPE status = xQueueSendToBack(xTxQueue, (void*)&u8Byte, xTicksToWait);
    vUart0_initSending();
    return status;
}


/**
 * @brief Send a string over UART
 *
 * @param pcString Zero-terminated string
 * @param pu32BytesWritten Get the number of bytes effectively written to queue. Set to 0 if not used
 * @param xTicksToWait see xQueueSendToBack
 * @return xQueueSendToBack() return value
 */
portBASE_TYPE xUart0_sendString(char *pcString, unsigned int *pu32BytesWritten, portTickType xTicksToWait)
{
    portBASE_TYPE status;

    if (pu32BytesWritten)
        *pu32BytesWritten = 0;

    for (; pcString[0] != '\0'; pcString++) { // Loop through until reach string's zero terminator
        status = xUart0_sendByte(pcString[0], xTicksToWait);

        if (status != pdPASS)
            return status;

        if (pu32BytesWritten)
            *pu32BytesWritten++;
    }
    return pdPASS;
}


/**
 * @brief Send data over UART
 *
 * @param pu8Data pointer to data
 * @param u32DataSize number of bytes to send
 * @param pu32BytesWritten Get the number of bytes effectively written to queue. Set to 0 if not used
 * @param xTicksToWait see xQueueSendToBack
 * @return xQueueSendToBack() return value
 */
portBASE_TYPE xUart0_sendData(unsigned char *pu8Data, unsigned int u32DataSize, 
                              unsigned int *pu32BytesWritten, portTickType xTicksToWait)
{
    portBASE_TYPE status;
    unsigned int i;

    if (pu32BytesWritten)
        *pu32BytesWritten = 0;

    for (i = 0; i < u32DataSize; i++) { // Loop through until reach string's zero terminator
        status = xUart0_sendByte(pu8Data[i], xTicksToWait);

        if (status != pdPASS)
            return status;

        if (pu32BytesWritten)
            *pu32BytesWritten++;
    }
    return pdPASS;
}



/**
 * @brief UART 0 IRQ handler
 * @todo Use ring buffer
 */
void UART0_IRQHandler (void) __irq
{
    uint8_t IIRValue, LSRValue;
    uint8_t u8ByteToSend;
    uint8_t u8Dummy = u8Dummy;    // Avoid warning
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

        
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
            u8Dummy = LPC_UART0->RBR;
            xQueueSendToBackFromISR(xRxQueue, (void*)&u8Dummy, &xHigherPriorityTaskWoken);  
        }
    }

    // Receive Data Available
    else if (IIRValue == IIR_RDA)
    {
        u8Dummy = LPC_UART0->RBR;
        xQueueSendToBackFromISR(xRxQueue, (void*)&u8Dummy, &xHigherPriorityTaskWoken); 
    }

    // Character timeout indicator
    else if (IIRValue == IIR_CTI)
    {
        //
    }

    // THRE, transmit holding register empty
    else if (IIRValue == IIR_THRE)
    {
        // Fill Tx FIFO
        while ((LPC_UART0->LSR & LSR_THRE) && xQueueReceiveFromISR(xTxQueue, (void*)&u8ByteToSend, &xHigherPriorityTaskWoken))
        {
            LPC_UART0->THR = u8ByteToSend;
        }
    }
    
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}





