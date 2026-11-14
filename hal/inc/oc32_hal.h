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
extern "C"
{
#endif

/* Includes ------------------------------------------------------------*/
#include "oc32_hal_def.h"
#include "oc32_hal_conf.h"

  /* Exported constants --------------------------------------------------*/

  /* Exported types ------------------------------------------------------*/

  typedef __builtin_va_list va_list;

#define va_start(v, l) __builtin_va_start(v, l)
#define va_end(v) __builtin_va_end(v)
#define va_arg(v, l) __builtin_va_arg(v, l)
#define va_copy(d, s) __builtin_va_copy(d, s)

  /* Exported macros -----------------------------------------------------*/
  /* Exported functions ----------------------------------------------------*/
  void *memcpy(void *dest, const void *src, uint32_t n);
  void *memset(void *dest, int c, uint32_t n);

  __sram__ HAL_StatusTypedef DmaUpdate(HAL_EnableTypedef DMAE, HAL_DMA_ChTypedef DmaCh, HAL_DMA_MapTypedef Module, HAL_DMA_StateTypedef State);

  HAL_StatusTypedef HAL_TICK_Init(void);
  HAL_StatusTypedef HAL_HIC_Init(HAL_HICInitTypedef *Init);
  void HAL_HIC_DeInit(void);

#define HAL_TICK_Inc() \
  do                   \
  {                    \
    uwTick++;          \
  } while (0)
#define HAL_TICK_GetVal() (uwTick)
#define HAL_GetHalVer() (__OC32_HAL_VERSION)

  void HAL_Delay(uint32_t Delay);

  __weak__ HAL_StatusTypedef HAL_POR_UserIRQ(void);
  __weak__ HAL_StatusTypedef HAL_DBGR_UserIRQ(void);
  HAL_StatusTypedef HAL_HRF_IRQHandler(void);

  /* extern var */
  extern uint32_t SystemClock;
  extern uint32_t DMAChState[OC32_DMA_CH_TOTAL];
  extern uint32_t SysReqBuf[4];
  extern uint32_t VtIndex;

/* analogs modules */
#include "oc32_hal_lvd.h"
#include "oc32_hal_vts.h"
#include "oc32_hal_amp.h"
#include "oc32_hal_cmp.h"
#include "oc32_hal_dac.h"
#include "oc32_hal_adc.h"

/* system base modules */
#include "oc32_hal_rcm.h"
#include "oc32_hal_gpio.h"

/* digital modules */
#include "oc32_hal_wdt.h"
#include "oc32_hal_flash.h"
#include "oc32_hal_crc.h"
#include "oc32_hal_uart.h"
#include "oc32_hal_timer.h"
#include "oc32_hal_ccp.h"
#include "oc32_hal_spi.h"
#include "oc32_hal_iic.h"
#include "oc32_hal_mpwm.h"

#ifdef HAL_UART_DEBUG_ENABLE
  __weak__ int puts(const char *s);
  __weak__ int printf(const char *format, ...);
  HAL_StatusTypedef HAL_DEBUG_Init(void);
#endif

  __weak__ HAL_StatusTypedef HAL_SYS_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __OC32_HAL_H */
