
#include <lpc17xx.h>
#include "lpc_types.h"

#include "FreeRTOS.h"
#include "task.h"

#include "basic_io.h"

#include "uart0.h"


#define SC	(const signed char*)


void vTask(void *pvParameters);

unsigned long ulTaskNumber[configEXPECTED_NO_RUNNING_TASKS];

int main(void)
{
    vUart0_init();
		bUart0_sendByte('H');
		bUart0_sendByte('e');
		bUart0_sendByte('x');
		bUart0_sendByte('a');
		bUart0_sendByte('p');
		bUart0_sendByte('o');


    xTaskCreate(vTask, SC"Task 1", 200, NULL, 1, NULL);

    vTaskStartScheduler();

    for(;;);
}

void vTask(void *pvParameters)
{
    while (1) {
        //
    }
}

