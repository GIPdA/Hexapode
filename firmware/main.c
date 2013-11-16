#include "FreeRTOS.h"
#include "task.h"
#include "LPC17xx.H"
#include "PWM_HEXA.H"



//#include "basic_io.h"


void vTask(void *pvParameters);

unsigned long ulTaskNumber[configEXPECTED_NO_RUNNING_TASKS];


int main(void)
{
	
	vInitPWM(25);

	xTaskCreate(vTask, "Task 1", 200, NULL, 1, NULL);

  vTaskStartScheduler();

  while(1);
}

void vTask(void *pvParameters)
{
  while (1) 
	{
		//
	}
}

