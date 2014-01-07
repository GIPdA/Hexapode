/*! ************************************************************************** *
 * @file tordriver.c
 * @brief Librairie pour gérer 6 entrées TOR
 *
 *
 * @author Benjamin Balga
 * @version 1.0
 *
 *  ************************************************************************** */

#include "tordriver.h"
#include "board.h"

typedef struct {
	uint32_t port;
	uint8_t bit;
} TORInput;

static TORInput prv_u32TORInputsLookup[TOR_INPUTS_MAX] = {
	{ TOR_SW1_PORT_NUM, TOR_SW1_BIT_NUM},
	{ TOR_SW2_PORT_NUM, TOR_SW2_BIT_NUM},
	{ TOR_SW3_PORT_NUM, TOR_SW3_BIT_NUM},
	{ TOR_SW4_PORT_NUM, TOR_SW4_BIT_NUM},
	{ TOR_SW5_PORT_NUM, TOR_SW5_BIT_NUM},
	{ TOR_SW6_PORT_NUM, TOR_SW6_BIT_NUM}
};


void vTOR_init()
{
	// Setup GPIO
	// false = input
	Chip_GPIO_WriteDirBit(LPC_GPIO, TOR_SW1_PORT_NUM, TOR_SW1_BIT_NUM, false);
	Chip_GPIO_WriteDirBit(LPC_GPIO, TOR_SW2_PORT_NUM, TOR_SW2_BIT_NUM, false);
	Chip_GPIO_WriteDirBit(LPC_GPIO, TOR_SW3_PORT_NUM, TOR_SW3_BIT_NUM, false);
	Chip_GPIO_WriteDirBit(LPC_GPIO, TOR_SW4_PORT_NUM, TOR_SW4_BIT_NUM, false);
	Chip_GPIO_WriteDirBit(LPC_GPIO, TOR_SW5_PORT_NUM, TOR_SW5_BIT_NUM, false);
	Chip_GPIO_WriteDirBit(LPC_GPIO, TOR_SW6_PORT_NUM, TOR_SW6_BIT_NUM, false);

	// Setup IOCON Pin Muxer ?
}


bool bTOR_read(uint8_t torNum)
{
	if (torNum >= TOR_INPUTS_MAX) {
		return false;
	}

	return Chip_GPIO_ReadPortBit(LPC_GPIO, prv_u32TORInputsLookup[torNum].port, prv_u32TORInputsLookup[torNum].bit);
}
