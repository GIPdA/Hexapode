
#include <lpc17xx.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include "uart0.h"
#include "decoder.h"
#include "decoder_servoposition.h"


typedef enum _eDecoderStates {
    eOk,
    eOverflow
} eDecoderStates;



static
void vDecoder_sendSuccess(void)
{
    xUart0_sendString("OK\n", 0, portMAX_DELAY);
}

static
void vDecoder_sendError(void)
{
    xUart0_sendString("ERR\n", 0, portMAX_DELAY);
}



static
bool bDecoder_processCommand(char *pcBuffer)
{
    //
    if (bDecoder_updateServoPosition(pcBuffer)) {
        return true;
    }
    
    return false;
}


void vATCommandsDecoderTask(void *pvParameters)
{
    unsigned char byte;
    char acBuffer[20];
    eDecoderStates eDecoderState = eOk;
    unsigned int u32CurrentIndex = 0;

    // AT+
    // AT+SP01 1500
    // AT+SA01 90.0
    
    while (1) {

        xUart0_getByte(&byte, portMAX_DELAY);

        printf("Byte received: %c (%x)\n", byte, byte);

        if (byte == '\n' || byte == '\r') {
            printf("End of cmd\n");
            if (eDecoderState == eOverflow) {
                // Error, send error command
                vDecoder_sendError();

                u32CurrentIndex = 0;
                eDecoderState = eOk;
                continue;
            }

            acBuffer[u32CurrentIndex] = 0;
            
            printf("Cmd: %s\n", acBuffer);

            // Check AT
            if (strncmp(acBuffer, "AT+", 3) != 0) {
                // Error, send error command
                vDecoder_sendError();
                printf("AT+ doesn't match");
                
                u32CurrentIndex = 0;
                eDecoderState = eOk;
                continue;
            }

            if (bDecoder_processCommand(&acBuffer[3])) {
                // Success, return OK
                vDecoder_sendSuccess();

                u32CurrentIndex = 0;
                eDecoderState = eOk;
                continue;
            }

            // Error, send error command
            vDecoder_sendError();
            printf("Command not decoded");

            u32CurrentIndex = 0;
            eDecoderState = eOk;
            continue;

        } else if (u32CurrentIndex < 20) {
            acBuffer[u32CurrentIndex++] = byte;
        } else {
            // Not enough room in buffer, wait end
            eDecoderState = eOverflow;
            printf("Overflow error\n");
        }

    }
}
