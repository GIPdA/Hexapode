#include <LPC17xx.H>

static long Coef_10us;
static int NextTablePWM[20];		//Table pour le prochain cycle
static int TablePWM[20];				//Table PWM actuel 
static int Etat_TIMER0;				//Etat actuel dans l'envoi des PWM
static int Etat_TIMER1;
static int Wait1;							//Attend la fin de la perdiode de 2 ms 
static int Wait2;


void vInitTIMER(long PWM_load);
void vLoadMR0(int Compteur_PWM1);
void vLoadMR1(int Compteur_PWM2);
void GPIO_maj(int pin, int level);
void Setup_PWM(int pin, int Valeurs);
void vMajTab1(void);
void vMajTab2(void);
void vInitTab(void);


/*****************************************************************************
																	INIT PWM
******************************************************************************/
void vInitPWM(int Frequency)
{
	long PWM_Start=0;
	float Calcul = 0 ; 
	
	LPC_SC->PCONP |= ( 1 << 15 ); // power up GPIO
	LPC_GPIO0->FIODIR = 0x0FFFFF; // init GPIO 0 to 19 output
 	
	Etat_TIMER0=0;
	Wait1=0;
	vInitTab();
	
	Calcul = Frequency* 1000000.0;		// Calcul Coef pour avoir des multiple de 10us
	Calcul = 4.0/Calcul;
	Coef_10us = 0.00001/Calcul;
	

	PWM_Start = 100;			// 100*10us = 1ms
	PWM_Start = PWM_Start*Coef_10us; 
	vInitTIMER(PWM_Start);
	

}
/*****************************************************************************
														Configuration TIMER
******************************************************************************/
void vInitTIMER(long PWM_load)
{
	LPC_SC->PCONP |= 1 << 1; 		//Power up Timer0
	LPC_SC->PCLKSEL0 |= 1 << 3; // Clock for timer = CCLK/2
	
	LPC_TIM0->MR0 = PWM_load;
	LPC_TIM0->MCR |= 1 << 0; 		// Interrupt on Match0 compare
	LPC_TIM0->TCR |= 1 << 1; 		// Reset Timer0
	LPC_TIM0->TCR &= 0 << 1;		// Stop Reset
	

	LPC_SC->PCONP |= 1 << 1; 		//Power up Timer1
	LPC_SC->PCLKSEL0 |= 1 << 5; // Clock for timer = CCLK/2
	
	
	LPC_TIM1->MR1 = PWM_load;
	LPC_TIM1->MCR |= 1 << 3; 		// Interrupt on Match1 compare
	LPC_TIM1->TCR |= 1 << 1; 		// Reset Timer0
	LPC_TIM1->TCR &= 0 << 1;		// Stop Reset
	

	NVIC_EnableIRQ(TIMER1_IRQn); 	// Enable timer interrupt
	NVIC_EnableIRQ(TIMER0_IRQn); 	// Enable timer interrupt
	LPC_TIM0->TCR |= 1 << 0; 			// Start timer0
	LPC_TIM1->TCR |= 1 << 0; 			// Start timer1
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
														Configuration MR1
******************************************************************************/
void vLoadMR1(int Compteur_PWM2)
{
	long PWM_time;
	
	PWM_time = TablePWM[Compteur_PWM2];
	
	PWM_time = PWM_time * Coef_10us; 
	LPC_TIM1->MR1 = PWM_time;
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
		
		if((Etat_TIMER1==9)&(Wait1==1))vMajTab1();				//Mise à jour du tableau
	}
}

/*****************************************************************************
																	INT TIMER 1 
******************************************************************************/

void TIM1_IRQHandler(void)
{
	if((LPC_TIM1->IR & 0x02) == 0x02) // if MR1 interrupt
	{
		LPC_TIM1->IR |= 1 << 0; // Clear MR1 interrupt flag
		
		if(Wait2==0)
		{
			Wait2=1;																				 
			TablePWM[Etat_TIMER1+10]=200-TablePWM[Etat_TIMER1+10]; 	// Calcul la durée restante avant la prochaine PWM
			vLoadMR1(Etat_TIMER1+10);													// +10 décalage sur le PORT pins 10 à 20
			GPIO_maj(Etat_TIMER1+10,0);
		}
		else
		{
			Wait2=0;
			Etat_TIMER1++;
			if(Etat_TIMER1>9)Etat_TIMER1=0;
			vLoadMR1(Etat_TIMER1+10);
			GPIO_maj(Etat_TIMER1+10,1);
		}
			
		LPC_TIM1->TCR |= 1 << 1; // Reset Timer 1
		LPC_TIM1->TCR &= 0 << 1;
		LPC_TIM1->TCR |= 1 << 0; // Start timer
		
		if((Etat_TIMER1==9)&(Wait2==1))vMajTab2();				//Mise à jour du tableau
	}

}

/*****************************************************************************
														 Changement Valeurs 
******************************************************************************/

void Setup_PWM(int pin, int Valeurs)
{
	if(Valeurs>199)Valeurs=199;
	if(Valeurs<100)Valeurs=100;
	NextTablePWM[pin]	= Valeurs;
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
	for (i=0; i<10; i++) 
	{
		TablePWM[i] =	NextTablePWM[i];				
	}
}

/*****************************************************************************
													Mise à jour des PWM 10 à 20
******************************************************************************/


void vMajTab2(void)
{
	int i;
	for (i=10; i<21; i++) 
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
	for (i=0; i<20; i++) 
	{
		NextTablePWM[i]	= 150;	
		TablePWM[i]			=	150;				
	}
}


