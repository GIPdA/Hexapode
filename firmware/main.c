
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
#include "decoder.h"


#define SC	(const signed char*)


void vTask(void *pvParameters);
void vReceiverTask(void *pvParameters);

unsigned long ulTaskNumber[configEXPECTED_NO_RUNNING_TASKS];

unsigned int prv_axServoOutputs[20] = {0};




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

