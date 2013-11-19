/*! ************************************************************************** *
 * @file PWM_HEXA.c
 * @brief librairie de controle hexapode
 *
 * Cette librairie permet de generé 20 PWM de cerveaux moteur sur le port 0
 * avec les timers 0 et 1.
 * 
 * Le lancement des PWM s'effectue avec la fonction vInitPWM(FREQUENCE) et 
 * on peut modifier le rapport cyclique de chaque PWM avec Setup_PWM(PIN,TEMPS).
 * PIN   = 1 => PORT0.1           (0<PIN<19)
 * TEMPS = 100 => 100*10us = 1ms  (100<TEMPS<200)
 *
 * Chaque timer réalise de façon successive 10 PWM.
 *
 * Exemple :
 *
 *      2ms
 *	   <----> 
 *                 20ms
 *     <---------------------------->
 *    |___                            ___
 * P1 |   |                          |   |
 *    |___|_____________ __ __ ______|___|_\
 *    |       ___                          /
 * P2 |      |   |
 *	  |______|_ _|______ __ __ ____________\
 *    |                        ___         /
 * P10|                       |   |
 *	  |_________________ __ __|___|________\
 *                                         / 
 *Zone de fonctionnement :
 *    |   | |   |                         
 *	  | 1 |2| 1 |
 *    |   | |   |
 *
 * Zone 1 :  Début: mise à 1 de la pin et lancement du timer (wait* = 0).
 *			 Fin : Interruption du timer passage de la pin à 0. 
 *
 * Zone 2 : Début : Lancement du timer jusqu'a la fin des 2ms alloué à cette PWM (wait=1).
 *			Fin : Interruption du timer lancement de la PWM suivante.
 *
 *   *wait: Flag qui permet d'identifier la zone actuel.
 *
 *
 *
 *
 *
 * @author Cédric CHRETIEN
 * @version 1.0
 *
 * @changelog 17-11-2013
 *
 *  ************************************************************************** */

#include <LPC17xx.H>

static long Coef_10us;
static int NextTablePWM[20];//Table pour le prochain cycle
static int TablePWM[20];	//Table PWM actuel 
static int Etat_TIMER0;	    //Définis la PWM en cour d'envoie
static int Etat_TIMER1;     
static int Wait1;           //Indicateur de fin de PWM 
static int Wait2;


void vInitTIMER(long PWM_load);
void vLoadMR0(int Compteur_PWM1);
void vLoadMR1(int Compteur_PWM2);
void GPIO_maj(int pin, int level);
void Setup_PWM(int pin, int Valeurs);
void vMajTab1(void);
void vMajTab2(void);
void vInitTab(void);



/*! ************************************************************************** *
 * @brief Initialisation des 20 PWM ( réglage GPIO et timer)
 *
 * @param Frequency, Fréquence du CCLK en MHz
 * @return Void
 *  ************************************************************************** */


void vInitPWM(int Frequency)
{
    long PWM_Start=0;
    float Calcul = 0 ; 
	
    LPC_SC->PCONP |= ( 1 << 15 ); // power up GPIO
    LPC_GPIO0->FIODIR = 0x0FFFFF; // init GPIO 0 to 19 output

    Etat_TIMER0=0;
    Wait1=0;
    vInitTab();

    Calcul = Frequency* 1000000.0;  // Calcul Coef pour avoir des multiples de 10us
    Calcul = 4.0/Calcul;
    Coef_10us = 0.00001/Calcul;


    PWM_Start = 100;    // 100*10us = 1ms
    PWM_Start = PWM_Start*Coef_10us; 
    vInitTIMER(PWM_Start);
	

}

/*! ************************************************************************** *
 * @brief Initialisation des TIMER0 et 1 CCLK/4.
 *
 * @param PWM_load PWM pour la valeur par défaut 
 * @return Void
 *  ************************************************************************** */

void vInitTIMER(long PWM_load)
{
    LPC_SC->PCONP |= 1 << 1; //Power up Timer0
    LPC_SC->PCLKSEL0 |= 1 << 3; // Clock for timer = CCLK/4

    LPC_TIM0->MR0 = PWM_load;
    LPC_TIM0->MCR |= 1 << 0; // Interrupt on Match0 compare
    LPC_TIM0->TCR |= 1 << 1; // Reset Timer0
    LPC_TIM0->TCR &= 0 << 1; // Stop Reset


    LPC_SC->PCONP |= 1 << 1; //Power up Timer1
    LPC_SC->PCLKSEL0 |= 1 << 5; // Clock for timer = CCLK/4


    LPC_TIM1->MR1 = PWM_load;
    LPC_TIM1->MCR |= 1 << 3; // Interrupt on Match1 compare
    LPC_TIM1->TCR |= 1 << 1; // Reset Timer0
    LPC_TIM1->TCR &= 0 << 1; // Stop Reset


    NVIC_EnableIRQ(TIMER1_IRQn); // Enable timer interrupt
    NVIC_EnableIRQ(TIMER0_IRQn); // Enable timer interrupt
    LPC_TIM0->TCR |= 1 << 0; // Start timer0
    LPC_TIM1->TCR |= 1 << 0; // Start timer1
}


/*! ************************************************************************** *
 * @brief Chargement de la valeur dans MR0
 *
 * @param Compteur_PWM1 Indice de la valeur dans le tableaux TablePWM
 * @return Void
 *  ************************************************************************** */

void vLoadMR0(int Compteur_PWM1)
{
    long PWM_time;

    PWM_time = TablePWM[Compteur_PWM1];

    PWM_time = PWM_time * Coef_10us; 
    LPC_TIM0->MR0 = PWM_time;
}

/*! ************************************************************************** *
 * @brief Chargement de la valeur dans MR1
 *
 * @param Compteur_PWM1 Indice de la valeur dans le tableaux TablePWM
 * @return Void
 *  ************************************************************************** */
 
void vLoadMR1(int Compteur_PWM2)
{
    long PWM_time;

    PWM_time = TablePWM[Compteur_PWM2];

    PWM_time = PWM_time * Coef_10us; 
    LPC_TIM1->MR1 = PWM_time;
}


/*! ************************************************************************** *
 * @brief Interruption du timer0 modification des état logique sur les GPIO
 *
 * @return Void
 *  ************************************************************************** */

void TIM0_IRQHandler(void)
{
    if((LPC_TIM0->IR & 0x01) == 0x01) // if MR0 interrupt
    {
        LPC_TIM0->IR |= 1 << 0; // Clear MR0 interrupt flag
        
        if(Wait1==0)    
        {
            Wait1=1;																				 
            TablePWM[Etat_TIMER0]=200-TablePWM[Etat_TIMER0];// Calcul la durée réstante avant la prochaine PWM
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
            
        LPC_TIM0->TCR |= 1 << 1;// Reset Timer 0
        LPC_TIM0->TCR &= 0 << 1;
        LPC_TIM0->TCR |= 1 << 0;// Start timer
        
        if((Etat_TIMER1==9)&(Wait1==1))vMajTab1();//Mise à jour du tableau
    }
}

/*! ************************************************************************** *
 * @brief Interruption du timer1 modification des état logique sur les GPIO
 *
 * @return Void
 *  ************************************************************************** */

void TIM1_IRQHandler(void)
{
    if((LPC_TIM1->IR & 0x02) == 0x02)// if MR1 interrupt
    {
        LPC_TIM1->IR |= 1 << 0;// Clear MR1 interrupt flag
        
        if(Wait2==0)
        {
            Wait2=1;																				 
            TablePWM[Etat_TIMER1+10]=200-TablePWM[Etat_TIMER1+10];// Calcul la durée restante avant la prochaine PWM
            vLoadMR1(Etat_TIMER1+10);// +10 décalage sur le PORT pins 10 à 20
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
        
        if((Etat_TIMER1==9)&(Wait2==1))vMajTab2();//Mise à jour du tableau
    }

}

/*! ************************************************************************** *
 * @brief Chargement d'une nouvelle PWM dans le Tableau
 *
 * @param pin le choix de la pin à modifier 
 * @param Valeurs en multiple de 10us comprise entre 100 et 200 
 * @return Void
 *  ************************************************************************** */

void Setup_PWM(int Pin, int Valeurs)
{
    if(Valeurs>199)Valeurs=199;
    if(Valeurs<100)Valeurs=100;
    NextTablePWM[Pin]	= Valeurs;
}

/*! ************************************************************************** *
 * @brief Configuration GPIO
 *
 * @param Pin le choix de la pin à modifier 
 * @param level état logique 0/1
 * @return Void
 *  ************************************************************************** */

void GPIO_maj(int pin, int level)
{
    if(level==1)LPC_GPIO0->FIOPIN |= 1 << pin; 
    else LPC_GPIO0->FIOCLR |= 1 << pin;
}

/*! ************************************************************************** *
 * @brief Mise à jour des valeurs du tableau de 0 à 9 
 *
 * @return Void
 *  ************************************************************************** */

void vMajTab1(void)
{
    int i;
    for (i=0; i<10; i++) 
    {
        TablePWM[i] =	NextTablePWM[i];				
    }
}

/*! ************************************************************************** *
 * @brief Mise à jour des valeurs du tableau de 10 à 19 
 *
 * @return Void
 *  ************************************************************************** */

void vMajTab2(void)
{
    int i;
    for (i=10; i<20; i++) 
    {
        TablePWM[i] =	NextTablePWM[i];				
    }
}

/*! ************************************************************************** *
 * @brief Mise à jour des valeurs du tableau de 10 à 19 
 *
 * @return Void
 *  ************************************************************************** */

void vInitTab(void)
{
    int i;
    for (i=0; i<20; i++) 
    {
        NextTablePWM[i]	= 150;	
        TablePWM[i]	    = 150;				
    }
}


