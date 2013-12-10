/**
 * @brief LPC1766STK Sysinit file
 *
 */

//#include <LPC17xx.H>
#include "board.h"
#include "string.h"


/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/* SCR pin definitions for pin muxing */
typedef struct {
	uint8_t pingrp;		/* Pin group */
	uint8_t pinnum;		/* Pin number */
	uint8_t pincfg;		/* Pin configuration for SCU */
	uint8_t funcnum;	/* Function number */
} PINMUX_GRP_T;

/* Pin muxing configuration */
STATIC const PINMUX_GRP_T pinmuxing[] = {
	{0,  15,  IOCON_MODE_INACT,                   IOCON_FUNC1},	/* TXD1 */
	{0,  16,  IOCON_MODE_INACT,                   IOCON_FUNC1},	/* RXD1 */
	{1,  25,  IOCON_MODE_INACT,                   IOCON_FUNC0},	/* Led 0 */
	{0,  4,   IOCON_MODE_INACT,                   IOCON_FUNC0},	/* Led 1 */
	{0,  23,  IOCON_MODE_INACT,                   IOCON_FUNC0},	/* BUT 1 */
	{2,  13,  IOCON_MODE_INACT,                   IOCON_FUNC0}	/* BUT 2 */
};

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Setup system clocking */
STATIC void SystemSetupClocking(void)
{
	/* CPU clock source starts with IRC */
	Chip_Clock_SetMainPllSource(SYSCTL_PLLCLKSRC_IRC);
	Chip_Clock_SetCPUClockSource(SYSCTL_CCLKSRC_SYSCLK);

	/* Enable main oscillator used for PLLs */
	LPC_SYSCTL->SCS = SYSCTL_OSCEC;
	while ((LPC_SYSCTL->SCS & SYSCTL_OSCSTAT) == 0) {}

	/* PLL0 clock source is 12MHz oscillator, PLL1 can only be the
	   main oscillator */
	Chip_Clock_SetMainPllSource(SYSCTL_PLLCLKSRC_MAINOSC);

	/* Setup PLL0 for a 480MHz clock. It is divided by CPU Clock Divider to create CPU Clock.
	   Input clock rate (FIN) is main oscillator = 12MHz
	   FCCO is selected for PLL Output and it must be between 275 MHz to 550 MHz.
	   FCCO = (2 * M * FIN) / N = integer multiplier of CPU Clock (120MHz) = 480MHz
	   N = 1, M = 480 * 1/(2*12) = 20 */
	Chip_Clock_SetupPLL(SYSCTL_MAIN_PLL, 19, 0);/* Multiply by 20, Divide by 1 */

	/* Enable PLL0 */
	Chip_Clock_EnablePLL(SYSCTL_MAIN_PLL, SYSCTL_PLL_ENABLE);

	/* Change the CPU Clock Divider setting for the operation with PLL0.
	   Divide value = (480/120) = 4 */
	Chip_Clock_SetCPUClockDiv(3);	/* pre-minus 1 */

	/* Change the USB Clock Divider setting for the operation with PLL0.
	   Divide value = (480/48) = 10 */
	//Chip_Clock_SetUSBClockDiv(9);	/* pre-minus 1 */

	/* Wait for PLL0 to lock */
	while (!Chip_Clock_IsMainPLLLocked()) {}

	/* Connect PLL0 */
	Chip_Clock_EnablePLL(SYSCTL_MAIN_PLL, SYSCTL_PLL_ENABLE | SYSCTL_PLL_CONNECT);

	/* Wait for PLL0 to be connected */
	while (!Chip_Clock_IsMainPLLConnected()) {}

	/* Setup USB PLL1 for a 48MHz clock
	   Input clock rate (FIN) is main oscillator = 12MHz
	   PLL1 Output = USBCLK = 48MHz = FIN * MSEL, so MSEL = 4.
	   FCCO = USBCLK = USBCLK * 2 * P. It must be between 156 MHz to 320 MHz.
	   so P = 2 and FCCO = 48MHz * 2 * 2 = 192MHz */
	//Chip_Clock_SetupPLL(SYSCTL_USB_PLL, 3, 1);	/* Multiply by 4, Divide by 2 */

#if 0	/* Use PLL1 output as USB Clock Source */
		/* Enable PLL1 */
	Chip_Clock_EnablePLL(SYSCTL_USB_PLL, SYSCTL_PLL_ENABLE);

	/* Wait for PLL1 to lock */
	while (!Chip_Clock_IsUSBPLLLocked()) {}

	/* Connect PLL1 */
	Chip_Clock_EnablePLL(SYSCTL_USB_PLL, SYSCTL_PLL_ENABLE | SYSCTL_PLL_CONNECT);

	/* Wait for PLL1 to be connected */
	while (!Chip_Clock_IsUSBPLLConnected()) {}
#endif

	/* Setup FLASH access to 5 clocks (120MHz clock) */
	//Chip_FMC_SetFLASHAccess(FLASHTIM_120MHZ_CPU);
}

/* Sets up system pin muxing */
STATIC void SystemSetupMuxing(void)
{
	int i;

	/* Setup system level pin muxing */
	for (i = 0; i < (sizeof(pinmuxing) / sizeof(pinmuxing[0])); i++) {
		Chip_IOCON_PinMux(LPC_IOCON, pinmuxing[i].pingrp, pinmuxing[i].pinnum,
						  pinmuxing[i].pincfg, pinmuxing[i].funcnum);
	}
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	Setup the system
 * @return	none
 * @note	SystemInit() is called prior to the application and sets up system
 *			clocking, memory, and any resources needed prior to the application
 *			starting.
 */
void SystemInit(void)
{
	unsigned int *pSCB_VTOR = (unsigned int *) 0xE000ED08;

#if defined(__IAR_SYSTEMS_ICC__)
	extern void *__vector_table;

	*pSCB_VTOR = (unsigned int) &__vector_table;
#elif defined(__CODE_RED)
	extern void *g_pfnVectors;

	*pSCB_VTOR = (unsigned int) &g_pfnVectors;
#elif defined(__ARMCC_VERSION)
	extern void *__Vectors;

	*pSCB_VTOR = (unsigned int) &__Vectors;
#endif

	/* Setup system clocking and memory. This is done early to allow the
	   application and tools to clear memory and use scatter loading to
	   external memory. */
	SystemSetupClocking();
	SystemSetupMuxing();
}

/**
 * @}
 */
