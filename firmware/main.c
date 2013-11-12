#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"

#include "basic_io.h"


unsigned char ucServoLine[20];
xTimerHandle xServoPeriodHandle;
xTimerHandle xServoWidthHandle;
unsigned long uiServoID = 1, uiServoWidthID = 2;
unsigned char ucCurrentServoWidthStep = 0;
unsigned char ucServoPosition[10] = {1, 2, 3, 4, 5, 1, 2, 3, 4, 5};


unsigned long ulTaskNumber[configEXPECTED_NO_RUNNING_TASKS];


void vTask(void *pvParameters);



void vServoTimerCallback(xTimerHandle pxTimer)
{
    unsigned char i;
    for (i = 0; i < 10; i++) {
        ucServoLine[i] = 1;
    }

    ucCurrentServoWidthStep = 0;
    xTimerChangePeriod(xServoWidthHandle, 10, 0);
    xTimerStart(xServoWidthHandle, 0);
}

void vServoWidthTimerCallback(xTimerHandle pxTimer)
{
    unsigned char i;

    ucCurrentServoWidthStep++;

    for (i = 0; i < 10; i++) {
        if (ucServoPosition[i] == ucCurrentServoWidthStep)
            ucServoLine[i] = 0;
    }

		if (ucCurrentServoWidthStep == 1)
        xTimerChangePeriod(xServoWidthHandle, 2, 0);
    else if (ucCurrentServoWidthStep == 5)
        xTimerStop(xServoWidthHandle, 0);
}


int main(void)
{
    xServoPeriodHandle = xTimerCreate("ServoPeriod", 
        200,                    // 20ms
        pdTRUE,                 // Auto reload
        (void*)&uiServoID,      // ID
        vServoTimerCallback);

    xServoWidthHandle = xTimerCreate("ServoWidthPeriod", 
        2,                          // 200µs
        pdTRUE,                     // Auto reload
        (void*)&uiServoWidthID,     // ID
        vServoWidthTimerCallback);
    
    xTaskCreate(vTask, "Task 1", 200, NULL, 1, NULL);

    if (xTimerStart(xServoPeriodHandle, 0) != pdPASS)
    {
        // The timer could not be set into the Active state.
    }

    vTaskStartScheduler();

    while (1);
}

void vTask(void *pvParameters)
{
    while (1) {
        vTaskDelay(5000);
    }
}

