
#include <lpc17xx.h>
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

int main(void)
{
    //
    
    if (!xUart0_init())
    {
        printf("UART0 init failed\n");
        while (1);
    }

    vServoDriver_init();

    xTaskCreate(vTask, SC"Task 1", 200, NULL, 1, NULL);
    xTaskCreate(vReceiverTask, SC"UART Receive", 200, NULL, 1, NULL);

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

void vReceiverTask(void *pvParameters)
{
    unsigned char byte;
    
    while (1) {
        if (xUart0_getByte(&byte, 1000)) {
            printf("Byte received: %c\n", byte);
        } else {
            printf("Error in receive\n");
        }
    }
}

