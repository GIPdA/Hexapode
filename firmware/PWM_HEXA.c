#include <LPC17xx.H>

/*****************************************************************************
																	INIT TINMER 0 
******************************************************************************/
void vTimerInit(void)
{
	LPC_SC->PCONP |= 1 << 1; //Power up Timer0
	LPC_SC->PCLKSEL0 |= 1 << 3; // Clock for timer = CCLK/2

	LPC_TIM0->MR0 = 1 << 10; // Give a value suitable for the LED blinking frequency based on the clock frequency
	LPC_TIM0->MCR |= 1 << 0; // Interrupt on Match0 compare
	LPC_TIM0->TCR |= 1 << 1; // Reset Timer0
	LPC_TIM0->TCR &= 0 << 1;
	

	NVIC_EnableIRQ(TIMER0_IRQn); // Enable timer interrupt

	LPC_TIM0->TCR |= 1 << 0; // Start timer

}

/*****************************************************************************
																	INT TIMER 0 
******************************************************************************/


void TIM0_IRQHandler(void)
{
	if((LPC_TIM0->IR & 0x01) == 0x01) // if MR0 interrupt
	{
		LPC_TIM0->IR |= 1 << 0; // Clear MR0 interrupt flag



		LPC_TIM0->TCR |= 1 << 1; // Reset Timer0
		LPC_TIM0->TCR &= 0 << 1;
		LPC_TIM0->TCR |= 1 << 0; // Start timer

	}
}
