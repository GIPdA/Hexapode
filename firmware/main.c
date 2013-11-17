#include "FreeRTOS.h"
#include "task.h"
#include "LPC17xx.H"
#include "PWM_HEXA.H"



//#include "basic_io.h"


void vTask(void *pvParameters);

unsigned long ulTaskNumber[configEXPECTED_NO_RUNNING_TASKS];


int main(void)
{
	LPC_SC->PCONP |= ( 1 << 15 ); // power up GPIO
	vInitPWM(24);
	
	Setup_PWM(0, 90);
	Setup_PWM(1,100);
	Setup_PWM(2,110);
	Setup_PWM(3,120);
	Setup_PWM(4,130);
	Setup_PWM(5,140);
	Setup_PWM(6,150);
	Setup_PWM(7,160);
	Setup_PWM(8,170);
	Setup_PWM(9,180);

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

