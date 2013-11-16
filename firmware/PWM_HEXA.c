#include <LPC17xx.H>

static long Coef_10us;
static int NextTablePWM[20];		//Table pour le prochain cycle
static int TablePWM[20];				//Table PWM actuel 
static int Etat_TIMER0;				//Etat actuel dans l'envoi des PWM
//static int Etat_TIMER1;
static int Wait1;							//Attend la fin de la perdiode de 2 ms 
//static int Wait2;




void vLoadMR0(int Compteur_PWM1);
void GPIO_maj(int pin, int level);
void vMajTab1(void);
void vInitTab(void);

/*****************************************************************************
																	INIT TINMER 0 
******************************************************************************/
void vInitPWM(int Frequency)
{
	long Valeur_PWM_Start=0;
	float Calcul = 0 ; 
	
	LPC_GPIO0->FIODIR = 0x0FFFFF; // init GPIO 0 to 19 output
 	
	Etat_TIMER0=0;
	Wait1=0;
	vInitTab();
	
	Calcul = Frequency* 1000000.0;		// Calcul Coef pour avoir des multiple de 10us
	Calcul = 4.0/Calcul;
	Coef_10us = 0.00001/Calcul;
		
	LPC_SC->PCONP |= 1 << 1; 		//Power up Timer0
	LPC_SC->PCLKSEL0 |= 1 << 3; // Clock for timer = CCLK/2

	Valeur_PWM_Start = 100;			// 100*10us = 1ms
	Valeur_PWM_Start = Valeur_PWM_Start*Coef_10us; 
	
	LPC_TIM0->MR0 = Valeur_PWM_Start;
	LPC_TIM0->MCR |= 1 << 0; 		// Interrupt on Match0 compare
	LPC_TIM0->TCR |= 1 << 1; 		// Reset Timer0
	LPC_TIM0->TCR &= 0 << 1;		// Stop Reset
	

	NVIC_EnableIRQ(TIMER0_IRQn); 	// Enable timer interrupt
	LPC_TIM0->TCR |= 1 << 0; 			// Start timer

}

/*****************************************************************************
														Configuration MR0
******************************************************************************/
void vLoadMR0(int Compteur_PWM1)
{
	long PWM_time;
	
	PWM_time = TablePWM[Compteur_PWM1];
	
	PWM_time = PWM_time * Coef_10us; 
	LPC_TIM0->MR0 = PWM_time;
}

/*****************************************************************************
																	INT TIMER 0 
******************************************************************************/


void TIM0_IRQHandler(void)
{
	if((LPC_TIM0->IR & 0x01) == 0x01) // if MR0 interrupt
	{
		LPC_TIM0->IR |= 1 << 0; // Clear MR0 interrupt flag
		
		if(Wait1==0)
		{
			Wait1=1;																				 
			TablePWM[Etat_TIMER0]=200-TablePWM[Etat_TIMER0]; // Calcul la durée restante avant la prochaine PWM
			vLoadMR0(Etat_TIMER0);
			GPIO_maj(Etat_TIMER0,0);
		}
		else
		{
			Wait1=0;
			Etat_TIMER0++;
			if(Etat_TIMER0>9)Etat_TIMER0=0;
			vLoadMR0(Etat_TIMER0);
			GPIO_maj(Etat_TIMER0,1);
		}
			
		LPC_TIM0->TCR |= 1 << 1; // Reset Timer 0
		LPC_TIM0->TCR &= 0 << 1;
		LPC_TIM0->TCR |= 1 << 0; // Start timer
		
		if((Etat_TIMER0==9)&&(Wait1==1))vMajTab1();				//Mise à jour du tableau
	}
}

/*****************************************************************************
																Commande GPIO
******************************************************************************/
void GPIO_maj(int pin, int level)
{
	if(level==1)LPC_GPIO0->FIOPIN |= 1 << pin; 
	else LPC_GPIO0->FIOCLR |= 1 << pin;
}

/*****************************************************************************
													Mise à jour des PWM 1 à 10
******************************************************************************/


void vMajTab1(void)
{
	int i;
	for (i=0; i<9; i++) 
	{
		TablePWM[i] =	NextTablePWM[i];				
	}
}

/*****************************************************************************
														Remplisage des valeurs de base 
******************************************************************************/


void vInitTab(void)
{
	int i;
	for (i=0; i<19; i++) 
	{
		NextTablePWM[i]	= 150;	
		TablePWM[i]			=	150;				
	}
}


