#include <lpc17xx.h>
#include "IR_sharp.h"
/*! ************************************************************************** *
 * @file IR_sharp.c
 * @brief Library for infrared telemeter sensor from Sharp.
 *
 *
 * @author Cédric CHRETIEN
 * @version 1.0
 *
 *  ************************************************************************** */


float fIR_sharp_GetValue(uint8_t channel);


/*! ************************************************************************** *
 * @brief Initialisation 
 *
 * @return None
 *  ************************************************************************** */

void vIR_sharp_init(void)
{
    IP_ADC_Init(LPC_ADC,0x8<<8,0,1<<21);
   
}

/*! ************************************************************************** *
 * @brief Get value in centimeters
 *
 * @param channel Channel to use. Currently only channel 0 supported.
 * @return Distance in centimeters
 *  ************************************************************************** */

float fIR_sharp_GetValue(uint8_t channel)
{
	uint32_t  test;
	float calcul;

	IP_ADC_SetChannelNumber	(LPC_ADC,channel,1);

	IP_ADC_SetStartMode	(LPC_ADC, 1);	

	while(!IP_ADC_Get_Val(LPC_ADC,channel,&test));

	calcul=test;
	calcul=calcul/4096*3.3;
	calcul=calcul*0.0416666-0.0075;
	calcul=(1-calcul*0.42)/calcul;

	return calcul;
}
