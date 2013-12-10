/*
 * @brief NXP LPC1769 Xpresso board file
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#ifndef __BOARD_LPC1766STK_H_
#define __BOARD_LPC1766STK_H_

//#include "lpc_types.h"
#include "chip.h"
#include "board_api.h"
#include "lpc_phy.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup BOARD_LPC1766STK NXP LPC1766 Olimex board support functions
 * @ingroup BOARDS_17XX_40XX
 * @{
 */

/** @defgroup BOARD_LPC1766STK_OPTIONS BOARD: NXP LPC1766 Olimex board build options
 * This board has options that configure its operation at build-time.<br>
 *
 * For more information on driver options see<br>
 * @ref LPCOPEN_DESIGN_ARPPROACH<br>
 * @{
 */

/**
 * @}
 */

#define BOARD_LPC1766STK	/*!< Build for LPC1766STK board */

/**
 * LED defines
 */
#define LEDS_LED1           0x01
#define LEDS_LED2           0x02
#define LEDS_NO_LEDS        0x00

/**
 * Button defines
 */
#define BUTTONS_BUTTON1     0x01
#define BUTTONS_BUTTON2     0x02
#define NO_BUTTON_PRESSED   0x00



void Board_UARTPutSTR(char *str);
int Board_UARTGetChar(void);

/**
 * @brief	Set LED state
 * @param	LEDNumber	: LED number to switch
 * @param	On	: true = On
 * @return	Nothing
 */
void Board_LED_Set(uint8_t LEDNumber, bool On);

/**
 * @brief	Get LED state
 * @param	LEDNumber	: LED number to switch
 * @return	true if LED is on, false otherwise
 */
bool Board_LED_Test(uint8_t LEDNumber);

/**
 * @brief	Get button status
 * @return	status of button
 */
uint32_t Buttons_GetStatus(void);


/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_LPC1766STK_H_ */
