/* PWM HEXAPODE */


#ifndef __PWM_HEXA_H__
#define __PWM_HEXA_H__

void vInitPWM(int Frequency); // Frequency in MHz 
void vLoadMR0(int Freq,int PWM_time_x10us); 
void TIM0_IRQHandler(void);


#endif /* PWM_HEXA_H */
