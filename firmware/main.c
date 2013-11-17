
#include <lpc17xx.h>
#include "lpc_types.h"

#include "FreeRTOS.h"
#include "task.h"

#include "basic_io.h"

#include "uart0.h"



void vTask(void *pvParameters);

unsigned long ulTaskNumber[configEXPECTED_NO_RUNNING_TASKS];

int main(void)
{
    vUart0Init();
    bUart0SendStr("HEXAPODE UART TEST");


    xTaskCreate(vTask, "Task 1", 200, NULL, 1, NULL);

    vTaskStartScheduler();

    for(;;);
}

void vTask(void *pvParameters)
{
    while (1) {
        //
    }
}

