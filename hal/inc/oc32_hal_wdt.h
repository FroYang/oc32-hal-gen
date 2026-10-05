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
  } HAL_WDTModeTypedef;

  /**
   * @brief  WDT Init Structure definition
   */
  typedef struct
  {
    WDTCON_TypeDef *WDTCON;   /*!< Specifies which WDTCON */
    HAL_WDTModeTypedef Mode;  /*!< Specifies the Mode */
    uint32_t Period;          /*!< Specifies the overflow period (us) */
    HAL_INTPriTypedef IntPri; /*!< Specifies the interrupt priority */
  } HAL_WDTInitTypeDef;

/**
 * @}
 */

/* Exported constants --------------------------------------------------*/

/**
 * @brief This is the HAL system configuration section
 */
#define WDT_H_PERIOD_MIN (1000000U / (OC32_IHOSC_FREQ / (4U * 32U)))     /*!< WDT min period at high clock in us*/
#define WDT_H_PERIOD_MAX (1000000U / (OC32_IHOSC_FREQ / (4U * 262144U))) /*!< WDT max period at high clock in us*/
#define WDT_L_PERIOD_MIN (1000000U / (OC32_ILOSC_FREQ / (4U * 32U)))     /*!< WDT min period at low clock in us*/
#define WDT_L_PERIOD_MAX (1000000U / (OC32_IHOSC_FREQ / (4U * 262144U))) /*!< WDT max period at low clock in us*/

#define WDT_CLR_KEY 0xA5U /*!< WDT clear/refresh key vaule */

  /**
   * @}
   */

  /* Exported macros -------------------------------------------------------*/

  /* Exported functions ----------------------------------------------------*/

  HAL_StatusTypeDef HAL_WDT_Init(HAL_WDTInitTypeDef *Init);
  HAL_StatusTypeDef HAL_WDT_DeInit(void);
  HAL_StatusTypeDef HAL_WDT_Clear(HAL_WDTInitTypeDef *Clear);
      __weak__ void HAL_WDT_IRQHandler();

  /**
   * @}
   */

#ifdef __cplusplus
}
#endif

#endif /* __OC32_HAL_WDT_H */
