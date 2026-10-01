/**
 ******************************************************************************
 * @file    oc32_hal.h
 * @author
 * @brief   Header file of OC32 HAL module.
 ******************************************************************************
 */

#ifndef __OC32_HAL_H
#define __OC32_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------*/
#include "oc32_hal_def.h"
#include "oc32_hal_conf.h"

#include "oc32_hal_rcm.h"
#include "oc32_hal_gpio.h"
#include "oc32_hal_flash.h"

#ifdef HAL_LVD_ENABLE
#include "oc32_hal_lvd.h"
#endif

#ifdef HAL_UART_ENABLE
#include "oc32_hal_uart.h"
#endif

#ifdef HAL_SPI_ENABLE
#include "oc32_hal_spi.h"
#endif

#ifdef HAL_I2C_ENABLE
#include "oc32_hal_i2c.h"
#endif

#ifdef HAL_TIM_ENABLE
#include "oc32_hal_tim.h"
#endif

#ifdef HAL_ADC_ENABLE
#include "oc32_hal_adc.h"
#endif

#ifdef HAL_EXTI_ENABLE
#include "oc32_hal_exti.h"
#endif

#ifdef HAL_CCP_ENABLE
#include "oc32_hal_ccp.h"
#endif

#ifdef HAL_WDT_ENABLE
#include "oc32_hal_wdt.h"
#endif

/* Exported types ------------------------------------------------------*/
/* Exported constants --------------------------------------------------*/
/**
 * @}
 */

/* Exported macros -----------------------------------------------------*/
/* Exported functions ----------------------------------------------------*/
static void *memcpy(void *dest, const void *src, uint32_t n)
void *memset(void *dest, int c, uint32_t n)

#ifdef HAL_UART_DEBUG_ENABLE
int puts(const char *s)
int printf(const char *format, ...)
#endif

HAL_StatusTypeDef HAL_SYS_InitTick(uint32_t TickPriority);
uint32_t HAL_SYS_GetTick(void);
uint32_t HAL_SYS_GetTickPrio(void);
void HAL_SYS_IncTick(void);
void HAL_Delay(uint32_t Delay);
void HAL_SYS_SuspendTick(void);
void HAL_SYS_ResumeTick(void);
uint32_t HAL_GetHalVersion(void);
uint32_t HAL_GetRevid(void);
uint32_t HAL_GetDevid(void);
uint32_t HAL_GetUIDw0(void);
uint32_t HAL_GetUIDw1(void);
uint32_t HAL_GetUIDw2(void);
uint32_t HAL_GetUIDw3(void);

/* Syscall numbers for fused data access via SYSC instruction */
#define SYSC_PID      0u /* fused Product ID (DEVID/REVID) */
#define SYSC_LID      1u /* fused Lot ID */
#define SYSC_TSN      2u /* fused Test Serial Number (96-bit UID) */
#define SYSC_REGTRIM0 3u /* fused reg: BPCON/IOSCCON/LVDCON/VTSCON */
#define SYSC_REGTRIM1 4u /* fused reg:  */
#define SYSC_REGTRIM2 5u /* fused reg:  */



/**
 * Current system clock frequency, in Hz.
 * Initialized to OC32_IHOSC_FREQ; call SystemClockUpdate() to refresh.
 */
uint32_t SystemClock = OC32_IHOSC_FREQ * 1000000U;

#ifdef __cplusplus
}
#endif

#endif /* __OC32_HAL_H */
