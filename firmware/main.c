
#include <lpc17xx.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lpc_types.h"

#include "FreeRTOS.h"
#include "task.h"

#include "basic_io.h"

#include "uart0.h"
#include "servodriver.h"


#define SC	(const signed char*)


void vTask(void *pvParameters);
void vReceiverTask(void *pvParameters);

unsigned long ulTaskNumber[configEXPECTED_NO_RUNNING_TASKS];

unsigned int prv_axServoOutputs[20] = {0};




typedef enum _eDecoderStates
{
    eOk,
    eOverflow
} eDecoderStates;


void vDecoder_sendSuccess(void)
{
    xUart0_sendString("OK\n", 0, portMAX_DELAY);
}

void vDecoder_sendError(void)
{
    xUart0_sendString("ERR\n", 0, portMAX_DELAY);
}


bool bDecoder_checkServoPos(char *pcBuffer)
{
    int i32ServoNumber = 0;
    int i32PulseWidth = 0;
    double fAngle = 0.0;
    
    // SPnn wwww
    // SAnn ffff

    if (pcBuffer[0] != 'S' || pcBuffer[4] != ' ')
        return false;

    i32ServoNumber = atoi(&pcBuffer[2]);
    if (i32ServoNumber < 1 || i32ServoNumber > 20) {
        // Servo index out of range
        printf("Servo index out of range\n");
        return false;
    }
    
    // Decrement number to fit array index
    i32ServoNumber--;

    // Servo index in range, get/check position
    if (pcBuffer[1] == 'P') {
        // Get position as pulse width (µs)
        i32PulseWidth = atoi(&pcBuffer[5]);

        if ((i32PulseWidth < 1000) || (i32PulseWidth > 2000)) {
            // Error, width out of range
            printf("Pulse width out of range\n");
            return false;
        }
        
        vServoDriver_setServoPosition((unsigned int)i32ServoNumber, (unsigned int)i32PulseWidth);
        return true;

    } else if (pcBuffer[1] == 'A') {
        // Get position as angle
        fAngle = atof(&pcBuffer[5]);

        if (fAngle < 0.0 || fAngle > 180.0) {
            // Error, angle out of range
            printf("Angle out of range\n");
            return false;
        }
        
        vServoDriver_setServoAngle((unsigned int)i32ServoNumber, (float)fAngle);
        return true;
    }
    
    printf("Unrecognized S cmd\n");

    return false;
}


bool bDecoder_processCommand(char *pcBuffer)
{
    //
    if (bDecoder_checkServoPos(pcBuffer)) {
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



int main(void)
{
    //
    
    if (!xUart0_init())
    {
        printf("UART0 init failed\n");
        while (1);
    }

    vServoDriver_init();
    //vServoDriver_setServoPosition(0, 1000);
    //vServoDriver_setServoPosition(1, 1000);

    //xTaskCreate(vTask, SC"Task 1", 200, NULL, 1, NULL);
    xTaskCreate(vATCommandsDecoderTask, SC"Command Decoder", 200, NULL, 2, NULL);

    vTaskStartScheduler();

    for(;;);
}

void vTask(void *pvParameters)
{
    while (1) {
        xUart0_sendString("Hexapode", 0, 100);
        vTaskDelay(1000);
    }
}

