#include <LPC17xx.H>

long Coef_10us;

/*****************************************************************************
																	INIT TINMER 0 
******************************************************************************/
void vInitPWM(int Frequency)
{
	long Valeur_PWM_Start=0;
	float Calcul = 0 ; 
	
	Calcul = Frequency* 1000000.0;		// Calcul Coef pour avoir des multiple de 10us
	Calcul = 4.0/Calcul;
	Coef_10us = 0.00001/Calcul;
		
	LPC_SC->PCONP |= 1 << 1; 		//Power up Timer0
	LPC_SC->PCLKSEL0 |= 1 << 3; // Clock for timer = CCLK/2

	Valeur_PWM_Start = 100;			// 100*10us = 1ms
	Valeur_PWM_Start = Valeur_PWM_Start*Coef_10us; 
	
	LPC_TIM0->MR0 = Valeur_PWM_Start;
	//LPC_TIM0->MR0 = 1 << 10; 		// Give a value suitable for the LED blinking frequency based on the clock frequency
	LPC_TIM0->MCR |= 1 << 0; 		// Interrupt on Match0 compare
	LPC_TIM0->TCR |= 1 << 1; 		// Reset Timer0
	LPC_TIM0->TCR &= 0 << 1;		// Stop Reset
	

	NVIC_EnableIRQ(TIMER0_IRQn); 	// Enable timer interrupt
	LPC_TIM0->TCR |= 1 << 0; 			// Start timer

}

/*****************************************************************************
																	Conf MR0
******************************************************************************/
void vLoadMR0(long PWM_time_x10us)
{
	
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
