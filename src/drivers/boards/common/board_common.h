/****************************************************************************
 *
 *   Copyright (C) 2016 PX4 Development Team. All rights reserved.
 *                 Author: David Sidrane <david_s5@nscdg.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file board_common.h
 *
 * Provide the the common board interfaces
 */

#pragma once

/************************************************************************************
 * Included Files
 ************************************************************************************/
#include <errno.h>
/************************************************************************************
 * Definitions
 ************************************************************************************/

/* SPI bus defining tools
 * For new boards we use a board_config.h to define all the SPI functionality
 *  A board provides SPI bus definitions and a set of buses that should be
 *  enumerated as well as chip selects that will be interatorable
 *
 * We will use these in macros place of the spi_dev_e enumeration to select a
 * specific SPI device on given SPI1 bus.
 *
 * These macros will define BUS:DEV For clarity and indexing
 *
 * The board config then defines:
 * 1) PX4_SPI_BUS_xxx Ids -the buses as a 1 based PX4_SPI_BUS_xxx as n+1 aping to SPI n
 *       where n is {1-highest SPI supported by SoC}
 * 2) PX4_SPIDEV_yyyy handles - PX4_SPIDEV_xxxxx handles using the macros below.
 * 3) PX4_xxxx_BUS_CS_GPIO - a set of chip selects that are indexed by the handles. and of set to 0 are
 *       ignored.
 * 4) PX4_xxxx_BUS_FIRST_CS and PX4_xxxxx_BUS_LAST_CS as  PX4_SPIDEV_lll for the first CS and
 *       PX4_SPIDEV_hhhh as the last CS
 *
 * Example:
 *
 * The PX4_SPI_BUS_xxx
 * #define PX4_SPI_BUS_SENSORS  1
 * #define PX4_SPI_BUS_MEMORY   2
 *
 * the PX4_SPIDEV_yyyy
 * #define PX4_SPIDEV_ICM_20689      PX4_MK_SPI_SEL(PX4_SPI_BUS_SENSORS,0)
 * #define PX4_SPIDEV_ICM_20602      PX4_MK_SPI_SEL(PX4_SPI_BUS_SENSORS,1)
 * #define PX4_SPIDEV_BMI055_GYRO    PX4_MK_SPI_SEL(PX4_SPI_BUS_SENSORS,2)
 *
 * The PX4_xxxx_BUS_CS_GPIO
 * #define PX4_SENSOR_BUS_CS_GPIO    {GPIO_SPI_CS_ICM20689, GPIO_SPI_CS_ICM20602, GPIO_SPI_CS_BMI055_GYR,...
 *
 * The PX4_xxxx_BUS_FIRST_CS and PX4_xxxxx_BUS_LAST_CS
 * #define PX4_SENSORS_BUS_FIRST_CS  PX4_SPIDEV_ICM_20689
*  #define PX4_SENSORS_BUS_LAST_CS   PX4_SPIDEV_BMI055_ACCEL
 *
 *
 */

#define PX4_MK_SPI_SEL(b,d)       ((((b) & 0xf) << 4) + ((d) & 0xf))
#define PX4_SPI_BUS_ID(bd)        (((bd) >> 4) & 0xf)
#define PX4_SPI_DEV_ID(bd)        ((bd) & 0xf)

/* ADC defining tools
 * We want to normalize the V5 Sensing to V = (adc_dn) * ADC_V5_V_FULL_SCALE/(2 ^ ADC_BITS) * ADC_V5_SCALE)
 */

/* Provide overrideable defaults ADC Full scale ranges and Divider ratios
 * If the board has a different ratio or full scale range for any voltage sensing
 * the board_congig.h file should define the constants that differ from these
 * defaults
 */
#if !defined(ADC_V5_V_FULL_SCALE)
#define ADC_V5_V_FULL_SCALE             (6.6f)  // 5 volt Rail full scale voltage
#endif
#if !defined(ADC_V5_SCALE)
#define ADC_V5_SCALE                    (2.0f) // The scale factor defined by HW's resistive divider (Rt+Rb)/ Rb
#endif

#if !defined(ADC_3V3_V_FULL_SCALE)
#define ADC_3V3_V_FULL_SCALE             (3.6f)  // 3.3V volt Rail full scale voltage
#endif
#if !defined(ADC_3V3_SCALE)
#define ADC_3V3_SCALE                    (2.0f) // The scale factor defined by HW's resistive divider (Rt+Rb)/ Rb
#endif

/* Choose the source for ADC_SCALED_V5_SENSE */

#if defined(ADC_5V_RAIL_SENSE)
#define ADC_SCALED_V5_SENSE ADC_5V_RAIL_SENSE
#else
#  if defined(ADC_SCALED_V5)
#    define ADC_SCALED_V5_SENSE ADC_SCALED_V5
#  endif
#endif

/* Define an overridable default of 0.0f V for batery v div
 * This is done to ensure the missing default trips a low
 * voltage lockdown
 */
#if !defined(BOARD_BATTERY1_V_DIV)
#define BOARD_BATTERY1_V_DIV 0.0f
#endif

#if !defined(BOARD_BATTERY2_V_DIV)
#define BOARD_BATTERY2_V_DIV 0.0f
#endif

/* Define an overridable default of 0.0f for A per V
 * This is done to ensure the default leads to an
 * unrealistic current value
 */
#if !defined(BOARD_BATTERY1_A_PER_V)
#define BOARD_BATTERY1_A_PER_V 0.0f
#endif

#if !defined(BOARD_BATTERY2_A_PER_V)
#define BOARD_BATTERY2_A_PER_V 0.0f
#endif

/* Conditional use of FMU GPIO
 * If the board use the PX4FMU driver and the board provides
 * BOARD_FMU_GPIO_TAB then we publish the logical BOARD_HAS_FMU_GPIO
 */
#if defined(BOARD_FMU_GPIO_TAB)
#  define BOARD_HAS_FMU_GPIO
#endif

/* Conditional use of PX4 PIO is Used to determine if the board
 * has a PX4IO processor.
 * We then publish the logical BOARD_USES_PX4IO
 */
#if defined(BOARD_USES_PX4IO_VERSION)
#  define BOARD_USES_PX4IO	1
/*  Allow a board_config to override the PX4IO FW search paths */
#  if defined(BOARD_PX4IO_FW_SEARCH_PATHS)
#    define PX4IO_FW_SEARCH_PATHS BOARD_PX4IO_FW_SEARCH_PATHS
#  else
/*  Use PX4IO FW search paths defaults based on version */
#    if BOARD_USES_PX4IO_VERSION == 1
#      define PX4IO_FW_SEARCH_PATHS {"/etc/extras/px4io-v1.bin", "/fs/microsd/px4io1.bin", "/fs/microsd/px4io.bin", nullptr }
#    endif
#    if BOARD_USES_PX4IO_VERSION == 2
#      define PX4IO_FW_SEARCH_PATHS {"/etc/extras/px4io-v2.bin", "/fs/microsd/px4io2.bin", "/fs/microsd/px4io.bin", nullptr }
#    endif
#  endif
#endif

/* Provide an overridable default nop
 * for BOARD_EEPROM_WP_CTRL
 */
#if !defined(BOARD_EEPROM_WP_CTRL)
#  define BOARD_EEPROM_WP_CTRL(on_true)
#endif


/************************************************************************************
 * Public Data
 ************************************************************************************/
typedef enum board_reset_e {
	board_reset_normal     = 0,
	board_reset_extended     = 1,
	board_reset_enter_bootloader = 2
} board_reset_e;
/************************************************************************************
 * Private Functions
 ************************************************************************************/

/************************************************************************************
 * Public Functions
 ************************************************************************************/

/************************************************************************************
 * Name: board_dma_alloc_init
 *
 * Description:
 *   All boards may optionally provide this API to instantiate a pool of
 *   memory for uses with FAST FS DMA operations.
 *
 *   Provision is controlled by declaring BOARD_DMA_ALLOC_POOL_SIZE in board_config.h
 *
 *
 ************************************************************************************/
#if defined(BOARD_DMA_ALLOC_POOL_SIZE)
__EXPORT int board_dma_alloc_init(void);
#else
#define board_dma_alloc_init() (-EPERM)
#endif

/************************************************************************************
 * Name: board_get_dma_usage
 *
 * Description:
 *   All boards may optionally provide this API to supply instrumentation for a pool of
 *   memory used for DMA operations.
 *
 *   Provision is controlled by declaring BOARD_DMA_ALLOC_POOL_SIZE in board_config.h
 *
 *
 ************************************************************************************/
#if defined(BOARD_DMA_ALLOC_POOL_SIZE)
__EXPORT int board_get_dma_usage(uint16_t *dma_total, uint16_t *dma_used, uint16_t *dma_peak_used);
#else
#define board_get_dma_usage(dma_total,dma_used, dma_peak_used) (-ENOMEM)
#endif

/************************************************************************************
 * Name: board_rc_input
 *
 * Description:
 *   All boards my optionally provide this API to invert the Serial RC input.
 *   This is needed on SoCs that support the notion RXINV or TXINV as apposed to
 *   and external XOR controlled by a GPIO
 *
 ************************************************************************************/
#if defined(INVERT_RC_INPUT)
#  if !defined(GPIO_SBUS_INV)
__EXPORT void board_rc_input(bool invert_on);
#  endif
#endif

/************************************************************************************
 * Name: board_reset
 *
 * Description:
 *   All boards my optionally provide this API to reset the board
 *
 ************************************************************************************/
#if defined(BOARD_HAS_NO_RESET)
#  define board_system_reset(status)
#else
__EXPORT void board_system_reset(int status) noreturn_function;
#endif
/************************************************************************************
 * Name: board_set_bootload_mode
 *
 * Description:
 *   All boards my optionally provide this API to enter configure the entry to
 *   boot loade mode on the next system reset.
 *
 ************************************************************************************/

#if defined(BOARD_HAS_NO_BOOTLOADER)
#  define board_set_bootload_mode(mode)
#else
__EXPORT int board_set_bootload_mode(board_reset_e mode);
#endif
