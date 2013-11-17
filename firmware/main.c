#include "FreeRTOS.h"
#include "task.h"
#include "LPC17xx.H"
#include "PWM_HEXA.H"



//#include "basic_io.h"


void vTask(void *pvParameters);

unsigned long ulTaskNumber[configEXPECTED_NO_RUNNING_TASKS];


int main(void)
{
	vInitPWM(24);
	
	Setup_PWM(10, 90);
	Setup_PWM(11,100);
	Setup_PWM(12,110);
	Setup_PWM(13,120);
	Setup_PWM(14,130);
	Setup_PWM(15,140);
	Setup_PWM(16,150);
	Setup_PWM(17,160);
	Setup_PWM(18,170);
	Setup_PWM(19,180);

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

