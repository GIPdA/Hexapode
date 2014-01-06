/*! ************************************************************************** *
 * @file IR_sharp.h
 * @brief Library for infrared telemeter sensor from Sharp.
 *
 *
 * @author Cédric CHRETIEN
 * @version 1.0
 *
 *  ************************************************************************** */


#ifndef IR_sharp_H_
#define IR_sharp_H_

#include <stdint.h>

void vIR_sharp_init(void);
float fIR_sharp_GetValue(uint8_t channel);


#endif // IR_sharp_H_
