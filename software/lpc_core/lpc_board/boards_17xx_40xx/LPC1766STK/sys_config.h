/*
 * @note
 *
 */

#ifndef __SYS_CONFIG_H_
#define __SYS_CONFIG_H_

/** @ingroup BOARD_LPC1766STK_OPTIONS
 * @{
 */

/* Build for 175x/6x chip family */
#define CHIP_LPC175X_6X

/* Build for RMII interface */
#define USE_RMII

/* Un-comment DEBUG_ENABLE for IO support via the UART */
#define DEBUG_ENABLE

/* Board UART used for debug output */
#define DEBUG_UART LPC_UART1

/* Crystal frequency into device */
#define CRYSTAL_MAIN_FREQ_IN (12000000)

/* Crystal frequency into device for RTC/32K input */
#define CRYSTAL_32K_FREQ_IN (32768)

/**
 * @}
 */

#endif /* __SYS_CONFIG_H_ */
