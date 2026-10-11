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
  typedef struct
  {
    uint32_t ChEnable;       /* specify interrupt enable for each channel */
    uint32_t ChPriorityBit0; /* specify interrupt priority for each channel */
    uint32_t ChPriorityBit1; /* specify interrupt priority for each channel */
  } HAL_HICInitTypeDef;

  /**
   * @brief  result structure definition
   */
  typedef struct
  {
    uint32_t Data;           /*!< Specifies Data buffer */
    HAL_StatusTypeDef Error; /*!< Specifies the status */
  } HAL_ResultTypedef;

  typedef __builtin_va_list va_list;

#define va_start(v, l) __builtin_va_start(v, l)
#define va_end(v) __builtin_va_end(v)
#define va_arg(v, l) __builtin_va_arg(v, l)
#define va_copy(d, s) __builtin_va_copy(d, s)

  /* Exported macros -----------------------------------------------------*/
  /* Exported functions ----------------------------------------------------*/
  void *memcpy(void *dest, const void *src, uint32_t n);
  void *memset(void *dest, int c, uint32_t n);

#ifdef HAL_UART_DEBUG_ENABLE
  int puts(const char *s);
  int printf(const char *format, ...);
#endif

  HAL_StatusTypeDef HAL_TICK_Init(void);
  HAL_StatusTypeDef HAL_HIC_Init(HAL_HICInitTypeDef *Init);
  void HAL_HIC_DeInit(void);

#define HAL_TICK_Inc() \
  do                   \
  {                    \
    uwTick++;          \
  } while (0)
#define HAL_TICK_GetVal() (uwTick)
#define HAL_GetHalVer() (__OC32_HAL_VERSION)

  void HAL_Delay(uint32_t Delay);
  __sram__ HAL_StatusTypeDef HAL_DMA_Update(HAL_DMAChTypedef DmaCh, HAL_DMAMapTypedef Module, HAL_DMAStateTypedef State);
  int __syscall_handler(uint32_t Num);
  int __systrap_handler(void);
  uint32_t HAL_GetRevID(void);
  uint32_t HAL_GetDevID(void);
  uint32_t HAL_GetUIDw0(void);
  uint32_t HAL_GetUIDw1(void);
  uint32_t HAL_GetUIDw2(void);
  uint32_t HAL_GetUIDw3(void);
  uint32_t HAL_GetRegTrim0w0(void);
  uint32_t HAL_GetRegTrim0w1(void);
  uint32_t HAL_GetRegTrim0w2(void);
  uint32_t HAL_GetRegTrim0w3(void);
  __sram__ __weak__ HAL_StatusTypeDef HAL_POR_UserIRQ(void);
  __sram__ __weak__ HAL_StatusTypeDef HAL_DBGR_UserIRQ(void);
  __sram__ HAL_StatusTypeDef HAL_HRF_IRQHandler(void);

/* Syscall numbers for fused data access via SYSC instruction */
#define SYSC_PID 0u      /* fused Product ID (DEVID/REVID) */
#define SYSC_LID 1u      /* fused Lot ID */
#define SYSC_TSN 2u      /* fused Test Serial Number (96-bit UID) */
#define SYSC_REGTRIM0 3u /* fused reg: BPCON/IOSCCON/LVDCON/VTSCON */
#define SYSC_REGTRIM1 4u /* fused reg:  */
#define SYSC_REGTRIM2 5u /* fused reg:  */

  /* extern var */
  extern uint32_t SystemClock;
  extern uint32_t DMAChState[OC32_DMA_CH_TOTAL];

#include "oc32_hal_rcm.h"
#include "oc32_hal_gpio.h"
#include "oc32_hal_wdt.h"
#include "oc32_hal_flash.h"
#include "oc32_hal_crc.h"
#include "oc32_hal_lvd.h"
#include "oc32_hal_uart.h"

#ifdef __cplusplus
}
#endif

#endif /* __OC32_HAL_H */
