/**
 ******************************************************************************
 * @file    oc32_hal_wdt.h
 * @author
 * @brief   Header file of WDT HAL module.
 ******************************************************************************
 */

#ifndef __OC32_HAL_WDT_H
#define __OC32_HAL_WDT_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------*/
#include "oc32_hal_def.h"
#include "oc32_hal_conf.h"

  /* extern ------------------------------------------------------------*/

  /** @addtogroup OC32_HAL_Driver
   * @{
   */

  /** @addtogroup WDT
   * @{
   */

  /* Exported types ------------------------------------------------------*/

  /**
   * @brief  WDT Mode definition
   */
  typedef enum
  {
    WDT_MODE_NONE = 0x0U,
    WDT_MODE_RST = 0x1U,
    WDT_MODE_WAKE = 0x2U,
    WDT_MODE_INT = 0x3U,
    WDT_MODE_INT_AND_WAKE = 0x4U
  } HAL_WDT_ModeTypedef;

  /**
   * @brief  WDT Init Structure definition
   */
  typedef struct
  {
    MODULE_WDT_Typedef *Wdt;  /*!< Specifies the watchdogtimer */
    HAL_WDT_ModeTypedef Mode; /*!< Specifies the Mode: INT or None */
    uint32_t Period;          /*!< Specifies the overflow period (us) */
  } HAL_WDT_InitTypedef;

/**
 * @}
 */

/* Exported constants --------------------------------------------------*/

/**
 * @brief This is the HAL system configuration section
 */
#define WDT_PERIOD_LIMIT_MARGIN 1U                                                                 /*!< WDT period limit margin: 1us for safety boundary check */
#define WDT_H_PERIOD_MIN (((4U * 32U * 1000000U) / OC32_IHOSC_FREQ) + WDT_PERIOD_LIMIT_MARGIN)     /*!< WDT min period at high clock in us*/
#define WDT_H_PERIOD_MAX (((4U * 262144U * 1000000U) / OC32_IHOSC_FREQ) - WDT_PERIOD_LIMIT_MARGIN) /*!< WDT max period at high clock in us*/
#define WDT_L_PERIOD_MIN (((4U * 32U * 1000000U) / OC32_ILOSC_FREQ) + WDT_PERIOD_LIMIT_MARGIN)     /*!< WDT min period at low clock in us*/
#define WDT_L_PERIOD_MAX (((4U * 262144U * 1000000U) / OC32_ILOSC_FREQ) - WDT_PERIOD_LIMIT_MARGIN) /*!< WDT max period at low clock in us*/

#define WDT_CLR_KEY 0xA5U /*!< WDT clear/refresh key value */

  /**
   * @}
   */

  /* Exported macros -------------------------------------------------------*/

  /* Exported functions ----------------------------------------------------*/

  HAL_StatusTypedef HAL_WDT_Init(HAL_WDT_InitTypedef *Init);
  HAL_StatusTypedef HAL_WDT_DeInit(MODULE_WDT_Typedef *Wdt);
  __sram__ HAL_StatusTypedef HAL_WDT_Clear(MODULE_WDT_Typedef *Wdt);

  #ifdef HAL_WDT_ENABLE
  HAL_StatusTypedef HAL_WDT_IRQHandler(void);
  __weak__ HAL_StatusTypedef HAL_WDT_UserIRQ(void);
  #endif

  /**
   * @}
   */

#ifdef __cplusplus
}
#endif

#endif /* __OC32_HAL_WDT_H */
