#include "FreeRTOS.h"
#include "task.h"

#include "basic_io.h"


void vTask(void *pvParameters);

unsigned long ulTaskNumber[configEXPECTED_NO_RUNNING_TASKS];

int main(void)
{
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

