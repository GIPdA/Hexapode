/*! ************************************************************************** *
 * @file servodriver.h
 * @brief Librairie g�rant 20 sorties servomoteurs.
 *  Les signaux sont g�n�r�s avec le module PWM. Le temps haut est de 1 � 2ms.
 *
 * Les signaux sont g�n�r�s en 2 s�ries (2*10), chaque s�rie via un compare match.
 * Les deux s�ries sont g�n�r�s en asym�trique et simultan�ment :
 *
 *     |  *   *   *   *   *   *   * 2.1ms period
 *     |  *__ *   *
 * S1  |__|  |_________________
 *     |...
 *        *   *   *
 *     |  * __*   *
 * S10 |___|  |_________________
 *     |...
 *
 * La p�riode de 2.1ms permet de laisser un d�lai suppl�mentaire entre les deux signaux
 * pour n'avoir aucun risque d'avoir 2 signaux � traiter en m�me temps.
 * La p�riode est l�g�rement allong�e (21ms) mais cela n'a aucune influence perceptible.
 *
 * @author Benjamin Balga
 * @version 1.0
 *
 *  ************************************************************************** */


#ifndef SERVO_DRIVER_H_
#define SERVO_DRIVER_H_

/*! Uncomment for debug mode: GPIO are not used and prv_axServoOutputs array is used instead.
Declare 'unsigned int prv_axServoOutputs[20] = {0};' in main.c with debug mode use.*/
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


/*!
 * @brief Initialisation des 20 PWM ( r�glage GPIO et timer)
 *
 * @return None
 */
void vServoDriver_init(void);

/*!
 * @brief Set the servo position in microseconds
 *
 * @param u32ServoIndex Servo index (0-19)
 * @param u32PulseWidth Pulse time in microseconds (1000-2000)
 * @return None
 */
void vServoDriver_setServoPosition(unsigned int u32ServoIndex, unsigned int u32PulseWidth);

/*!
 * @brief Set the servo position as angle
 *
 * @param u32ServoIndex Servo index (0-19)
 * @param fAngle Servo angle in range 0-180�
 * @return None
 */
void vServoDriver_setServoAngle(unsigned int u32ServoIndex, float fAngle);


#endif // SERVO_DRIVER_H_
