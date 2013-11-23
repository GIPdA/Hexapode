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


#ifndef SERVO_DRIVER_H_
#define SERVO_DRIVER_H_

#define SERVO_DRIVER_DEBUG


#define SERVO1_OUT_PORT	PORT0
#define SERVO1_OUT_PIN	0
#define SERVO2_OUT_PORT	PORT0
#define SERVO2_OUT_PIN	0
#define SERVO3_OUT_PORT	PORT0
#define SERVO3_OUT_PIN	0
#define SERVO4_OUT_PORT	PORT0
#define SERVO4_OUT_PIN	0
#define SERVO5_OUT_PORT	PORT0
#define SERVO5_OUT_PIN	0
#define SERVO6_OUT_PORT	PORT0
#define SERVO6_OUT_PIN	0
#define SERVO7_OUT_PORT	PORT0
#define SERVO7_OUT_PIN	0
#define SERVO8_OUT_PORT	PORT0
#define SERVO8_OUT_PIN	0
#define SERVO9_OUT_PORT	PORT0
#define SERVO9_OUT_PIN	0
#define SERVO10_OUT_PORT	PORT0
#define SERVO10_OUT_PIN	0

#define SERVO11_OUT_PORT	PORT0
#define SERVO11_OUT_PIN	0
#define SERVO12_OUT_PORT	PORT0
#define SERVO12_OUT_PIN	0
#define SERVO13_OUT_PORT	PORT0
#define SERVO13_OUT_PIN	0
#define SERVO14_OUT_PORT	PORT0
#define SERVO14_OUT_PIN	0
#define SERVO15_OUT_PORT	PORT0
#define SERVO15_OUT_PIN	0
#define SERVO16_OUT_PORT	PORT0
#define SERVO16_OUT_PIN	0
#define SERVO17_OUT_PORT	PORT0
#define SERVO17_OUT_PIN	0
#define SERVO18_OUT_PORT	PORT0
#define SERVO18_OUT_PIN	0
#define SERVO19_OUT_PORT	PORT0
#define SERVO19_OUT_PIN	0
#define SERVO20_OUT_PORT	PORT0
#define SERVO20_OUT_PIN	0


void vServoDriver_init(void);

void vServoDriver_setServoPosition(unsigned int u32ServoIndex, unsigned int u32PulseWidth);
void vServoDriver_setServoAngle(unsigned int u32ServoIndex, float fAngle);


#endif // SERVO_DRIVER_H_
