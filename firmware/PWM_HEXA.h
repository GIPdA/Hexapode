/*! ************************************************************************** *
 * @file PWM_HEXA.h
 * @brief librairie de controle hexapode
 *
 *
 *
 * @author Cédric CHRETIEN
 * @version 1.0
 *
 * @changelog 17-11-2013
 *
 *  ************************************************************************** */


#ifndef __PWM_HEXA_H__
#define __PWM_HEXA_H__

void vInitPWM(int Frequency); // Frequency in MHz 
void vLoadMR0(int Compteur_PWM1); 
void TIM0_IRQHandler(void);
void TIM1_IRQHandler(void);
void Setup_PWM(int pin, int Valeurs);
void vMajTab1(void);
void vMajTab2(void);
void vInitTab(void);


#endif /* PWM_HEXA_H */
