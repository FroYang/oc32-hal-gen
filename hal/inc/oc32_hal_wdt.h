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

  /** @addtogroup OC32_HAL_Driver
   * @{
   */

  /** @addtogroup WDT
   * @{
   */

  /* Exported types ------------------------------------------------------*/

  /** @defgroup WDT_Mode
   * @{
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
    MODULE_WDT_TypeDef *Wdt;   /*!< Specifies the watchdogtimer */
    HAL_WDT_ModeTypedef Mode;  /*!< Specifies the Mode: INT or None */
    uint32_t Period;          /*!< Specifies the overflow period (us) */
  } HAL_WDT_InitTypeDef;

/**
 * @}
 */

/* Exported constants --------------------------------------------------*/

/**
 * @brief This is the HAL system configuration section
 */
#define WDT_H_PERIOD_MIN ((4U * 32U * 1000000U) / OC32_IHOSC_FREQ)     /*!< WDT min period at high clock in us*/
#define WDT_H_PERIOD_MAX ((4U * 262144U * 1000000U) / OC32_IHOSC_FREQ) /*!< WDT max period at high clock in us*/
#define WDT_L_PERIOD_MIN ((4U * 32U * 1000000U) / OC32_ILOSC_FREQ)     /*!< WDT min period at low clock in us*/
#define WDT_L_PERIOD_MAX ((4U * 262144U * 1000000U) / OC32_ILOSC_FREQ) /*!< WDT max period at low clock in us*/

#define WDT_CLR_KEY 0xA5U /*!< WDT clear/refresh key vaule */

  /**
   * @}
   */

  /* Exported macros -------------------------------------------------------*/

  /* Exported functions ----------------------------------------------------*/

  HAL_StatusTypeDef HAL_WDT_Init(HAL_WDT_InitTypeDef *Init);
  HAL_StatusTypeDef HAL_WDT_DeInit(MODULE_WDT_TypeDef *Wdt);
  __sram__ HAL_StatusTypeDef HAL_WDT_Clear(MODULE_WDT_TypeDef *Wdt);
  HAL_StatusTypeDef HAL_WDT_IRQHandler(MODULE_WDT_TypeDef *Wdt);
  __weak__ HAL_StatusTypeDef HAL_WDT_UserIRQ(MODULE_WDT_TypeDef *Wdt);

  /**
   * @}
   */

#ifdef __cplusplus
}
#endif

#endif /* __OC32_HAL_WDT_H */
