/*! ************************************************************************** *
 * @file tordriver.h
 * @brief Librairie pour gérer 6 entrées TOR
 *
 *
 * @author Benjamin Balga
 * @version 1.0
 *
 *  ************************************************************************** */

#ifndef TOR_DRIVER_H_
#define TOR_DRIVER_H_

#include <stdint.h>
#include "board.h"

/*! The number of TOR inputs to monitor. You need to modify the lookup table in tordriver.c if you change this number. */
#define TOR_INPUTS_MAX	6

/*! Definitions of each TOR tuple: port & bit index */
#define TOR_SW1_PORT_NUM	0
#define TOR_SW1_BIT_NUM		0

#define TOR_SW2_PORT_NUM	0
#define TOR_SW2_BIT_NUM		1

#define TOR_SW3_PORT_NUM	0
#define TOR_SW3_BIT_NUM		2

#define TOR_SW4_PORT_NUM	0
#define TOR_SW4_BIT_NUM		3

#define TOR_SW5_PORT_NUM	0
#define TOR_SW5_BIT_NUM		4

#define TOR_SW6_PORT_NUM	0
#define TOR_SW6_BIT_NUM		5


/*!
 * @brief Initialize the TOR subsystem (set GPIOs).
 *
 * @return None
 */
void vTOR_init(void);

/*!
 * @brief Read a TOR input
 *
 * @param torNum Index of TOR input to read : [0 ; (TOR_INPUTS_MAX-1)]
 * @return None
 */
bool bTOR_read(uint8_t torNum);

#endif // TOR_DRIVER_H_
